#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


	//XMEGA == 0
	//STM32 == 1
	//MEGA  == 2
	// better define it in project settings:
	// Project->Build options... -> tab 'Compiler settings' -> tab '#defines' - add string MDISP_CPU=1
	#define MDISP_CPU 1




	// 0 - 96x68 LCD on STE2007
	// 1 - 128x64 OLED on SSD1306
	#define MDISP_TYPE 1


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// полезные макросы
	#define BIT_ARR_SIZE(s) (((s) + 7) >> 3)
	#define NUMOFARRAY(a)    (sizeof(a) / sizeof(a[0]))
	#define min(a, b) ((a < b) ? a : b)
	#define max(a, b) ((a > b) ? a : b)
	//#define abs(a) ((a) > 0 ? (a) : -(a))
	#define lim(a,b,c) max((a), min((b),(c)))    // a <= b <= c


#endif /* CONFIG_H_INCLUDED */
