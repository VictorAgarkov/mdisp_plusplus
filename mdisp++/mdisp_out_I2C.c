/***********************************************\
***                                           ***
***     Набор функций для вывода данных       ***
***     на дисплей по интерфейсу I2C          ***
***                                           ***
***                                           ***
***   (c) Виктор Агарков                      ***
***                                           ***
\***********************************************/


#include "config.h"
#include "gpio_F1xx.h"




#if (MDISP_CPU == 0) || (MDISP_CPU == 2)  // XMEGA
	#include <util/delay.h>
	#include <avr/pgmspace.h>
	#include <avr/io.h>
#elif MDISP_CPU == 1  // STM32
	#include <stm32f10x.h>    // Device header
#endif

void mdisp_I2C_sw_send_byte(uint8_t data);
void mdisp_I2C_sw_start_addr(uint8_t addr);
void mdisp_I2C_sw_stop(void);

const struct pin_def  mdisp_i2c_pins[] =
{
	{GPIOB, 15, 0},  // SDA
	{GPIOB, 14, 0},  // SCL
};

#define MDISP_SDA mdisp_i2c_pins[0]
#define MDISP_SCL mdisp_i2c_pins[1]

#define MDISP_SDA_LOW   GPIO_OUT_VAL(MDISP_SDA, 0)
#define MDISP_SDA_HIGH  GPIO_OUT_VAL(MDISP_SDA, 1)
#define MDISP_SCL_LOW   GPIO_OUT_VAL(MDISP_SCL, 0)
#define MDISP_SCL_HIGH  GPIO_OUT_VAL(MDISP_SCL, 1)

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void mdisp_out_init(void)
{
	// init GPIO as open drain out
	for(int i = 0; i < NUMOFARRAY(mdisp_i2c_pins); i++)
	{
		GPIO_SET_PASSIVE(mdisp_i2c_pins[i]);
		GPIO_SET_MODE(mdisp_i2c_pins[i], GPIO_Mode_OUT_OD | GPIO_Speed_2MHz);
	}

}
//---------------------------------------------------------------------------------------------------------------------------------
void mdisp_I2C_sw_out_buff(const uint8_t *data, int num, int data_cmd)
{
	int i;
	uint8_t v;
	mdisp_I2C_sw_start_addr(MDISP_I2C_ADDR);

	v = (data_cmd != 0) ? 0x40 : 0;
	//v |= 0x80; // "Co" bit (?)
	mdisp_I2C_sw_send_byte(v);  // send control byte
	for(i = 0; i < num; i++)
	{
			mdisp_I2C_sw_send_byte(data[i]);  // send data byte
	}
	mdisp_I2C_sw_stop();
}

//---------------------------------------------------------------------------------------------------------------------------------
void mdisp_I2C_sw_send_byte(uint8_t data)
{
	int i;
	volatile int dd;
	uint32_t v = data;
	v <<= 1;
	v |= 1; // ack
	for(i = 0; i < 9; i++)
	{
		GPIO_OUT_VAL(MDISP_SDA, v & 0x0100);
		for(dd = 0; dd < 2; dd++);
		v <<= 1;
		MDISP_SCL_HIGH;      for(dd = 0; dd < 3; dd++);
		MDISP_SCL_LOW;      //for(dd = 0; dd < 3; dd++);
	}

}
//---------------------------------------------------------------------------------------------------------------------------------
void mdisp_I2C_sw_start_addr(uint8_t addr)
{
	// make start condition
	// I2C start
	volatile int dd;
	MDISP_SDA_LOW;     for(dd = 0; dd < 20; dd++);
	MDISP_SCL_LOW;     //for(dd = 0; dd < 3; dd++);

	mdisp_I2C_sw_send_byte(addr << 1);  // send addres + ack bit

}
//---------------------------------------------------------------------------------------------------------------------------------
void mdisp_I2C_sw_stop(void)
{
	// I2C stop
	volatile int dd;
	MDISP_SDA_LOW;      //for(dd = 0; dd < 10; dd++);
	MDISP_SCL_HIGH;     for(dd = 0; dd < 20; dd++);
	MDISP_SDA_HIGH;     //for(dd = 0; dd < 10; dd++);

}
//---------------------------------------------------------------------------------------------------------------------------------




