#ifndef MOVER_H_INCLUDED
#define MOVER_H_INCLUDED

	#include "point_types.h"

	class Mover
	{
	public:
		Mover(point range[2], int32_t speed_x, int32_t speed_y, int16_t pos_x, int16_t pos_y);
		void Move();
		void Move(point Size[2]);
		void GetCurrent(point &dst);
		int16_t GetX(void){return m_current[0] >> 16;}
		int16_t GetY(void){return m_current[1] >> 16;}

	private:
		point m_range[2];       // ограничения по x, y
		int32_t m_speed[2];     // скорость по x, y   в формате 16.16
		int32_t m_current[2];   // текущее положение, в формате 16.16
	};


#endif /* MOVER_H_INCLUDED */
