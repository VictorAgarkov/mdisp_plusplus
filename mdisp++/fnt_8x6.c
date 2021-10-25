#include "config.h"

//#ifdef MDISP_FONT8x6_EN

	#if (MDISP_CPU == 0) || (MDISP_CPU == 2) // XMEGA
		#include <avr/pgmspace.h>
		#define asPROGMEM PROGMEM
	#endif

	#if MDISP_CPU == 1  // STM32
		#define asPROGMEM
	#endif


	const unsigned char fnt8x6[][5] asPROGMEM =

  {
  	{0x00, 0x00, 0x00, 0x00, 0x00},// (space) 32
  	{0x00, 0x00, 0x5F, 0x00, 0x00},// !  33
  	{0x00, 0x07, 0x00, 0x07, 0x00},// "  34
  	{0x14, 0x7F, 0x14, 0x7F, 0x14},// #  35
  	{0x24, 0x2A, 0x7F, 0x2A, 0x12},// $  36
  	{0x23, 0x13, 0x08, 0x64, 0x62},// %  37
  	{0x36, 0x49, 0x55, 0x22, 0x50},// &  38
  	{0x00, 0x05, 0x03, 0x00, 0x00},// '  39
  	{0x00, 0x1C, 0x22, 0x41, 0x00},// (  40
  	{0x00, 0x41, 0x22, 0x1C, 0x00},// )  41
  	{0x08, 0x2A, 0x1C, 0x2A, 0x08},// *  42
  	{0x08, 0x08, 0x3E, 0x08, 0x08},// +  43
  	{0x00, 0x50, 0x30, 0x00, 0x00},// ,  44
  	{0x08, 0x08, 0x08, 0x08, 0x08},// -  45
  	{0x00, 0x30, 0x30, 0x00, 0x00},// .  46
  	{0x20, 0x10, 0x08, 0x04, 0x02},// /  47
  	{0x3E, 0x51, 0x49, 0x45, 0x3E},// 0  48
  	{0x00, 0x42, 0x7F, 0x40, 0x00},// 1  49
  	{0x42, 0x61, 0x51, 0x49, 0x46},// 2  50
  	{0x21, 0x41, 0x45, 0x4B, 0x31},// 3  51
  	{0x18, 0x14, 0x12, 0x7F, 0x10},// 4  52
  	{0x27, 0x45, 0x45, 0x45, 0x39},// 5  53
  	{0x3C, 0x4A, 0x49, 0x49, 0x30},// 6  54
  	{0x01, 0x71, 0x09, 0x05, 0x03},// 7  55
  	{0x36, 0x49, 0x49, 0x49, 0x36},// 8  56
  	{0x06, 0x49, 0x49, 0x29, 0x1E},// 9  57
  	{0x00, 0x36, 0x36, 0x00, 0x00},// :  58
  	{0x00, 0x56, 0x36, 0x00, 0x00},// ;  59
  	{0x00, 0x08, 0x14, 0x22, 0x41},// <  60
  	{0x14, 0x14, 0x14, 0x14, 0x14},// =  61
  	{0x41, 0x22, 0x14, 0x08, 0x00},// >  62
  	{0x02, 0x01, 0x51, 0x09, 0x06},// ?  63
  	{0x32, 0x49, 0x79, 0x41, 0x3E},// @  64
  	{0x7E, 0x11, 0x11, 0x11, 0x7E},// A  65
  	{0x7F, 0x49, 0x49, 0x49, 0x36},// B  66
  	{0x3E, 0x41, 0x41, 0x41, 0x22},// C  67
  	{0x7F, 0x41, 0x41, 0x22, 0x1C},// D  68
  	{0x7F, 0x49, 0x49, 0x49, 0x41},// E  69
  	{0x7F, 0x09, 0x09, 0x01, 0x01},// F  70
  	{0x3E, 0x41, 0x41, 0x51, 0x32},// G  71
  	{0x7F, 0x08, 0x08, 0x08, 0x7F},// H  72
  	{0x00, 0x41, 0x7F, 0x41, 0x00},// I  73
  	{0x20, 0x40, 0x41, 0x3F, 0x01},// J  74
  	{0x7F, 0x08, 0x14, 0x22, 0x41},// K  75
  	{0x7F, 0x40, 0x40, 0x40, 0x40},// L  76
  	{0x7F, 0x02, 0x04, 0x02, 0x7F},// M  77
  	{0x7F, 0x04, 0x08, 0x10, 0x7F},// N  78
  	{0x3E, 0x41, 0x41, 0x41, 0x3E},// O  79
  	{0x7F, 0x09, 0x09, 0x09, 0x06},// P  80
  	{0x3E, 0x41, 0x51, 0x21, 0x5E},// Q  81
  	{0x7F, 0x09, 0x19, 0x29, 0x46},// R  82
  	{0x46, 0x49, 0x49, 0x49, 0x31},// S  83
  	{0x01, 0x01, 0x7F, 0x01, 0x01},// T  84
  	{0x3F, 0x40, 0x40, 0x40, 0x3F},// U  85
  	{0x1F, 0x20, 0x40, 0x20, 0x1F},// V  86
  	{0x7F, 0x20, 0x18, 0x20, 0x7F},// W  87
  	{0x63, 0x14, 0x08, 0x14, 0x63},// X  88
  	{0x03, 0x04, 0x78, 0x04, 0x03},// Y  89
  	{0x61, 0x51, 0x49, 0x45, 0x43},// Z  90
  	{0x00, 0x00, 0x7F, 0x41, 0x41},// [  91
  	{0x02, 0x04, 0x08, 0x10, 0x20},// \  92
  	{0x41, 0x41, 0x7F, 0x00, 0x00},// ]  93
  	{0x04, 0x02, 0x01, 0x02, 0x04},// ^  94
  	{0x40, 0x40, 0x40, 0x40, 0x40},// _  95
  	{0x00, 0x01, 0x02, 0x04, 0x00},// `  96
  	{0x20, 0x54, 0x54, 0x54, 0x78},// a  97
  	{0x7F, 0x48, 0x44, 0x44, 0x38},// b  98
  	{0x38, 0x44, 0x44, 0x44, 0x20},// c  99
  	{0x38, 0x44, 0x44, 0x48, 0x7F},// d  100
  	{0x38, 0x54, 0x54, 0x54, 0x18},// e  101
  	{0x08, 0x7E, 0x09, 0x01, 0x02},// f  102
  	{0x08, 0x14, 0x54, 0x54, 0x3C},// g  103
  	{0x7F, 0x08, 0x04, 0x04, 0x78},// h  104
  	{0x00, 0x44, 0x7D, 0x40, 0x00},// i  105
  	{0x20, 0x40, 0x44, 0x3D, 0x00},// j  106
  	{0x00, 0x7F, 0x10, 0x28, 0x44},// k  107
  	{0x00, 0x41, 0x7F, 0x40, 0x00},// l  108
  	{0x7C, 0x04, 0x18, 0x04, 0x78},// m  109
  	{0x7C, 0x08, 0x04, 0x04, 0x78},// n  110
  	{0x38, 0x44, 0x44, 0x44, 0x38},// o  111
  	{0x7C, 0x14, 0x14, 0x14, 0x08},// p  112
  	{0x08, 0x14, 0x14, 0x18, 0x7C},// q  113
  	{0x7C, 0x08, 0x04, 0x04, 0x08},// r  114
  	{0x48, 0x54, 0x54, 0x54, 0x20},// s  115
  	{0x04, 0x3F, 0x44, 0x40, 0x20},// t  116
  	{0x3C, 0x40, 0x40, 0x20, 0x7C},// u  117
  	{0x1C, 0x20, 0x40, 0x20, 0x1C},// v  118
  	{0x3C, 0x40, 0x30, 0x40, 0x3C},// w  119
  	{0x44, 0x28, 0x10, 0x28, 0x44},// x  120
  	{0x0C, 0x50, 0x50, 0x50, 0x3C},// y  121
  	{0x44, 0x64, 0x54, 0x4C, 0x44},// z  122
  	{0x00, 0x08, 0x36, 0x41, 0x00},// {  123
  	{0x00, 0x00, 0x7F, 0x00, 0x00},// |  124
  	{0x00, 0x41, 0x36, 0x08, 0x00},// }  125
  	{0x02, 0x01, 0x02, 0x04, 0x02},// ~  126
  	{0x08, 0x1C, 0x2A, 0x08, 0x08},// <- 127

    //#ifdef MDISP_FONT8x6_FULL_CHARSET
      // Cirilyc - CP1251
    	{ 0x7E, 0x11, 0x11, 0x11, 0x7E },  // �  192
    	{ 0x7F, 0x49, 0x49, 0x49, 0x33 },  // �  193
    	{ 0x7F, 0x49, 0x49, 0x49, 0x36 },  // �  194
    	{ 0x7F, 0x01, 0x01, 0x01, 0x03 },  // �  195
    	{ 0xE0, 0x51, 0x4F, 0x41, 0xFF },  // �  196
    	{ 0x7F, 0x49, 0x49, 0x49, 0x49 },  // �  197
    	{ 0x77, 0x08, 0x7F, 0x08, 0x77 },  // �  198
    	{ 0x49, 0x49, 0x49, 0x49, 0x36 },  // �  199
    	{ 0x7F, 0x10, 0x08, 0x04, 0x7F },  // �  200
    	{ 0x7C, 0x21, 0x12, 0x09, 0x7C },  // �  201
    	{ 0x7F, 0x08, 0x14, 0x22, 0x41 },  // �  202
    	{ 0x20, 0x41, 0x3F, 0x01, 0x7F },  // �  203
    	{ 0x7F, 0x02, 0x0C, 0x02, 0x7F },  // �  204
    	{ 0x7F, 0x08, 0x08, 0x08, 0x7F },  // �  205
    	{ 0x3E, 0x41, 0x41, 0x41, 0x3E },  // �  206
    	{ 0x7F, 0x01, 0x01, 0x01, 0x7F },  // �  207
    	{ 0x7F, 0x09, 0x09, 0x09, 0x06 },  // �  208
    	{ 0x3E, 0x41, 0x41, 0x41, 0x22 },  // �  209
    	{ 0x01, 0x01, 0x7F, 0x01, 0x01 },  // �  210
    	{ 0x27, 0x48, 0x48, 0x48, 0x3F },  // �  211
    	{ 0x1C, 0x22, 0x7F, 0x22, 0x1C },  // �  212
    	{ 0x63, 0x14, 0x08, 0x14, 0x63 },  // �  213
    	{ 0x7F, 0x40, 0x40, 0x40, 0xFF },  // �  214
    	{ 0x07, 0x08, 0x08, 0x08, 0x7F },  // �  215
    	{ 0x7F, 0x40, 0x7F, 0x40, 0x7F },  // �  216
    	{ 0x7F, 0x40, 0x7F, 0x40, 0xFF },  // �  217
    	{ 0x01, 0x7F, 0x48, 0x48, 0x30 },  // �  218
    	{ 0x7F, 0x48, 0x30, 0x00, 0x7F },  // �  219
    	{ 0x7F, 0x48, 0x48, 0x30, 0x00 },  // �  220
    	{ 0x22, 0x41, 0x49, 0x49, 0x3E },  // �  221
    	{ 0x7F, 0x08, 0x3E, 0x41, 0x3E },  // �  222
    	{ 0x46, 0x29, 0x19, 0x09, 0x7F },  // �  223
    	{ 0x20, 0x54, 0x54, 0x54, 0x78 },  // �  224
    	{ 0x3C, 0x4A, 0x4A, 0x49, 0x31 },  // �  225
    	{ 0x7C, 0x54, 0x54, 0x28, 0x00 },  // �  226
    	{ 0x7C, 0x04, 0x04, 0x04, 0x0C },  // �  227
    	{ 0xE0, 0x54, 0x4C, 0x44, 0xFC },  // �  228
    	{ 0x38, 0x54, 0x54, 0x54, 0x08 },  // �  229
    	{ 0x6C, 0x10, 0x7C, 0x10, 0x6C },  // �  230
    	{ 0x44, 0x44, 0x54, 0x54, 0x28 },  // �  231
    	{ 0x7C, 0x20, 0x10, 0x08, 0x7C },  // �  232
    	{ 0x78, 0x42, 0x24, 0x12, 0x78 },  // �  233
    	{ 0x7C, 0x10, 0x28, 0x44, 0x00 },  // �  234
    	{ 0x20, 0x44, 0x3C, 0x04, 0x7C },  // �  235
    	{ 0x7C, 0x08, 0x10, 0x08, 0x7C },  // �  236
    	{ 0x7C, 0x10, 0x10, 0x10, 0x7C },  // �  237
    	{ 0x38, 0x44, 0x44, 0x44, 0x38 },  // �  238
    	{ 0x7C, 0x04, 0x04, 0x04, 0x7C },  // �  239
    	{ 0x7C, 0x14, 0x14, 0x14, 0x08 },  // �  240
    	{ 0x38, 0x44, 0x44, 0x44, 0x44 },  // �  241
    	{ 0x04, 0x04, 0x7C, 0x04, 0x04 },  // �  242
    	{ 0x0C, 0x50, 0x50, 0x50, 0x3C },  // �  243
    	{ 0x18, 0x24, 0x7E, 0x24, 0x18 },  // �  244
    	{ 0x44, 0x28, 0x10, 0x28, 0x44 },  // �  245
    	{ 0x7C, 0x40, 0x40, 0x40, 0xFC },  // �  246
    	{ 0x0C, 0x10, 0x10, 0x10, 0x7C },  // �  247
    	{ 0x7C, 0x40, 0x7C, 0x40, 0x7C },  // �  248
    	{ 0x7C, 0x40, 0x7C, 0x40, 0xFC },  // �  249
    	{ 0x04, 0x7C, 0x50, 0x50, 0x20 },  // �  250
    	{ 0x7C, 0x50, 0x20, 0x00, 0x7C },  // �  251
    	{ 0x7C, 0x50, 0x50, 0x20, 0x00 },  // �  252
    	{ 0x28, 0x44, 0x54, 0x54, 0x38 },  // �  253
    	{ 0x7C, 0x10, 0x38, 0x44, 0x38 },  // �  254
    	{ 0x08, 0x54, 0x34, 0x14, 0x7C }   // �  255

    //#endif  // MDISP_FONT8x6_FULL_CHARSET
  };

//#endif // MDISP_FONT32x24_EN
