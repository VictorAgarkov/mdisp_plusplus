
/***********************************************\
***                                           ***
***  Набор функций для работы с               ***
***  монохромным графическим дисплеем.        ***
***                                           ***
***  Поддерживаемые типы:                     ***
***                                           ***
***   0 - 96x68 LCD on STE2007                ***
***   1 - 128x64 OLED on SSD1306 4-wire SPI   ***
***                                           ***
***                                           ***
***          (c) Виктор Агарков               ***
***                                           ***
\***********************************************/

#include  "mdisp.h"
#include <stdlib.h>
#include <string.h>

#ifndef min
	#define min(a,b) ((a) < (b)) ? (a) : (b)
#endif
#ifndef max
	#define max(a,b) ((a) > (b)) ? (a) : (b)
#endif

#define BYTES_FOR_BITS(a) (((a) + 7) / 8)  // сколько целых байт нужно для для количества бит


/***************************
***                      ***
***         Base         ***
***                      ***
***       function       ***
***                      ***
***************************/


//---------------------------------------------------------------------------------------------------------------------------------
//  void  MDISP_init(void)
//  Функция инициализации дисплея.
//  Для STM32 должно быть разрешено тактирование GPIO, к которому подключен дисплей
//  in:  -
//  out: -
Mdisp::Mdisp(int16_t width, int16_t height, MdispDevice &disp, const uint8_t* init_seq, int init_seq_size)
:
	#ifdef MDISP_BUFF_EN
		m_FlushImmediate(false), // deferred flush default
	#endif
	m_OverwriteMode(MDISP_FILL_OVERWRITE),
	m_disp(&disp),
	m_x(0),
	m_y8(0),
	width1(width),
	height1(height),
	height8(BYTES_FOR_BITS(height)),
	m_buff_size(width1 * height8)
	#ifdef MDISP_BUFF_EN
		, m_dirty_buff_size(BYTES_FOR_BITS(m_buff_size))
	#endif

{
	#ifdef MDISP_BUFF_EN
		m_buff = new uint8_t[m_buff_size];
		#ifdef MDISP_DEFERRED_FLUSH
			m_dirty_buff = new uint8_t[m_dirty_buff_size];
		#endif
	#endif
	m_disp->SendBuffer(init_seq, (int)init_seq_size, MDISP_CMD);
	clear(); // clear LCD
}
//---------------------------------------------------------------------------------------------------------------------------------
//Mdisp::Mdisp(uint8_t width, uint8_t height, const struct proc_param * Pp)
//:
//	#ifdef MDISP_BUFF_EN
//		m_FlushImmediate(false), // deferred flush default
//	#endif
//	m_OverwriteMode(MDISP_FILL_OVERWRITE),
//	m_Pp(Pp),
//	width1(width),
//	height1(height),
//	height8(BYTES_FOR_BITS(height)),
//	m_buff_size(width1 * height8)
//	#ifdef MDISP_BUFF_EN
//		, m_dirty_buff_size(BYTES_FOR_BITS(m_buff_size))
//	#endif
//{
//	#ifdef MDISP_BUFF_EN
//		m_buff = new uint8_t[m_buff_size];
//		#ifdef MDISP_DEFERRED_FLUSH
//			m_dirty_buff = new uint8_t[m_dirty_buff_size];
//		#endif
//	#endif
//}
//---------------------------------------------------------------------------------------------------------------------------------
Mdisp::~Mdisp()
{
	//if(m_buff) free(m_buff);
	if(m_buff) delete[] m_buff;
	m_buff = 0;
	#ifdef MDISP_DEFERRED_FLUSH
//		if(m_dirty_buff) free(m_dirty_buff);
		if(m_dirty_buff) delete[] m_dirty_buff;
		m_dirty_buff = 0;
	#endif
}

//---------------------------------------------------------------------------------------------------------------------------------
//
// send_data_m(uint8_t data, uint8_t mask)
//
// Модифицирует байт в буфере и/или отправляет его дисплею. Способ модификации задаётся глобальной переменной MDISP_OverwriteMode.
// Адрес в буфере определяется значением текущих координат MDISP_x и _y8.
// Поведение зависит от дефайнов MDISP_BUFF_EN и MDISP_DEFERRED_FLUSH:
//     - не установлены  - данные отправляются непосредственно в дисплей
//     - MDISP_BUFF_EN - в соответстсвии с данными модифицируются значения в буфере и изменённое значение отправляется в дисплей
//     - MDISP_BUFF_EN и MDISP_DEFERRED_FLUSH - в соответстсвии с данными модифицируются значения в буфере, для отправки их в дисплей
//         нужно вызвать функцию flush().
//
// in:
//     data - байт данных
//     mask - битовая маска
//
// out:
//       0 - успех
//
char  Mdisp::send_data_m(uint8_t data, uint8_t mask)
{
	// send / modify data with mask

	#ifdef MDISP_BUFF_EN

		uint8_t ddd;
		uint8_t dd;

		unsigned short buff_offs = m_x + m_y8 * width1;

		if(buff_offs >= m_buff_size) return 1;

		if((m_OverwriteMode & MDISP_FILL_NEG) != 0)
		{
			data ^= 0xff;
		}

		dd = m_buff[buff_offs];

		switch(m_OverwriteMode & 0x07)
		{
			case MDISP_FILL_OVERWRITE:
			{
				dd = data;
				break;
			}
			case MDISP_FILL_OR:
			{
				dd |= data;
				break;
			}
			case MDISP_FILL_AND:
			{
				dd &= data;
				break;
			}
			case MDISP_FILL_XOR:
			{
				dd ^= data;
				break;
			}
		}

		ddd = (dd & mask) | (m_buff[buff_offs] & ~mask);


		#ifdef MDISP_DEFERRED_FLUSH  // deferred update

			if(ddd != m_buff[buff_offs] || m_FlushImmediate)
			{
				m_buff[buff_offs] = ddd;
				if(m_FlushImmediate)
				{
					m_disp->SendBuffer(&ddd, 1, MDISP_DATA);
					m_dirty_buff[buff_offs >> 3] &= ~(1 << (buff_offs & 7));
				}
				else
				{
					m_dirty_buff[buff_offs >> 3] |= 1 << (buff_offs & 7);
				}
			}

		#else  // update immediate

			m_buff[buff_offs] = ddd;
			send_buff_fp(&ddd, 1, MDISP_DATA);

		#endif // MDISP_DEFERRED_FLUSH


	#else  // not defined MDISP_BUFF_EN

		// send data direct to device without buffering
		uint8_t da_ta = data & mask;
		send_buff_fp(&da_ta, 1, MDISP_DATA);

	#endif  // MDISP_BUFF_EN


	// increase coordinates

	m_x++;

	if (m_x >= width1)
	{
		m_x = 0;
		m_y8++;
		if (m_y8 >= height8)
		{
			m_y8 = 0;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------
//
//  char  MDISP_send_data(uint8_t data)
//
//  Отправляет дисплею/в буфер байт данных, используя все биты.
//  in:
//      data - байт данных
//  out: -
//
char  Mdisp::send_data(uint8_t data)
{
	return send_data_m(data, 0xff);
}

//---------------------------------------------------------------------------------------------------------------------------------
//
#ifdef MDISP_DEFERRED_FLUSH



	//---------------------------------------------------------------------------------------------------------------------------------
	void Mdisp::flush_segment(int16_t x0, int16_t x1, int16_t y8)
	{
		send_x1y8(x0, y8);
		m_disp->SendBuffer(m_buff + x0 + y8 * width1, x1 - x0 + 1, MDISP_DATA);
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  flush(bool force)
	//
	//  Отправляет (сливает) в дисплей содержимое буфера.
	//
	//  in:
	//      force == true   - в дисплей отправляется всё содержимое буфера, байт за байтом.
	//      force == false  - отправляются только те байты, которые изменились с момента последнего слива.
	//
	//  out: -
	//
	void  Mdisp::flush(bool force)
	{
		int16_t y;
		//int seg_num = 0;
		if(force)
		{
			// сливаем весь буфер принудительно
			//  page address mode - need set each row (y8) addres
			// птотому что horizontal address mode у SSD1306 с интерфейсом I2C почему-то
			// не происходт инкремент после первой строки (первая строка переписывается 2 раза)
			for(y = 0; y < height8; y++)
			{
				send_x1y8(0, y);
				m_disp->SendBuffer(m_buff + y * width1, width1, MDISP_DATA);
			}
		}
		else
		{
			// сливаем только те фрагменты, которые изменились
			for(y = 0; y < height8; y++)
			{
				//int offs_p = -1;
				int x0 = -1, x1 = -1;
				uint8_t drt;
				for(int x = 0; x < width1; )
				{
					unsigned short offs = x + y * width1;

					if((x & 7) == 0)
					{
						drt = m_dirty_buff[offs >> 3];
					}

					if(drt == 0)
					{
						// в текущих 8 байтах всё чисто - переходим к следующей восьмёрке
						x = (x + 8) & ~7;
						continue;
					}

					bool dirty = (drt & 1);

					if(dirty)
					{
						if(x0 < 0) x0 = x;
						x1 = x;
					}
					else
					{
						if(x1 >= 0)
						{
							if(x > (x1 + 6))
							{
								flush_segment(x0, x1, y);
								//seg_num++;
								x0 = x1 = -1;
							}
						}
					}
					x++;
					drt >>= 1;
				}
				if(x1 >= 0)
				{
					flush_segment(x0, x1, y);
					//seg_num++;
				}
			}
		}
		memset(m_dirty_buff, 0, m_dirty_buff_size);
	}

#else  // do not use DEFERRED FLUSH

	void  Mdisp::flush(bool force)
	{
	}

#endif

//---------------------------------------------------------------------------------------------------------------------------------
//  void MDISP_send_x1y8(uint16_t x1, uint16_t y8)
//  (раньше называлась MDISP_send_xy)
//  Отправляет дисплею координаты для работы.
//  in:
//     x, y - новые координаты
//  out: -
//

void Mdisp::send_x1y8(int16_t x1, int16_t y8)
{
	#if (MDISP_TYPE == 0) || (MDISP_TYPE == 1)// 0 - 96x68 LCD on STE2007,  1 - 128x64 OLED on SSD1306
		uint8_t d[3];
		d[0] = (0x00 | (x1 & 0x0F));         // set X: 0000 x[3:0]
		d[1] = (0x10 | ((x1 >> 4) & 0x07));  // set X: 0010 0 x[6:4]
		d[2] = (0xB0 | (y8 & 0x07));         // set Y: 1011 0yyy
		m_disp->SendBuffer(d, 3, MDISP_CMD);
	#endif
}

//---------------------------------------------------------------------------------------------------------------------------------
//  char set_x1y8(uint16_t x, uint16_t y)
//  (раньше называлась MDISP_set_xy)
//  Устанавливает текущие программные координаты,
//  и если не установлен отложенный слив (MDISP_DEFERRED_FLUSH) отправляет их дисплею.
//  in:
//      x, y - новые координаты:
//         x - смещение от левого края, пикселов
//         y - смещение от верхнего края, по 8 пикселов
//
//  out:
//       0 - успех
//       1 - координаты выходят за допустимые пределы
//
char Mdisp::set_x1y8(int16_t x1, int16_t y8)
{
	// x - in PIXEL
	// y - in vertical 8-pixel height LINES (pixel / 8)

	if(x1 < 0 || x1 >= width1 || y8 < 0 || y8 >= height8) return 1;

	m_x = x1;
	m_y8 = y8;

	#ifndef MDISP_DEFERRED_FLUSH
		MDISP_send_x1y8(x1, y8);
	#endif

	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------

/***************************
***                      ***
***         TEXT         ***
***                      ***
***       function       ***
***                      ***
***************************/

#ifdef MDISP_FONT8x6_EN

	#if (MDISP_CPU == 0) || (MDISP_CPU == 2)  // XMEGA
		extern const uint8_t fnt8x6[][5] PROGMEM;
	#endif

	#if MDISP_CPU == 1  // STM32
		extern const uint8_t fnt8x6[][5];
	#endif
	//---------------------------------------------------------------------------------------------------------------------------------

	//  void MDISP_scroll()
	//  Сдвигает изображение вверх на высоту одной строки (8 точек)
	//  in:  -
	//  out: -
	#ifdef MDISP_BUFF_EN
		void Mdisp::text_8x6_scroll(int16_t L8)
		{
			int i;
			#ifdef MDISP_DEFERRED_FLUSH  // deferred update
				for(i = 0; i < height8 * width1; i++)
				{
					uint8_t dd = m_buff[i];
					uint8_t ddd;
					uint8_t bit_mask = 1 << (i & 7);

					ddd = (i < (height8 - L8) * width1) ? m_buff[i + L8 * width1] : 0;

					m_buff[i] = ddd;

					if(dd != ddd)
					{
						// set "dirty" flag
						m_dirty_buff[i >> 3] |= bit_mask ;
					}
					else
					{
						// copy "dirty" flag with OR
						if(i < (height8 - L8) * width1)
						{
							//MDISP_dirty_buff[i >> 3] = (MDISP_dirty_buff[i >> 3] & ~bit_mask) | (MDISP_dirty_buff[(i + L8 * width1) >> 3] & bit_mask);
							m_dirty_buff[i >> 3] |= (m_dirty_buff[(i + L8 * width1) >> 3] & bit_mask);
						}
					}

				}

			#else  // update immediate
				set_x1y8(0,0);
				for(i = 0; i < height8 * width1; i++)
				{
					uint8_t ddd;

					ddd = (i < (height8 - L8) * width1) ? m_buff[i + L8 * width1] : 0;
					send_data_m(ddd, 0xff);
				}
			#endif // MDISP_DEFERRED_FLUSH

			set_x1y8(0, height8 - L8);
		}
	#else
		void scroll(uint8_t L8)
		{
			set_x1y8(0, 0);
		}
	#endif
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void MDISP_new_line()
	//  переводит строку, и при необходимости сдвигает экран вверх
	//
	//  in:  -
	//  out: -
	//
	void Mdisp::text_8x6_new_line(void)
	{
		if(m_y8 >= (height8 - 1))
		{
			text_8x6_scroll(1);
		}
		else
		{
			set_x1y8(0, m_y8 + 1);
		}

	}

	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_putc(uint8_t c)
	//  выводит на экран и/или в буфер 1 символ
	//  in:
	//      c - выводимый символ. Доступны управляющие символы \n, \r, \b.
	//
	//  out: -
	//

	extern const unsigned char fnt8x6[][5];
	void  Mdisp::text_8x6_putc(uint8_t c)
	{
		//uint8_t i;
		if (c > 127) c -= 64;

		if(c == '\n')  // New line character
		{
			text_8x6_new_line();
			return;
		}

		if(c == '\r') // Return character
		{
			set_x1y8(0, m_y8);
			return;
		}

		if(c == '\b') // Backspace character
		{
			set_x1y8((m_x >= 6) ? m_x - 6 : 0, m_y8);
			return;
		}

		if(m_x >= (width1 - 6))  // outline - go to new line
		{
			text_8x6_new_line();
		}

		c -= 32;

		for(int i = 0; i < 5; i++ )
		{
			#if (MDISP_CPU == 0) || (MDISP_CPU == 2) // XMEGA
				send_data_m(pgm_read_byte( &fnt8x6[c][i]), 0xff);
			#endif

			#if MDISP_CPU == 1  // STM32
				send_data_m(fnt8x6[c][i], 0xff);
			#endif
		}

		send_data_m(0x00, 0xff); // 6-th collumn (space)
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_puts(char *s)
	//  выводит на экран строку символов, заканчивающуюся 0.
	//
	//  in:
	//      s - строка (указатель на 1й байт)
	//
	//  out: -
	//
	void  Mdisp::text_8x6_puts(char *s)
	{
		while(*s)
		{
			text_8x6_putc(*(s++));
		}
	}
	//---------------------------------------------------------------------------------------------------------------------------------

	#ifdef MDISP_FONT8x6_VERTICAL_EN
		//  void  MDISP_putc_v(uint8_t c, uint8_t x, uint8_t y)
		//  Выводит на экран символ, повёрнутый влево на 90 градусов
		//
		//  in:
		//      c - символ
		//      x, y - координаты верхнего левого угла, в пикселах (!), не имеет ничего общего с _y8 и MDISP_x.
		//
		//  out: -
		//
		void  Mdisp::text_8x6_putc_v(uint8_t c, int16_t x, int16_t y)
		{
			// x, y  -in PIXEL
			//
			// vertical (down to up) text out
			// very slow
			//
			//  x,y
			//  ()  ##########
			//    ##    ##
			//  ##      ##
			//    ##    ##
			//      ##########
			//




			if (c > 127) c -= 64;
			c -= 32;

			for(int i = 0; i < 6; i++ )
			{

				#if (MDISP_CPU == 0) || (MDISP_CPU == 2) // XMEGA
					uint8_t bb = (i != 0) ? pgm_read_byte( &fnt8x6[c][5 - i]) : 0;
				#endif

				#if MDISP_CPU == 1  // STM32
					uint8_t bb = (i != 0) ? fnt8x6[c][5 - i] : 0;
				#endif

				int16_t line = y >> 3;
				uint8_t mask = 1 << (y & 7);
				set_x1y8(x, line);

				for(int b = 0; b < 8; b++)
				{
					send_data_m(((bb & 1) == 0) ? 0 : 0xff, mask);
					bb >>= 1;
				}

				y++;
			}

		}

		//---------------------------------------------------------------------------------------------------------------------------------
		//  void  MDISP_puts_v(char *s, uint16_t x, uint16_t y)
		//  Выводит на экран строку символов, повёрнутую влево на 90 градусов.
		//  Строка заканчивается 0.
		//
		//  in:
		//       s - строка (указатель на 1й байт)
		//      x, y - координаты верхнего левого угла, в пикселах (!), не имеет ничего общего с _y8 и MDISP_x.
		//
		//  out: -
		//

		void  Mdisp::text_8x6_puts_v(char *s, int16_t x, int16_t y)
		{
			while(*s)
			{
				text_8x6_putc_v(*(s++), x, y);
				y -= 6;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------
	#endif
	//---------------------------------------------------------------------------------------------------------------------------------

#endif


#ifdef MDISP_FONT16x10_EN
//--------------------------------------------------------------------------------------------------------------------------------

	extern const uint8_t fnt16x10[][20];

	void Mdisp::text_16x10_putc(uint8_t c)
	{
		uint8_t i, line;
		int16_t px, py8;

		if(c == '\n')  // New line character
		{
			text_16x10_new_line();
			return;
		}

		if(c == '\r') // Return character
		{
			set_x1y8(0, m_y8);
			return;
		}

		if(c == '\b') // Backspace character
		{
			set_x1y8((m_x >= 10) ? m_x - 10 : 0, m_y8);
			return;
		}

		if (c > 127) c -= 64;
		c -= 32;

		if(m_x > (width1 - 10)) text_16x10_new_line();

		px  = m_x;
		py8 = m_y8;

		for(line = 0; line < 2; line++)
		{
			set_x1y8(px, py8 + line);
			for(i = 0; i < 10; i++ )
			{
				#if (MDISP_CPU == 0) || (MDISP_CPU == 2) // XMEGA
					send_data_m(pgm_read_byte( &fnt16x10[c][i + line * 10]), 0xff);
				#endif

				#if MDISP_CPU == 1  // STM32
					send_data_m(fnt16x10[c][i + line * 10], 0xff);
				#endif
			}
		}
		set_x1y8(px + 10, py8);
	}
//--------------------------------------------------------------------------------------------------------------------------------
void Mdisp::text_16x10_puts(char* s)
{
	while(*s)
	{
		text_16x10_putc(*s);
		s++;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
//  void MDISP_new_line()
//  переводит строку, и при необходимости сдвигает экран вверх
//
//  in:  -
//  out: -
//
void Mdisp::text_16x10_new_line(void)
{
	if(m_y8 >= (height8 - 2))
	{
		text_8x6_scroll(2);
	}
	else
	{
		set_x1y8(0, m_y8 + 2);
	}
}


//--------------------------------------------------------------------------------------------------------------------------------
//void  MDISP_16x10_scroll(void)
//{
//
//}
//--------------------------------------------------------------------------------------------------------------------------------
#endif // MDISP_FONT16x10_EN

//---------------------------------------------------------------------------------------------------------------------------------
#ifdef MDISP_FONT32x24_EN

	extern uint8_t fnt_32x24_widths[];
	extern uint8_t *fnt_32x24_Pchars[];
	extern const uint8_t fnt_32x24_qtt;

	//  void MDISP_32x24_out(uint8_t c, uint16_t x, uint16_t y, bool flush)
	//
	//  Выводит на экран символ размером 32x24 точки.
	//
	//  in:
	//      c - символ
	//      x, y - координаты верхнего левого угла, в пикселах (!), не имеет ничего общего с m_y8 и MDISP_x.
	//      flush_forrce - сливать буфер после вывода.
	//
	//  out: -
	//
	void Mdisp::text_32x24_out(uint8_t c, int16_t x, int16_t y, bool flush_forrce)
	{


		int16_t width;
		int16_t stuff1;
		int16_t stuff2;

		uint8_t *p;
		int16_t line;

		if(c >= fnt_32x24_qtt) return;

		width = fnt_32x24_widths[c];
		stuff1 = (24 - width) >> 1;
		stuff2 = 24 - width - stuff1;

		p = fnt_32x24_Pchars[c];

		for(line = 0; line < 4; line++)
		{
			int16_t i;
			set_x1y8(x, y + line);

			for(i = 0; i < stuff1; i++) send_data_m(0x00,               0xff);

			#if (MDISP_CPU == 0) || (MDISP_CPU == 0)  // XMEGA
				for(i = 0; i < width; i++)  send_data_m(pgm_read_byte(p++), 0xff);
			#endif

			#if MDISP_CPU == 1  // STM32
				for(i = 0; i < width; i++)  send_data_m(*(p++), 0xff);
			#endif

			for(i = 0; i < stuff2; i++) send_data_m(0x00,               0xff);
		}

		if(flush_forrce)
		{
			flush(false);
		}
	}
#endif

//---------------------------------------------------------------------------------------------------------------------------------
//  void MDISP_clear(void)
//
//  очищает буфер и иэкран
//
//  in:  -
//  out: -
//
void Mdisp::clear(void)
{

	#ifdef MDISP_BUFF_EN
		memset(m_buff, 0, m_buff_size);

		#ifdef MDISP_DEFERRED_FLUSH
			flush(true);
		#else  // no deferred flush
			unsigned short i;
			set_x1y8(0, 0);
			for(i = 0; i < m_buff_size; i++)
			{
				MDISP_send_data(0);
			}
		#endif
	#else
		unsigned short i;
		set_x1y8(0, 0);
		for(i = 0; i < m_buff_size; i++)
		{
			MDISP_send_data(0);
		}
	#endif

}
//---------------------------------------------------------------------------------------------------------------------------------

/*****************************
***                        ***
***       Graphics         ***
***                        ***
***       function         ***
***                        ***
*****************************/

#ifdef MDISP_GRAPHIC_EN

	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t val)
	//
	//  Рисует на экране линию в произвольном направлении.
	//  Работает медленно, поэтому для рисования вертикальных или горизонтальных линий лучше использовать MDISP_fill_rect.
	//  При работе используется значение MDISP_OverwriteMode
	//
	//  in:
	//      x1, y1 - координаты начала линии
	//      x2, y2 - координаты конца линии
	//      val - "цвет".
	//
	//  out: -
	//
	void  Mdisp::line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val)
	{
		int16_t dy, dx;
		int16_t addx, addy;
		signed int 	P, diff;

		addx = 1;
		addy = 1;

		dx = abs((x2 - x1));
		dy = abs((y2 - y1));

		if(x1 > x2)	addx = -1;
		if(y1 > y2)	addy = -1;

		if(dx >= dy)
		{
			dy *= 2;
			P = dy - dx;

			diff = P - dx;

			for(; x1 != (x2 + addx); )
			{
				pixel(x1, y1, val);

				x1 += addx;
				if(P < 0)
				{
					P  += dy;
				}
				else
				{
					P  += diff;
					y1 += addy;
				}
			}
		}
		else
		{
			dx *= 2;
			P = dx - dy;
			diff = P - dy;

			for(; y1 != (y2 + addy); )
			{
				pixel(x1, y1, val);

				y1 += addy;
				if(P < 0)
				{
					P  += dx;
				}
				else
				{
					P  += diff;
					x1 += addx;
				}
			}
		}
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//
	//  void  pixel(uint8_t x, uint8_t y, uint8_t val)
	//
	//  рисует точку на экране заданного "цвета".
	//  При работе используется значение MDISP_OverwriteMode
	//
	//  in:
	//      x, y - координаты точки
	//      val - "цвет".
	//
	//  out: -
	//
	void  Mdisp::pixel(int16_t x, int16_t y, uint8_t val)
	{


		uint8_t mask = 1 << (y & 7);
		if(0 == set_x1y8(x, y >> 3))
		{
			send_data_m(val == 0 ? 0 : 0xff, mask);
		}

	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t val, bool fill)
	//
	//  Рисует прямоугольник.
	//
	//  in:
	//      x1, y1, x2, y2 - координаты диагональных углов прямоугольника
	//      val  - "цвет".
	//      fill:
	//            false - рисовать пустой прямоугольник
	//            true  - рисовать залитый прямоугольник
	//
	//  out: -
	//
	void  Mdisp::rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val, bool fill)
	{
		if(fill) rect_fill(x1, y1, x2, y2, val);
		else     rect_holl(x1, y1, x2, y2, val);
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  rect_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t val)
	//
	//  Рисует залитый  прямоугольник. Предпочтительно для рисования вертикальных или горизонтальных линий.
	//
	//  in:
	//      x1, y1, x2, y2 - координаты диагональных углов прямоугольника
	//      val  - "цвет".
	//  out: -
	//
	void  Mdisp::rect_fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val)
	{
		// filled rectangle


		int16_t x, y;
		if(y1 > y2)
		{
			y = y1;
			y1 = y2;
			y2 = y;
		}

		if(x1 > x2)
		{
			x = x1;
			x1 = x2;
			x2 = x;
		}


		do
		{
			uint8_t mask;
			y = min((y1 | 7), y2);
			mask = ~((1 << (y1 & 7)) - 1);
			mask &= ((1 << ((y & 7) + 1)) - 1);

			if(!set_x1y8(x1, y1 >> 3))
			{
				uint8_t byte_val = val == 0 ? 0 : 0xff;
				for(x = x1; x <= x2; x++)
				{
					send_data_m(byte_val, mask);
				}
			}

			y1 = y + 1;
		}
		while(y1 <= y2);
	}

	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_rect_hollow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t val)
	//
	//  Рисует полый  прямоугольник.
	//
	//  in:
	//      x1, y1, x2, y2 - координаты диагональных углов прямоугольника
	//      val  - "цвет".
	//  out: -
	//
	void  Mdisp::rect_holl(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val)
	{
		// draw 4 lines as fiil_rect
		// vertical lines

		rect_fill(x1, y1, x1, y2, val);
		rect_fill(x2, y1, x2, y2, val);
		// horizontal lines
		if(x1 > x2)
		{
			int16_t x;
			x = x1;
			x1 = x2;
			x2 = x;
		}
		x1++;
		x2--;
		rect_fill(x1, y1, x2, y1, val);
		rect_fill(x1, y2, x2, y2, val);

	}
	//---------------------------------------------------------------------------------------------------------------------------------
	//  void  MDISP_cyrcle(uint8_t x, uint8_t y, uint8_t radius, uint8_t val, bool fill)
	//
	//  Рисует круг.
	//
	//  in:
	//      x, y - координаты центра круга
	//      radius - внезапно радиус круга
	//      val  - "цвет".
	//      fill:
	//            false - рисовать пустой круг
	//            true  - рисовать залитый круг
	//  out: -
	//
	void  Mdisp::cyrcle(int16_t x, int16_t y, int16_t radius, uint8_t val, bool fill)
	{
		int16_t a, b, P;

		if
		(
					(radius <= 0) ||
					(x < -radius) ||
					(x > (radius + width1)) ||
					(y < -radius) ||
					(y > (radius + height1))
		) return;

		a = 0;
		b = radius;
		P = 1 - radius;

		do
		{
			if(fill)
			{
				// draw cyrcle with _horisontal_ lines
				if(P >= 0 && a != b)
				{
					rect_fill(x+b, y-a, x+b, y+a, val);
					rect_fill(x-b, y-a, x-b, y+a, val);
				}

				rect_fill(x+a, y-b, x+a, y+b, val);
				if(a)
				{
					rect_fill(x-a, y-b, x-a, y+b, val);
				}
			}
			else
			{
				pixel(x+a, y+b, val);
				pixel(x+a, y-b, val);

				if(a != 0)
				{
					pixel(x-a, y-b, val);
					pixel(x-a, y+b, val);
				}

				if(a != b)
				{
					pixel(x+b, y+a, val);
					pixel(x-b, y+a, val);

					if(a)
					{
						pixel(x+b, y-a, val);
						pixel(x-b, y-a, val);
					}
				}
			}

			if(P < 0)
			{
				P += 3 + 2 * a++;
			}
			else
			{
				P += 5 + 2 * (a++ - b--);
			}
		}
		while(a <= b);
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	#ifdef MDISP_BEZIER_MAXP
		#define MDISP_BEZIER_FACTOR_SCALE 0x40000000UL
		int32_t Mdisp::interpolate_16_16_by_2_30(int32_t v1, int32_t v2, uint32_t factor)
		{
			// v1, v2 - fixed point 16.16
			// factor - fixed point 2.30 (1.31?)
			// return - fixed point 16.16
			long long ll = (long long)v2 * factor;
			ll += (long long)v1 * (MDISP_BEZIER_FACTOR_SCALE - factor);
			return ll >> 30;
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void Mdisp::bezier(struct point *P, short p_num, short t_num, uint8_t val)
		{
			// рисуем кривые Безье методом Поля де Фаже де Кастельжо.
			// P - массив опорных точек
			// p_num - кол-во опорных точек
			// t_num - кол-во отрезков интерполяции
			// val - "цвет" кривой

			long l[MDISP_BEZIER_MAXP * 2];
			unsigned long t, dt;
			int i, j;
			long x0, y0;
			long *lpp;

			if(p_num < 1 || p_num > MDISP_BEZIER_MAXP) return;

			dt = MDISP_BEZIER_FACTOR_SCALE / t_num;   // приращение с фиксированной точкой

			x0 = (long)P[0].x << 16;
			y0 = (long)P[0].y << 16;


			for(t = dt; t <= (MDISP_BEZIER_FACTOR_SCALE); t += dt)
			{
				// первый проход - интерполируем из исходных точек в буфер
				for(i = 0; i < p_num - 1; i++)
				{
					l[i * 2 + 0] = interpolate_16_16_by_2_30((long)P[i].x << 16, (long)P[i + 1].x << 16, t);
					l[i * 2 + 1] = interpolate_16_16_by_2_30((long)P[i].y << 16, (long)P[i + 1].y << 16, t);
				}


				// остальные проходы - интерполируем сам буфер, постепенно сужаясь к началу
				for(i = p_num - 2; i > 0; i--)
				{
					lpp = l;
					for(j = 1; j <= i * 2; j++)
					{
						lpp[0] = interpolate_16_16_by_2_30(lpp[0], lpp[2], t);
						lpp++;
					}
				}
				// рисуем очередной отрезок
				line(x0 >> 16, y0 >> 16, l[0] >> 16, l[1] >> 16, val);
				x0 = l[0];
				y0 = l[1];
			}
		}

	#endif  // MDISP_BEZIER_MAXP




	#ifdef MDISP_GRAPH_TRIANGLE_EN

		void  Mdisp::triangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t val, bool fill)
		{
			if(fill) triangle_fill(x1, y1, x2, y2, x3, y3, val);
			else     triangle_holl(x1, y1, x2, y2, x3, y3, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		int32_t get_incline_16p16(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
		{
			// get increment value for line by 2 points
			// return - fixed point 16.16 value
			int32_t dy = y2 - y1;
			int32_t dx = x2 - x1;
			dy = (dy << 16) / dx;
			return dy;
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t val)
		{
			// filled triangle

			// sort by X
			int16_t t;
			#define swap(a, b)  t = a; a = b; b = t
			if(x1 > x2)
			{
				swap(x1, x2);
				swap(y1, y2);
			}

			if(x2 > x3)
			{
				swap(x2, x3);
				swap(y2, y3);
			}

			if(x1 > x2)
			{
				swap(x1, x2);
				swap(y1, y2);
			}
			#undef swap
			if(x3 == x1)
			{
				// прямая вертикальная линия
				int16_t yh, yl;
				yh = max(y1, max(y2, y3));
				yl = min(y1, min(y2, y3));
				rect_fill(x1, yh, x1, yl, val);
			}
			else
			{
				int32_t dy13;
				int32_t yh, yl;
				int16_t xx;
				dy13 = get_incline_16p16(x1, y1, x3, y3);
				
				yh = (int32_t)y1 << 16;

				if(x1 != x2)
				{
					int32_t dy12 = get_incline_16p16(x1, y1, x2, y2);
					yl = yh;

					for(xx = x1; xx < x2; xx++)
					{
						rect_fill(xx, yh >> 16, xx, yl >> 16, val);
						yh += dy13;
						yl += dy12;
					}
				}

				if(x3 != x2)
				{
					int32_t dy23 = get_incline_16p16(x2, y2, x3, y3);
					yl = (int32_t)y2 << 16;

					for(xx = x2; xx <= x3; xx++)
					{
						rect_fill(xx, yh >> 16, xx, yl >> 16, val);
						yh += dy13;
						yl += dy23;
					}
				}

			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_holl( int16_t x1,  int16_t y1,  int16_t x2,  int16_t y2,  int16_t x3,  int16_t y3, uint8_t val)
		{
			// hollow triangle
			line(x1, y1, x2, y2, val);
			line(x2, y2, x3, y3, val);
			line(x3, y3, x1, y1, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_fill(const struct point P[],  uint8_t val)
		{
			triangle_fill(P[0].x, P[0].y, P[1].x, P[1].y, P[2].x, P[2].y, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_holl(const struct point P[],  uint8_t val)
		{
			triangle_holl(P[0].x, P[0].y, P[1].x, P[1].y, P[2].x, P[2].y, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_fill(const struct point &p1, const  struct point &p2, const  struct point &p3,  uint8_t val)
		{
			triangle_fill(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------
		void  Mdisp::triangle_holl(const struct point &p1, const struct point &p2, const struct point &p3,  uint8_t val)
		{
			triangle_holl(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, val);
		}
		//---------------------------------------------------------------------------------------------------------------------------------

	#endif
	//---------------------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------------

#endif
