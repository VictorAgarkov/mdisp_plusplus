#ifndef BUS_I2C_SOFT_H_INCLUDED
#define BUS_I2C_SOFT_H_INCLUDED

	#include "bus.h"
	#include "config.h"

	class BusI2cSoft : public Bus
	{

		public:

			BusI2cSoft(const struct i2c_pins &pins);
			bool IsBusy(void) override {return false;}
			bool StartBuffers(const struct data_buff *buffs, int num) override;
			void Break(void){}

		private:

			void    send_byte(uint8_t data);
			struct i2c_pins  m_pins;

	};


#endif /* BUS_I2C_SOFT_H_INCLUDED */
