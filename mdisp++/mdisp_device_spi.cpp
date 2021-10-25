
#include "mdisp_device_spi.h"
#include <string.h>
//-------------------------------------------------------------------------------------------------------------
MdispDeviceSpi::MdispDeviceSpi(Bus &bus, const struct pin_def &CS_pin, const struct pin_def &DC_pin, const struct pin_def &RST_pin)
:
	m_bus(&bus)
{
	memcpy(&m_CS_pin, &CS_pin, sizeof(m_CS_pin));
	memcpy(&m_DC_pin, &DC_pin, sizeof(m_DC_pin));
	memcpy(&m_RST_pin, &RST_pin, sizeof(RST_pin));
	Init();
}
//-------------------------------------------------------------------------------------------------------------
MdispDeviceSpi::MdispDeviceSpi(Bus &bus, const struct pin_def &CS_pin)
:
	m_bus(&bus)
{
	memcpy(&m_CS_pin, &CS_pin, sizeof(m_CS_pin));
	m_DC_pin.port = 0;
	m_RST_pin.port = 0;
	Init();
}
//-------------------------------------------------------------------------------------------------------------
void MdispDeviceSpi::Init(void)
{
	GPIO_OUT_VAL(m_CS_pin, 1);
	GPIO_SET_MODE(m_CS_pin, GPIO_Mode_OUT_PP | GPIO_Speed_10MHz);
	if(m_DC_pin.port)
	{
		GPIO_OUT_VAL(m_DC_pin, 1);
		GPIO_SET_MODE(m_DC_pin, GPIO_Mode_OUT_PP | GPIO_Speed_10MHz);
	}
	if(m_RST_pin.port)
	{
		GPIO_OUT_VAL(m_RST_pin, 1);
		GPIO_SET_MODE(m_RST_pin, GPIO_Mode_OUT_PP | GPIO_Speed_10MHz);
	}
}
//-------------------------------------------------------------------------------------------------------------
void MdispDeviceSpi::StartBuffer(const uint8_t *buf, int buf_size, int data_cmd)
{
	uint32_t flags = 0;

	if(!m_DC_pin.port) flags  = SPI_FLAG_9BIT;
	if(data_cmd)       flags |= SPI_FLAG_DC;

	m_buffs.buff     = buf;
	m_buffs.len_flag = buf_size | flags;

	m_bus->StartBuffers(&m_buffs, 1);
	//while(m_bus->IsBusy());  // debug

}
//-------------------------------------------------------------------------------------------------------------
void MdispDeviceSpi::SendBuffer(const uint8_t *buf, int buf_size, int data_cmd)
{
	GPIO_OUT_VAL(m_CS_pin, 0);

	if(m_DC_pin.port) GPIO_OUT_VAL(m_DC_pin, data_cmd);

	StartBuffer(buf, buf_size, data_cmd);
	while(m_bus->IsBusy());

	if(m_DC_pin.port) GPIO_OUT_VAL(m_DC_pin, 1);
	GPIO_OUT_VAL(m_CS_pin, 1);

}
//-------------------------------------------------------------------------------------------------------------

