#ifndef BUS_H_INCLUDED
#define BUS_H_INCLUDED

	//#include ""

	#include "stm32f10x_conf.h"
	#include "gpio_F1xx.h"



	#define BUS_FLAG_READ         0x80000000


	#define I2C_FLAG_READ         BUS_FLAG_READ
	#define I2C_FLAG_START_BEFORE 0x40000000
	#define I2C_FLAG_STOP_AFTER   0x20000000
	#define SPI_FLAG_DC           0x80000000   // data/cmd
	#define SPI_FLAG_9BIT         0x40000000   // 9-bit SPI mode (no D/C pin)

	struct i2c_pins
	{
		struct pin_def SDA;
		struct pin_def SCL;
	};

	struct spi_pins
	{
		struct pin_def SCK;
		struct pin_def MOSI;
		struct pin_def MISO;
	};


	struct data_buff
	{
		const void *buff;        // pointer to data
		uint32_t    len_flag;    // data length + some flags

	};


	class Bus
	{
	public:
		virtual bool IsBusy(void) = 0;
		virtual bool StartBuffers(const struct data_buff *buffs, int num) = 0;
	};

#endif /* BUS_H_INCLUDED */
