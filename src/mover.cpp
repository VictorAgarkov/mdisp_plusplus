
#include "mover.h"
#include <string.h>
//------------------------------------------------------------------------------------------------------------------

Mover::Mover(point range[2], int32_t speed_x, int32_t speed_y, int16_t pos_x, int16_t pos_y)

{
	m_speed[0] = speed_x;
	m_speed[1] = speed_y;

	memcpy(m_range, range, sizeof(m_range));
	m_current[0] = (int32_t)pos_x << 16;
	m_current[1] = (int32_t)pos_y << 16;
}
//------------------------------------------------------------------------------------------------------------------
void Mover::Move()
{
	point def_size[2] = {{0,0}, {0,0}};
	Move(def_size);
}
//------------------------------------------------------------------------------------------------------------------
void Mover::Move(point Size[2])
{


	int16_t cc = (m_current[0] += m_speed[0]) >> 16;

	if(cc < (m_range[0].x - Size[0].x))
	{
		if(m_speed[0] < 0) m_speed[0] = -m_speed[0];

		m_current[0] = (m_range[0].x - Size[0].x);
		m_current[0] <<= 16;
	}
	else if(cc > (m_range[1].x - Size[1].x))
	{
		if(m_speed[0] > 0) m_speed[0] = -m_speed[0];
		m_current[0] = (m_range[1].x - Size[1].x);
		m_current[0] <<= 16;
	}

	cc = (m_current[1] += m_speed[1]) >> 16;

	if(cc < (m_range[0].y - Size[0].y))
	{
		if(m_speed[1] < 0) m_speed[1] = -m_speed[1];

		m_current[1] = (m_range[0].y - Size[0].y);
		m_current[1] <<= 16;
	}
	else if(cc > (m_range[1].y - Size[1].y))
	{
		if(m_speed[1] > 0) m_speed[1] = -m_speed[1];
		m_current[1] = (m_range[1].y - Size[1].y);
		m_current[1] <<= 16;
	}

}
//------------------------------------------------------------------------------------------------------------------
void Mover::GetCurrent(point &dst)
{
	dst.x = m_current[0] >> 16;
	dst.y = m_current[1] >> 16;
}
//------------------------------------------------------------------------------------------------------------------

