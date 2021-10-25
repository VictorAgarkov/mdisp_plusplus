#include "bus_i2c_hard_dma.h"

#define I2C_CR2_FREQ_v 36

//-------------------------------------------------------------------------------------------------------------

static BusI2cHardDma *s_I2cHard_I2cIsr_client[2] = {0, 0};

static const struct i2c_pins g_pins[2] =
{
	// SDA         SCL
	{ {GPIOB,  7}, {GPIOB,  6} },       // i2c1
	{ {GPIOB, 11}, {GPIOB, 10} }        // i2c2
};

//-------------------------------------------------------------------------------------------------------------

BusI2cHardDma::BusI2cHardDma(I2C_TypeDef *const i2c_base)
:
	m_i2c_base(i2c_base),
	dma_tx_ch(-1),
	dma_rx_ch(-1)
{

	struct pin_def *pd = 0;
	enum IRQn i2c_irqn; // = (enum IRQn) -10;

	// get DMA channels & GPIO pins
	if(m_i2c_base == I2C1)
	{
		dma_tx_ch = 5;   // channel 6
		dma_rx_ch = 6;   // channel 7
		s_I2cHard_I2cIsr_client[0] = this;
		pd = (pin_def*) &g_pins[0];
		i2c_irqn = I2C1_EV_IRQn;

	}
	else if(m_i2c_base == I2C2)
	{
		dma_tx_ch = 3;   // channel 4
		dma_rx_ch = 4;   // channel 5
		s_I2cHard_I2cIsr_client[1] = this;
		pd = (pin_def*) &g_pins[1];
		i2c_irqn = I2C2_EV_IRQn;
	}
	else
	{
		return;
	}

	// init pins
	for(unsigned int i = 0; i < sizeof(struct i2c_pins) / sizeof(struct pin_def) ; i++)
	{
		GPIO_OUT_VAL(pd[i], 1);
		GPIO_SET_MODE(pd[i], GPIO_Mode_AF_OD | GPIO_Speed_2MHz);
	}

	// register this object as client for DMA IRQ
	SetDmaIsrClent(1, dma_rx_ch + 1, this);
	SetDmaIsrClent(1, dma_tx_ch + 1, this);

	// enable I2C IRQ
	NVIC_EnableIRQ(i2c_irqn);
}

//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::MakeStart(void)
{
	m_stage = I2C_STAGE_START;
	m_i2c_base->CR2 = I2C_CR2_ITEVTEN | I2C_CR2_FREQ_v;
	m_i2c_base->CR1 = I2C_CR1_PE | I2C_CR1_START;

}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::MakeStop(void)
{
	m_i2c_base->CR2 = I2C_CR2_ITEVTEN | I2C_CR2_FREQ_v;
	m_i2c_base->CR1 = I2C_CR1_PE | I2C_CR1_STOP;
	m_stage = I2C_STAGE_IDLE;
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::StartDmaAfterSTART(void)
{
	// ��������� �������� �� DMA ����� �������� START condition
	// ����� ����� ������������� �������� ������ ���� (��� �����)
	// � ������� ������, � ��������� ����� �������� ��� ������ DMA
	//
	// (����� ���� ������� ��� �� ��������� �����, ��� � �� �����������)


	uint32_t addr = (uint32_t)DMA1_Channel1;
	addr += dma_tx_ch * 20;
	uint16_t len = m_buffs[m_curr_idx].len_flag;
	uint16_t cr2 = 0;
	uint8_t *u8p = (uint8_t*) m_buffs[m_curr_idx].buff;


	if(len > 1)
	{
		// ���� ����� 1 ����� ��� �������� - ��������� �������� ���� ����� �������
		DMA_Channel_TypeDef* dma_ch = (DMA_Channel_TypeDef*)addr;
		dma_ch->CCR   = 0;
		dma_ch->CNDTR = len - 1;
		dma_ch->CPAR  = (uint32_t) &m_i2c_base->DR;
		dma_ch->CMAR  = (uint32_t) (u8p + 1);
		dma_ch->CCR   = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_TCIE | DMA_CCR1_EN;

		cr2 = I2C_CR2_DMAEN | I2C_CR2_FREQ_v;

	}
	else
	{
		cr2 = I2C_CR2_ITBUFEN | I2C_CR2_FREQ_v;
	}

	m_stage = I2C_STAGE_DATA;

	m_i2c_base->DR = u8p[0];
	m_i2c_base->CR2 = cr2;

}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::StartDmaContinued(void)
{
	// ���������� �������� �� DMA ����� �������� ����� ������
	// (��������� ����������� DMA)
	uint32_t addr = (uint32_t)DMA1_Channel1;
	addr += dma_tx_ch * 20;
	uint16_t len = m_buffs[m_curr_idx].len_flag;

	// init DMA
	DMA_Channel_TypeDef* dma_ch = (DMA_Channel_TypeDef*)addr;
	dma_ch->CCR   = 0;
	dma_ch->CNDTR = len;
	dma_ch->CPAR  = (uint32_t) &m_i2c_base->DR;
	dma_ch->CMAR  = (uint32_t) m_buffs[m_curr_idx].buff;
	dma_ch->CCR   = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_TCIE | DMA_CCR1_EN;

	m_i2c_base->CR2 = I2C_CR2_DMAEN | I2C_CR2_FREQ_v;
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::StartCurrentBuff(void)
{
	if(m_buffs[m_curr_idx].len_flag & I2C_FLAG_START_BEFORE)
	{
		// ����� START - ��������
		MakeStart();
		// ����� ��� ����� ����������
	}
	else
	{
		// �� ����� ����� - ��������� ��������/���� ������� ������
		StartDmaContinued();
		// ����� ��� ����� ����������
	}
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::CheckNextBuff(void)
{
	// ���� ��������, ����� ��������� ��� ������ � ������� �������


	if(++m_curr_idx < m_buffs_num)
	{
		// ��� ���� ����� ��� ��������
		m_stage = I2C_STAGE_PREPARE;
		StartCurrentBuff();
	}
	else
	{
		// �� ��������

		m_i2c_base->CR1 = 0;
		DMA1_Channel1[dma_tx_ch].CCR = 0;
		m_stage = I2C_STAGE_IDLE;
	}
}
//-------------------------------------------------------------------------------------------------------------
bool BusI2cHardDma::StartBuffers(const struct data_buff *buffs, int num)
{
	// �������� ����� ���������� �������
	// �������:
	// false - �������� �� ������
	// true  - �������� ������, ���� ���������

	if(dma_tx_ch < 0 || dma_rx_ch < 0)     return false;
	if(!num)                               return false;
	if(!buffs)                             return false;
	if((buffs[0].len_flag & 0xffff) == 0)  return false;
	if(!buffs[0].buff)                     return false;

	m_buffs = buffs;
	m_buffs_num = num;
	m_curr_idx = 0;
	m_stage = I2C_STAGE_PREPARE;


	m_i2c_base->CCR = 10 | I2C_CCR_FS;

	StartCurrentBuff();
	return true;
}

//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::DmaIsr(uint32_t dma_isr_value)
{
	CheckNextBuff();
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHardDma::IsrI2cCallback(void)
{
	// clear event flags by reading status registers
	m_i2c_base->SR1;
	m_i2c_base->SR2;

	m_i2c_base->CR2 = 0 | I2C_CR2_FREQ_v; // disable interrupts
	switch(m_stage)
	{
		case I2C_STAGE_START:
			// ����� �������� 'start condition' ��������� �������� �� DMA
			StartDmaAfterSTART();
		break;
		case I2C_STAGE_STOP:
			// ����� �������� 'stop condition' �������, ���� �� ��� ����������
			CheckNextBuff();
		break;
		case I2C_STAGE_PREPARE:
			// ����� �������� ����� ������ (��������� DMA) ��������� �������� �� DMA
			StartDmaContinued();
		break;

		default:;
	}
}
//-------------------------------------------------------------------------------------------------------------

// ISR handlers

extern "C"
{
	// I2C ISR handlers
	void I2C1_EV_IRQHandler(void){if(s_I2cHard_I2cIsr_client[0]) s_I2cHard_I2cIsr_client[0]->IsrI2cCallback();}
	void I2C2_EV_IRQHandler(void){if(s_I2cHard_I2cIsr_client[1]) s_I2cHard_I2cIsr_client[1]->IsrI2cCallback();}
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

