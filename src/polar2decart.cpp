

#include "polar2decart.h"
#include <string.h>
#include "get_wave.h"


//--------------------------------------------------------------------------------------------------
PolarToDecart::PolarToDecart(struct polar_point P[], int point_num)
:
	m_turn(0),
	m_point_num(point_num)
{

	m_vertex = new struct polar_point[m_point_num];
	SetPolar(P);
}

//--------------------------------------------------------------------------------------------------
PolarToDecart::~PolarToDecart()
{
	delete[] m_vertex;
}
//--------------------------------------------------------------------------------------------------
void   PolarToDecart::SetPolar(struct polar_point P[])
{
	memcpy(m_vertex, P, sizeof(P[0]) * m_point_num);
}
//--------------------------------------------------------------------------------------------------
void PolarToDecart::GetDecart(struct point dst[])
{
	struct point c = {0,0};
	GetDecart(dst, c);
}
//--------------------------------------------------------------------------------------------------
void PolarToDecart::GetDecart(struct point dst[3], struct point &center)
{
	GetDecart(dst, center.x, center.y);
}
//--------------------------------------------------------------------------------------------------
void  PolarToDecart::GetDecart(struct point dst[3], int16_t cx, int16_t cy)
{
	// получить новые координаты вершин треугольника с учётом сдвига и поворота
	for(int i = 0; i < m_point_num; i++)
	{
		uint32_t trn = m_turn + m_vertex[i].turn;
		uint32_t r = m_vertex[i].r;


		int32_t xx = get_wave16(table_s100_m0, table_s100_m0_d, (+trn + 0x40000000));
		int32_t yy = get_wave16(table_s100_m0, table_s100_m0_d, (-trn + 0x00000000));

		xx = (xx * r) >> 14;
		yy = (yy * r) >> 14;

		dst[i].x = xx + cx;
		dst[i].y = yy + cy;
	}
}
//--------------------------------------------------------------------------------------------------

