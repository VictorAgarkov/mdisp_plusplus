
#include "mdisp_out.h"
#include "config.h"
#include <string.h>

//-------------------------------------------------------------------------------------------------------------
MdispOut_sw_i2c::MdispOut_sw_i2c(const struct i2c_pins &pins, uint8_t i2c_addr)
:
	m_pins(&pins),
	m_Addr(i2c_addr)
{
	struct pin_def *pd = (struct pin_def *)m_pins;
	for(unsigned int i = 0; i < sizeof(struct i2c_pins) / sizeof(struct pin_def) ; i++)
	{
		GPIO_OUT_VAL(pd[i], 1);
		GPIO_SET_MODE(pd[i], GPIO_Mode_OUT_OD | GPIO_Speed_2MHz);
	}
}
//-------------------------------------------------------------------------------------------------------------
void MdispOut_sw_i2c::Out(const uint8_t *data, int num, int data_cmd)
{
	int i;
	uint8_t v;
	start_addr();

	v = (data_cmd != 0) ? 0x40 : 0;
	//v |= 0x80; // "Co" bit (?)
	send_byte(v);  // send control byte
	for(i = 0; i < num; i++)
	{
			send_byte(data[i]);  // send data byte
	}
	stop();
}

//-------------------------------------------------------------------------------------------------------------
void MdispOut_sw_i2c::send_byte(uint8_t data)
{
	int i;
	volatile int dd;
	uint32_t v = data;
	v <<= 1;
	v |= 1; // ack
	for(i = 0; i < 9; i++)
	{
		GPIO_OUT_VAL(m_pins->SDA, v & 0x0100);
		for(dd = 0; dd < 2; dd++);
		v <<= 1;
		GPIO_OUT_VAL(m_pins->SCL, 1); for(dd = 0; dd < 3; dd++);
		GPIO_OUT_VAL(m_pins->SCL, 0); //for(dd = 0; dd < 3; dd++);
	}

}
//-------------------------------------------------------------------------------------------------------------
void MdispOut_sw_i2c::start_addr(void)
{
	// make start condition
	// I2C start
	volatile int dd;
	GPIO_OUT_VAL(m_pins->SDA, 0);  for(dd = 0; dd < 50; dd++);
	GPIO_OUT_VAL(m_pins->SCL, 0);  //for(dd = 0; dd < 3; dd++);

	send_byte(m_Addr << 1);  // send addres + wr(0)

}
//-------------------------------------------------------------------------------------------------------------
void MdispOut_sw_i2c::stop(void)
{
	// I2C stop
	volatile int dd;
	GPIO_OUT_VAL(m_pins->SDA, 0);  //for(dd = 0; dd < 10; dd++);
	GPIO_OUT_VAL(m_pins->SCL, 1);  for(dd = 0; dd < 50; dd++);
	GPIO_OUT_VAL(m_pins->SDA, 1);  //for(dd = 0; dd < 10; dd++);

}

//=============================================================================================================

MdispOut_sw_spi::MdispOut_sw_spi(const struct spi_pins &pins)
:
	m_pins(&pins)
{
	struct pin_def *pd = (struct pin_def *)m_pins;
	for(unsigned int i = 0; i < sizeof(struct spi_pins) / sizeof(struct pin_def) ; i++)
	{
		if(pd[i].port)
		{
			GPIO_OUT_VAL(pd[i], 1);
			GPIO_SET_MODE(pd[i], GPIO_Mode_OUT_PP | GPIO_Speed_10MHz);
		}
	}
};

//-------------------------------------------------------------------------------------------------------------
void MdispOut_sw_spi::Out(const uint8_t *data, int num, int data_cmd)
{
	// software serialization
	GPIO_OUT_VAL(m_pins->CS, 0); // chip select;

	if(m_pins->DC.port != 0)
	{
		// set dedicated 'data/control' pin if present
		GPIO_OUT_VAL(m_pins->DC, data_cmd);
	}

	for(int n = 0; n < num; n++)
	{
		uint8_t dd = data[n];

		if(m_pins->DC.port == 0)
		{
			// 9-bit serial send (no dedicate D/C pin)
			GPIO_OUT_VAL(m_pins->SCL, 0); //  MDISP_SCLK_RESET;
			GPIO_OUT_VAL(m_pins->SDI, data_cmd);
			GPIO_OUT_VAL(m_pins->SCL, 1); //  MDISP_SCLK_SET;
		}

		for(int i = 0; i < 8; i++)
		{
			// 8 bits data send
			GPIO_OUT_VAL(m_pins->SCL, 0); //  MDISP_SCLK_RESET;
			GPIO_OUT_VAL(m_pins->SDI, dd & 0x80);
			dd <<= 1;
			GPIO_OUT_VAL(m_pins->SCL, 1); //  MDISP_SCLK_SET;
		}
	}
	GPIO_OUT_VAL(m_pins->SDI, 1); //MDISP_D_SET;
	GPIO_OUT_VAL(m_pins->CS, 1); // MDISP_CS_SET;
}


//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
