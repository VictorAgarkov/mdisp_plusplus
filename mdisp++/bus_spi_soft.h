#ifndef BUS_SPI_SOFT_H_INCLUDED
#define BUS_SPI_SOFT_H_INCLUDED

	#include "bus.h"
	#include "config.h"

	class BusSpiSoft : public Bus
	{

		public:

			BusSpiSoft(const struct spi_pins &pins);
			bool IsBusy(void) override {return false;}
			bool StartBuffers(const struct data_buff *buffs, int num) override;
			void Break(void){}

		private:

			struct spi_pins m_pins;

	};



#endif /* BUS_SPI_SOFT_H_INCLUDED */
