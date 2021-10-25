/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x_conf.h"
#include "mdisp.h"
#include "polar2decart.h"
#include "get_wave.h"
#include "mover.h"

#include "bus_i2c_hard.h"
#include "bus_i2c_soft.h"
#include "bus_spi_soft.h"
#include "mdisp_device_i2c.h"
#include "mdisp_device_spi.h"
#include "bus_spi_hard_dma.h"

const struct i2c_pins i2c_pins1 =
{
	{GPIOB, 14},  // SDA
	{GPIOB, 15}   // SCL
};

const struct i2c_pins i2c_pins2 =
{
	{GPIOB, 7},  // SDA
	{GPIOB, 6}   // SCL
};

const struct spi_pins spi_pins1 =
{
	{GPIOA,  8},		//	SCK;   // clock
	{GPIOA,  9},		//	MOSI;  // data out
	{0},            //	MISO;  // data in
//	{GPIOA, 12} 		//	RST;   // reset
};

const struct pin_def spi1_DC_pin = {GPIOA, 11};		//	DC;   // data/command
const struct pin_def spi1_CS_pin = {GPIOA, 10};		//	CS;   // chip select
const struct pin_def spi1_RST_pin = {GPIOA, 12};	//	RST;  // reset

int tmr1s;
volatile bool pass_1sec = false;
volatile bool flash_en = false;
//--------------------------------------------------------------------------------------------------
extern "C"
{
	void SysTick_Handler(void)
	{
		tmr1s++;
		if(tmr1s >= 100)
		{
			flash_en = pass_1sec = true;
			tmr1s -= 100;
		}
		else if(tmr1s == 25 || tmr1s == 50 || tmr1s ==75)
		{
			flash_en = true;
		}
	}
}
//--------------------------------------------------------------------------------------------------

#define DW 128
#define DH 64

void add_offset(point *dst, point *src, int point_num, int16_t xo, int16_t yo)
{
	for(int i = 0; i < point_num; i++)
	{
		dst[i].x = src[i].x + xo;
		dst[i].y = src[i].y + yo;
	}
}

//--------------------------------------------------------------------------------------------------
void int2strdot(char *dst, int val, int num, int dot, char trailing_0)
{
	// преобразуем целое val в строку dst типа 1234.567
	// num - кол-во символов
	// dot - сколько разрядов после точки (-1, чтоб без точки)
	// trailing_0 - какие символы вставлять вместо первых нулей

	for(int pos = num - 1; pos >= 0; pos--)
	{
		if(dot-- == 0)
		{
			dst[pos] = '.';
		}
		else
		{
			if(val || dot > 0)
			{
				dst[pos] = '0' + val % 10;
				val /= 10;
			}
			else
			{
				// val == 0 - заполняем trailing_0
				dst[pos] = trailing_0;
			}
		}
	}
	//dst[num] = 0; // EOL

}
//--------------------------------------------------------------------------------------------------

int main(void)
{

 	RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_SPI1EN;
 	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
 	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_I2C2EN | RCC_APB1ENR_SPI2EN;


 	SysTick_Config(SystemCoreClock / 100);

	Mdisp * disp[4];

	// ****  make buses  ****
	BusI2cHard bus_i2c1(I2C1, 10);     // HW I2C
	BusI2cHard bus_i2c2(I2C2, 10);     // HW I2C
	//BusI2cSoft bus_i2c1(i2c_pins2);  // SW I2C

	BusSpiHardDma bus_spi1(SPI2, 2);
	//BusSpiSoft bus_spi1(spi_pins1);

	// ****  make displays  ****
	MdispDeviceSpi disp_dev0(bus_spi1, spi1_CS_pin, spi1_DC_pin, spi1_RST_pin);
	disp[0] = new Mdisp(DW, DH, disp_dev0, SSD1306_init_sequence, sizeof(SSD1306_init_sequence)); // создаём объект-дисплей 0

	MdispDeviceI2c disp_dev1(bus_i2c2, 0x3c);
	disp[1] = new Mdisp(DW, DH, disp_dev1, SSD1306_init_sequence, sizeof(SSD1306_init_sequence)); // создаём объект-дисплей 1

	MdispDeviceI2c disp_dev2(bus_i2c1, 0x3c);
	disp[2] = new Mdisp(DW, DH, disp_dev2, SSD1306_init_sequence, sizeof(SSD1306_init_sequence)); // создаём объект-дисплей 2

	MdispDeviceI2c disp_dev3(bus_i2c1, 0x3d);
	disp[3] = new Mdisp(DW, DH, disp_dev3, SSD1306_init_sequence, sizeof(SSD1306_init_sequence)); // создаём объект-дисплей 3

	for(unsigned int ii = 0; ii < NUMOFARRAY(disp); ii++)
	{
		disp[ii]->OverwriteMode(MDISP_FILL_XOR);
	}

	polar_point long_triangle_p[] =        // triangle vertexes
	{
		{500, 0xffffffff / 3 * 0},
		{20,  0xffffffff / 3 * 1},
		{20,  0xffffffff / 3 * 2},
	};

	PolarToDecart LongTriangle(long_triangle_p, NUMOFARRAY(long_triangle_p));   // triangle with polar coordinates
	point dp3[3];     // triangle dest. points (vertexes)
	LongTriangle.SetRotate(0xfc000000);

	disp[2]->triangle
	(
		DW / 2, -10,
		DW / 2 - 20, DH - 1,
		DW / 2 + 20, DH - 1,
		1, 1
	);

	disp[3]->rect(0, 1, 3, 4, 1, 1);
	//int o = 15;
	//disp[3]->rect(1 + o, 1, 5 + o, 5, 1, 1);

	point screen_range[2] = {{0,0}, {DW * 4 - 1, DH - 1}};

	const int R = 29;

	Mover cyrcle_mover(screen_range, 0x10000, 0x03000, DW/2, DH/2);
	point cycrle_size[2] = { {-R, -R}, {R, R} };
	cycrle_size[0].x = cycrle_size[0].y = -R;
	cycrle_size[1].x = cycrle_size[1].y = +R;

	const int r5 = 11;
	Mover roll_sqr_mv(screen_range, 0x05000, 0x20000, DW*2 - 5 - r5, 10);
	point roll_sqr_pd[6];  // points decart
	point roll_sqr_size[2] = { {-r5, -r5}, {+r5, +r5} };
	polar_point roll_sqr_pp[] =   // triangle vertexes (points polar)
	{
		{r5,  0xffffffff / 5 * 0},
		{r5,  0xffffffff / 5 * 1},
		{r5,  0xffffffff / 5 * 2},
		{r5,  0xffffffff / 5 * 3},
		{r5,  0xffffffff / 5 * 4},
		{0                     },
	};
	PolarToDecart roll_sqr_p2d(roll_sqr_pp, 6);

	int frame_cnt_1s = 0;
	int frame_cnt_tot = 0;
	int seconds_cnt = 0;
	int fps_min, fps_max;
	char fps_str[4] = {0};   // строка с текущим FPS
	char frm_str[10] = {0};   // строка со счётчиком фреймов
	char afps_str[10] = {0};  // строка со спедним FPS
	char fps_extr_str[16] = {"### < к/с < ###"};    // min/max FPS

  while(1)
  {

		// move & transform graphical objects


		LongTriangle.IncRotate(0x00300000);
		LongTriangle.GetDecart(dp3, 9, DH / 2);

		cyrcle_mover.Move(cycrle_size);

		roll_sqr_mv.Move(roll_sqr_size);     // move
		roll_sqr_p2d.IncRotate(-0x04000000);  // rotate
		roll_sqr_p2d.GetDecart(roll_sqr_pd, roll_sqr_mv.GetX(), roll_sqr_mv.GetY());  // get vertex position

		const int txt_x_fps = 98;
		const int txt_x_label = 10;
		const int txt_x_values = 44;

		if(pass_1sec)
		{

			pass_1sec = false;
			seconds_cnt++;

			// выводим текущий, min/max FPS
			if(fps_str[0])
			{
				// гасим предыдущее значение, если есть
				disp[3]->set_x1y8(txt_x_fps, 0);
				disp[3]->text_16x10_puts(fps_str);

				disp[3]->set_x1y8(txt_x_label, 2);
				disp[3]->text_8x6_puts(fps_extr_str);

				fps_min = min(frame_cnt_1s, fps_min);
				fps_max = max(frame_cnt_1s, fps_max);
			}
			else
			{
				fps_min = fps_max = frame_cnt_1s;
			}

			int2strdot(fps_str, frame_cnt_1s, 3, -1, 'o');
			int2strdot(fps_extr_str + 0,  fps_min, 3, -1, 'o');
			int2strdot(fps_extr_str + 12, fps_max, 3, -1, 'o');



			disp[3]->set_x1y8(txt_x_fps, 0);
			disp[3]->text_16x10_puts(fps_str);
			disp[3]->set_x1y8(txt_x_label, 2);
			disp[3]->text_8x6_puts(fps_extr_str);


			// выводим средний FPS
			if(afps_str[0])
			{
				// уже выводили - стираем
				disp[3]->set_x1y8(txt_x_values, 1);
				disp[3]->text_8x6_puts(afps_str);
			}
			else
			{
				// первый раз - выводим заголовок
				disp[3]->set_x1y8(txt_x_label, 1);
				disp[3]->text_8x6_puts((char*)"К/с :");
			}

			int2strdot(afps_str, frame_cnt_tot * 1000 / seconds_cnt, 8, 3, 'o');
			disp[3]->set_x1y8(txt_x_values, 1);
			disp[3]->text_8x6_puts(afps_str);

			frame_cnt_1s = 0;
		}


		// выодим счётчик кадров
		if(frm_str[0])
		{
			// уже выводили - стираем
			disp[3]->set_x1y8(txt_x_values, 0);
			disp[3]->text_8x6_puts(frm_str);
		}
		else
		{
			// первый раз - выводим заголовок
			disp[3]->set_x1y8(txt_x_label, 0);
			disp[3]->text_8x6_puts((char*)"Кадр:");
		}

		int2strdot(frm_str, frame_cnt_tot, 8, -1, 'o');
		disp[3]->set_x1y8(txt_x_values, 0);
		disp[3]->text_8x6_puts(frm_str);


		// мигаем квадратиком
		if(flash_en)
		{
			flash_en = false;
			disp[3]->rect(1, 2, 2, 3, 1, 1);
		}


		// draw objects
		for(unsigned int ii = 0; ii < NUMOFARRAY(disp); ii++)
		{
			//disp[ii]->rect_fill(0, 0, DW-1, DH-1, 0);
			point tp[8]; // temporary points

			disp[ii]->cyrcle(cyrcle_mover.GetX() - ii * DW, cyrcle_mover.GetY(), R, 1, 1);

			add_offset(tp, dp3, 3, -ii * DW, 0);
			disp[ii]->triangle_fill(tp, 1);


			add_offset(tp, roll_sqr_pd, 6, -ii * DW, 0);
			for(int pp = 0; pp < 5; pp++) disp[ii]->triangle_fill(tp[5], tp[pp], tp[(pp+1) % 5], 1);


			disp[ii]->flush(0);

			disp[ii]->cyrcle(cyrcle_mover.GetX() - ii * DW, cyrcle_mover.GetY(), R, 1, 1);

			add_offset(tp, dp3, 3, -ii * DW, 0);
			disp[ii]->triangle_fill(tp, 1);

			add_offset(tp, roll_sqr_pd, 6, -ii * DW, 0);
			for(int pp = 0; pp < 5; pp++) disp[ii]->triangle_fill(tp[5], tp[pp], tp[(pp+1) % 5], 1);
		}
		frame_cnt_tot++;
		frame_cnt_1s++;
  }
}
//--------------------------------------------------------------------------------------------------
