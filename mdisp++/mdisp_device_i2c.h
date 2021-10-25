#ifndef MDISP_DEVICE_I2C_H_INCLUDED
#define MDISP_DEVICE_I2C_H_INCLUDED

	#include "mdisp_device.h"
	#include "bus.h"

	class MdispDeviceI2c : public MdispDevice
	{
		public:

			MdispDeviceI2c(Bus &bus, uint8_t i2c_addr);
			void StartBuffer(const uint8_t *buf, int buf_size, int data_cmd) override;
			void SendBuffer(const uint8_t *buf, int buf_size, int data_cmd) override;
			bool IsBusy(void) override {return m_bus->IsBusy();}

			uint8_t GetAddr(void){return m_addr;}
			Bus* GetBus(void){return m_bus;}

		private:

			Bus *m_bus;
			uint8_t m_addr;
			uint8_t m_buff4[4];
			struct data_buff m_buffs[2];

	};

#endif /* MDISP_DEVICE_I2C_H_INCLUDED */
