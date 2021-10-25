
#include "bus_i2c_soft.h"
#include "config.h"
#include <string.h>

//-------------------------------------------------------------------------------------------------------------
BusI2cSoft::BusI2cSoft(const struct i2c_pins &pins)
{
	memcpy(&m_pins, &pins, sizeof(m_pins));
	struct pin_def *pd = (struct pin_def *)&m_pins;
	for(unsigned int i = 0; i < sizeof(struct i2c_pins) / sizeof(struct pin_def) ; i++)
	{
		GPIO_OUT_VAL(pd[i], 1);
		GPIO_SET_MODE(pd[i], GPIO_Mode_OUT_OD | GPIO_Speed_2MHz);
	}
}
//-------------------------------------------------------------------------------------------------------------

bool BusI2cSoft::StartBuffers(const struct data_buff *buffs, int num)
{
	for(int idx = 0; idx < num; idx++)
	{
		// check for START condition
		if((buffs[idx].len_flag & I2C_FLAG_START_BEFORE) != 0)
		{
			volatile int dd;
			GPIO_OUT_VAL(m_pins.SDA, 0);  for(dd = 0; dd < 50; dd++);
			GPIO_OUT_VAL(m_pins.SCL, 0);  //for(dd = 0; dd < 3; dd++);
		}

		// send bytes from current buffer
		int byte_num = buffs[idx].len_flag & 0xffff;
		uint8_t* cp = (uint8_t*)buffs[idx].buff;
		for(int offs = 0; offs < byte_num; offs++)
		{
			send_byte(cp[offs]);
		}

		// check for STOP condition
		if((buffs[idx].len_flag & I2C_FLAG_STOP_AFTER) != 0)
		{
			volatile int dd;
			GPIO_OUT_VAL(m_pins.SDA, 0);  //for(dd = 0; dd < 10; dd++);
			GPIO_OUT_VAL(m_pins.SCL, 1);  for(dd = 0; dd < 30; dd++);
			GPIO_OUT_VAL(m_pins.SDA, 1);  //for(dd = 0; dd < 10; dd++);
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------------------
void BusI2cSoft::send_byte(uint8_t data)
{
	int i;
	volatile int dd;
	uint32_t v = data;
	v <<= 1;
	v |= 1; // ack
	for(i = 0; i < 9; i++)
	{
		GPIO_OUT_VAL(m_pins.SDA, v & 0x0100);
		for(dd = 0; dd < 2; dd++);
		v <<= 1;
		GPIO_OUT_VAL(m_pins.SCL, 1); for(dd = 0; dd < 30; dd++);
		GPIO_OUT_VAL(m_pins.SCL, 0); //for(dd = 0; dd < 3; dd++);
	}

}
//-------------------------------------------------------------------------------------------------------------

