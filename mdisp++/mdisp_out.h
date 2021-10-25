#ifndef MDISP_OUT_H_INCLUDED
#define MDISP_OUT_H_INCLUDED

	#include <stdint.h>
	#include "gpio_F1xx.h"
	#include "bus.h"


	struct spi_pins
	{
		struct pin_def SDI;  // data
		struct pin_def SCL;  // clock
		struct pin_def RST;  // reset
		struct pin_def DC;   // data/command
		struct pin_def CS;   // chip select
	};
/*
	struct i2c_pins
	{
		struct pin_def SDA;
		struct pin_def SCL;
	};
*/
	//-------------------------------------------------------------------------------------------------------------
	class MdispOut
	{
	public:
		virtual void Out(const uint8_t *data, int num, int data_cmd) = 0;   // do nothing
	};

	//-------------------------------------------------------------------------------------------------------------
	class MdispOut_sw_i2c : public MdispOut
	{
		public:
			MdispOut_sw_i2c(const struct i2c_pins &pins, uint8_t i2c_addr);
			void  Out(const uint8_t *data, int num, int data_cmd) override;
		private:
			void  send_byte(uint8_t data);
			void  start_addr(void);
			void  stop(void);
			const struct i2c_pins * m_pins;
			uint8_t m_Addr;
	};

	//-------------------------------------------------------------------------------------------------------------
	class MdispOut_sw_spi : public MdispOut
	{
		public:
			MdispOut_sw_spi(const struct spi_pins &pins);
			void  Out(const uint8_t *data, int num, int data_cmd) override;
		private:
			void  send_byte(uint8_t data);
			const struct spi_pins * m_pins;
			//bool m_dedicated_DC;
	};

	//-------------------------------------------------------------------------------------------------------------


#endif /* MDISP_OUT_H_INCLUDED */
