#ifndef BUS_SPI_HARD_DMA_H_INCLUDED
#define BUS_SPI_HARD_DMA_H_INCLUDED

	#include "bus.h"
	#include "config.h"
	#include "dma_isr.h"

	class BusSpiHardDma : public Bus, DmaIsrClient
	{

		public:

			BusSpiHardDma(SPI_TypeDef * const spi_base, int bit_rate = 1);
			bool IsBusy(void) override {return m_buffs_num >= 0;}
			void SetSpeed(int bit_rate){m_bit_rate = bit_rate & 7;}
			bool StartBuffers(const struct data_buff *buffs, int num) override;
			void Break(void);
			void DmaIsr(uint32_t dma_isr_value) override;  // callback

		private:

			void StartDma(void);
			void StartCurrentBuff(void);
			void CheckNextBuff(void);

			SPI_TypeDef * const m_spi_base;
			int      m_dma_tx_channel;
			DMA_Channel_TypeDef* m_dma_tx_base;

			const struct data_buff *m_buffs;  // buffers set
			int m_buffs_num;                  // buffers num
			int m_curr_idx;                   // index of currend buffer
			int m_bit_rate;



	};



#endif /* BUS_SPI_HARD_DMA_H_INCLUDED */
