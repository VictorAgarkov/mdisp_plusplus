#ifndef INIT_SEQ_H_INCLUDED
#define INIT_SEQ_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

	#include <stdint.h>
	#define asPROGMEM   // for ARM
	//#define asPROGMEM   PROGMEM // for Atmel AVR

	extern const uint8_t STE2007_init_sequence[13] asPROGMEM;
	extern const uint8_t SSD1306_init_sequence[28] asPROGMEM;

#ifdef __cplusplus
}
#endif


#endif /* INIT_SEQ_H_INCLUDED */
