
#include "get_wave.h"

//------------------------------------------------------------------------------------------------------------------
/*
int32_t get_wave32(const int32_t *table256, const int32_t *d_table256, int32_t value32)
{
	uint32_t u = value32;
	int idx = u >> 24;
	long long v = d_table256[idx];
	v *= u & 0x00ffffff;
	v >>= 24;
	return table256[idx] + v;
}
*/
//------------------------------------------------------------------------------------------------------------------
int16_t get_wave16(const int16_t *table256, const int16_t *d_table256, int32_t value32)
{
	uint32_t u = value32;
	int idx = u >> 24;
	long long v = d_table256[idx];

	v *= u & 0x00ffffff;
	v >>= 24;
	return table256[idx] + v;
}

//------------------------------------------------------------------------------------------------------------------
