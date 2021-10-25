/********************************************
*                                           *
*                                           *
*   Инициализирующие последовательности     *
*   для некоторых монохромных дисплеев      *
*                                           *
*                                           *
********************************************/


#include "init_seq.h"

// 96x68 LCD on STE2007
const uint8_t STE2007_init_sequence[13] asPROGMEM =
{

	0xE2, // *** SOFTWARE RESET
	0x3A, // *** Use internal oscillator
	0xEF, // *** FRAME FREQUENCY:
	0x04, // *** 80Hz
	0xD0, // *** 1:65 divider
	0x20, // Запись в регистр Vop
	0x90,

	0xA4, // all on/normal display
	0x2F, // Power control set(charge pump on/off)
	0x40, // set start row address = 0
	//0xB0, // установить Y-адрес = 0
	//0x10, // установить X-адрес, старшие 3 бита
	//0x0,  // установить X-адрес, младшие 4 бита
	//0xC8, // mirror Y axis about X axis
	//0xA1, // Инвертировать экран по горизонтали
	0xAC, // set initial row R0 of the display
	0x07,
	//0xF9, //
	0xAF, // экран вкл/выкл
};

// 128x64 OLED on SSD1306
const uint8_t SSD1306_init_sequence[28] asPROGMEM =
{
	0xAE | 0,             // Display Off (0x00/0x01)
	0xD5, 0xf0, //0x80,   // Set Clock as 100 Frames/Sec
	0xA8, 0x3f,           // 1/64 Duty (0x0F~0x3F)
	0xD3, 0,              // Shift Mapping RAM Counter (0x00~0x3F)
	0x40 | 0,             // Set Mapping RAM Display Start Line (0x00~0x3F)
	0x8D, 0x10 | 4,       // Enable Embedded DC/DC Converter (0x00/0x04)
	0x20, 0,              // Set Addressing Mode (0x00 - horizontal, 0x01 - vertical, 0x02 - page)
	0xA0 | 1,             // Set SEG/Column Mapping
	0xC0 | 8,             // Set COM/Row Scan Direction
	0xDA, 0x02 | 0x10,    // Set Sequential Configuration (0x00/0x10)
	0x81, 255, //0xCF,    // Set SEG Output Current
	0xD9, 0xF1,           // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	0xDB, 0x40,           // Set VCOM Deselect Level
	0xA4 | 0,             // Disable Entire Display On (0x00/0x01)
	0xA6 | 0,             // Disable Inverse Display On (0x00/0x01)
	0xAE | 1,             // Display On (0x00/0x01)
	0xb0, 0x00, 0x10      // set pos 0,0
};
