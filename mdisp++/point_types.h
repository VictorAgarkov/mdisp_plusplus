#ifndef POINT_TYPES_H_INCLUDED
#define POINT_TYPES_H_INCLUDED

	#include <stdint.h>
	//------------------------------------------------------------------------------
	struct point
	{
		int16_t x, y;
	};


	//------------------------------------------------------------------------------
/*
	union rect
	{
		uint32_t l[2];
		struct point p[2];
		int16_t s[4];
		struct
		{
			int16_t x1, y1;
			int16_t x2, y2;
		};
	};
	*/
	//------------------------------------------------------------------------------


#endif /* POINT_TYPES_H_INCLUDED */
