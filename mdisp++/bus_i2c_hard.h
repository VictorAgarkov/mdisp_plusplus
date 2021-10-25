#ifndef BUS_I2C_HARD_H_INCLUDED
#define BUS_I2C_HARD_H_INCLUDED

	#include "bus.h"
	#include "dma_isr.h"
	#include "config.h"


	#define I2C_HARD_CCR_DFLT_SPEED 28


	enum I2c_hard_stage
	{
		I2C_STAGE_IDLE = 0,
		I2C_STAGE_PREPARE,
		I2C_STAGE_START,
		I2C_STAGE_DATA,
		I2C_STAGE_STOP
	};

	class BusI2cHard : public Bus
	{

		public:

			BusI2cHard(I2C_TypeDef *const i2c_base, uint8_t speed = I2C_HARD_CCR_DFLT_SPEED);
			bool IsBusy(void) override {return m_stage != I2C_STAGE_IDLE;}
			void SetSpeed(uint8_t speed = I2C_HARD_CCR_DFLT_SPEED){m_speed = speed;}
			bool StartBuffers(const struct data_buff *buffs, int num) override;
			void Break(void);
			int  GetBuffIdx(void){return m_curr_idx;}
			enum I2c_hard_stage GetStage(void){return m_stage;}

			void IsrI2cCallback(void);

		private:

			bool CheckMakeStart(void);
			void CheckMakeStop(void);
			void SendNexByte(void);
			void StartCurrentBuff(void);
			void CheckNextBuff(void);

			I2C_TypeDef *const m_i2c_base;

			// current transfer state
			const struct data_buff *m_buffs;  // buffers set
			int m_buffs_num;                  // buffers num
			enum I2c_hard_stage m_stage;      // current buffer state
			int m_curr_idx;                   // index of currend buffer
			uint16_t m_curr_offs;             // offset in currend buffer
			uint8_t  m_speed;                 // speed factor for CCR
			uint16_t m_last_SR1;              // last value of SR1
	};


#endif /* BUS_I2C_HARD_H_INCLUDED */
