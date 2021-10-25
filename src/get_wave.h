#ifndef GET_WAVE_H
#define GET_WAVE_H

	#include <stdint.h>

	#ifdef __cplusplus
		extern "C" {
	#endif


	int32_t get_wave32(const int32_t *table256, const int32_t *d_table256, int32_t value32);
	int16_t get_wave16(const int16_t *table256, const int16_t *d_table256, int32_t value32);


	extern const int16_t table_s100_m0[256];
	extern const int16_t table_s100_m0_d[256];

	#ifdef __cplusplus
		}
	#endif


#endif //GET_WAVE_H
