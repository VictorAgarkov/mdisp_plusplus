
#include "mdisp_device_i2c.h"

//-------------------------------------------------------------------------------------------------------------

MdispDeviceI2c::MdispDeviceI2c(Bus &bus, uint8_t i2c_addr)
:
	m_bus(&bus),
	m_addr(i2c_addr)
{

	m_buff4[0] = (m_addr << 1) | 0;
	// buffers
	m_buffs[0].buff     = m_buff4;
	m_buffs[0].len_flag = 2 | I2C_FLAG_START_BEFORE;
}
//-------------------------------------------------------------------------------------------------------------
void MdispDeviceI2c::SendBuffer(const uint8_t *buf, int buf_size, int data_cmd)
{
	StartBuffer(buf, buf_size, data_cmd);
	while(m_bus->IsBusy());
}
//-------------------------------------------------------------------------------------------------------------
void MdispDeviceI2c::StartBuffer(const uint8_t *buf, int buf_size, int data_cmd)
{
	// buff with i2c addres & data_cmd feature
	m_buff4[1] = (data_cmd != 0) ? 0x40 : 0;

	m_buffs[1].buff     = buf;
	m_buffs[1].len_flag = buf_size | I2C_FLAG_STOP_AFTER;

	m_bus->StartBuffers(m_buffs, 2);
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------


