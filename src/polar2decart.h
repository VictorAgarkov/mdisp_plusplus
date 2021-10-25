#ifndef POLARTODECART_H
#define POLARTODECART_H

	#include "point_types.h"

	struct polar_point
	{
		uint32_t r;
		uint32_t turn;
	};


	class PolarToDecart
	{
	public:
		PolarToDecart(struct polar_point P[], int point_num);
		~PolarToDecart();
		void      SetPolar(struct polar_point P[]);
		void      SetRotate(uint32_t turn){m_turn = turn;}           // fixed point 0.32 ===  0 .. 359.999 degree
		void      IncRotate(uint32_t turn){m_turn += turn;}           // fixed point 0.32 ===  0 .. 359.999 degree
		uint32_t  GetRotate(void){return m_turn;}         // fixed point 0.32 ===  0 .. 359.999 degree
		void      GetDecart(struct point dst[]);
		void      GetDecart(struct point dst[], struct point &center);
		void      GetDecart(struct point dst[], int16_t cx, int16_t cy);
	private:
		struct    polar_point *m_vertex;
		uint32_t  m_turn;
		int       m_point_num;
	};




#endif //POLARTODECART_H
