#ifndef BUS_I2C_HARD_DMA_H_INCLUDED
#define BUS_I2C_HARD_DMA_H_INCLUDED

	#include "bus.h"
	#include "dma_isr.h"
	#include "config.h"

	#define I2C_ADDR_WR(a) (((a) << 1) | 0)
	#define I2C_ADDR_RD(a) (((a) << 1) | 1)


	enum I2c_hard_stage
	{
		I2C_STAGE_IDLE = 0,
		I2C_STAGE_PREPARE,
		I2C_STAGE_START,
		I2C_STAGE_DATA,
		I2C_STAGE_STOP
	};

	class BusI2cHardDma : public Bus, public DmaIsrClient
	{

		public:

			BusI2cHardDma(I2C_TypeDef *const i2c_base);
			bool IsBusy(void) override {return m_stage != I2C_STAGE_IDLE;}
			bool StartBuffers(const struct data_buff *buffs, int num) override;
			int  GetDmaRxChn(void){return dma_rx_ch;}
			int  GetDmaTxChn(void){return dma_tx_ch;}
			int  GetBuffIdx(void){return m_curr_idx;}
			enum I2c_hard_stage GetStage(void){return m_stage;}

			void DmaIsr(uint32_t dma_isr_value) override;
			void IsrI2cCallback(void);

		private:

			void StartDmaAfterSTART(void);
			void StartDmaContinued(void);
			void MakeStart(void);
			void MakeStop(void);
			void StartCurrentBuff(void);
			void CheckNextBuff(void);

			I2C_TypeDef *const m_i2c_base;
			int dma_tx_ch, dma_rx_ch;        // номера каналов DMA, начинаются с 0 (не с 1, как в документации!)

			// current transfer state
			const struct data_buff *m_buffs;  // buffers set
			int m_buffs_num;                  // buffers num
			enum I2c_hard_stage m_stage;      // current buffer state
			int m_curr_idx;                   // index of currend buffer
	};


#endif /* BUS_I2C_HARD_DMA_H_INCLUDED */
