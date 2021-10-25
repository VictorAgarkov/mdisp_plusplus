
#include "bus_spi_soft.h"
#include "config.h"
#include <string.h>


//-------------------------------------------------------------------------------------------------------------
BusSpiSoft::BusSpiSoft(const struct spi_pins &pins)
{
	memcpy(&m_pins, &pins, sizeof(m_pins));
	struct pin_def *pd = (struct pin_def *)&m_pins;
	for(unsigned int i = 0; i < sizeof(struct spi_pins) / sizeof(struct pin_def) ; i++)
	{
		if(pd[i].port)
		{
			GPIO_OUT_VAL(pd[i], 1);
			GPIO_SET_MODE(pd[i], GPIO_Mode_OUT_PP | GPIO_Speed_10MHz);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------

bool BusSpiSoft::StartBuffers(const struct data_buff *buffs, int num)
{
	for(int idx = 0; idx < num; idx++)
	{
		int data_cmd = 0;
		// DATA / CMD flag
		if((buffs[idx].len_flag & SPI_FLAG_DC) != 0)
		{
			data_cmd = 1;
		}

		// send bytes from current buffer
		int byte_num = buffs[idx].len_flag & 0xffff;
		uint8_t* cp = (uint8_t*)buffs[idx].buff;

		for(int offs = 0; offs < byte_num; offs++)
		{
			if((buffs[idx].len_flag & SPI_FLAG_9BIT) != 0)
			{
				// 9-bit serial send (no dedicate D/C pin)
				GPIO_OUT_VAL(m_pins.SCK, 0); //  MDISP_SCLK_RESET;
				GPIO_OUT_VAL(m_pins.MOSI, data_cmd);
				GPIO_OUT_VAL(m_pins.SCK, 1); //  MDISP_SCLK_SET;
			}

			// send bytes from current buffer
			uint8_t dd = cp[offs];

			for(int i = 0; i < 8; i++)
			{
				// 8 bits data send
				GPIO_OUT_VAL(m_pins.SCK, 0); //  MDISP_SCLK_RESET;
				GPIO_OUT_VAL(m_pins.MOSI, dd & 0x80);
				dd <<= 1;
				GPIO_OUT_VAL(m_pins.SCK, 1); //  MDISP_SCLK_SET;
			}
		}
	}
	return false;
}
//-------------------------------------------------------------------------------------------------------------

