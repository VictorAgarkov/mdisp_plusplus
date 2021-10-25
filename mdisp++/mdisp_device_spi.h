#ifndef MDISP_DEVICE_SPI_H_INCLUDED
#define MDISP_DEVICE_SPI_H_INCLUDED

	#include "mdisp_device.h"
	#include "bus.h"

	class MdispDeviceSpi : public MdispDevice
	{
		public:

			MdispDeviceSpi(Bus &bus, const struct pin_def &CS_pin, const struct pin_def &DC_pin, const struct pin_def &RST_pin);
			MdispDeviceSpi(Bus &bus, const struct pin_def &CS_pin);
			void StartBuffer(const uint8_t *buf, int buf_size, int data_cmd) override;
			void SendBuffer(const uint8_t *buf, int buf_size, int data_cmd) override;
			bool IsBusy(void) override {return m_bus->IsBusy();}

			Bus* GetBus(void){return m_bus;}

		private:

			void Init(void);

			Bus *m_bus;
			struct data_buff m_buffs;
			struct pin_def   m_CS_pin;
			struct pin_def   m_DC_pin;
			struct pin_def   m_RST_pin;

	};


#endif /* MDISP_DEVICE_SPI_H_INCLUDED */
