#include "bus_i2c_hard.h"

#define I2C_CR2_FREQ_v 36
#define I2C_ADDR_WR(a) (((a) << 1) | 0)
#define I2C_ADDR_RD(a) (((a) << 1) | 1)

//-------------------------------------------------------------------------------------------------------------

static BusI2cHard *s_I2cHard_I2cIsr_client[2] = {0, 0};

static const struct i2c_pins g_pins[2] =
{
	// SDA         SCL
	{ {GPIOB,  7}, {GPIOB,  6} },       // i2c1
	{ {GPIOB, 11}, {GPIOB, 10} }        // i2c2
};

//-------------------------------------------------------------------------------------------------------------

BusI2cHard::BusI2cHard(I2C_TypeDef *const i2c_base, uint8_t speed)
:
	m_i2c_base(i2c_base),
	m_stage(I2C_STAGE_IDLE),
	m_speed(speed),
	m_buffs(0),
	m_buffs_num(0),
	m_curr_offs(0),
	m_last_SR1(0),
	m_curr_idx(0)
	
{
	struct pin_def *pd;
	enum IRQn i2c_irqn;

	// get DMA channels & GPIO pins
	if(m_i2c_base == I2C1)
	{
		s_I2cHard_I2cIsr_client[0] = this;
		pd = (pin_def*) &g_pins[0];
		i2c_irqn = I2C1_EV_IRQn;
	}
	else if(m_i2c_base == I2C2)
	{
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

	// enable I2C IRQ
	NVIC_EnableIRQ(i2c_irqn);
}

//-------------------------------------------------------------------------------------------------------------
bool BusI2cHard::CheckMakeStart(void)
{
	if((m_buffs[m_curr_idx].len_flag & I2C_FLAG_START_BEFORE) != 0)
	{
		m_stage = I2C_STAGE_START;
		m_i2c_base->CR2 = I2C_CR2_ITEVTEN | I2C_CR2_FREQ_v;
		m_i2c_base->CR1 = I2C_CR1_PE | I2C_CR1_START;
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::CheckMakeStop(void)
{
	if((m_buffs[m_curr_idx].len_flag & I2C_FLAG_STOP_AFTER) != 0)
	{
		while(!(m_i2c_base->SR1 & I2C_SR1_BTF));
		m_stage = I2C_STAGE_STOP;
		m_i2c_base->CR1 = I2C_CR1_PE | I2C_CR1_STOP;
		while(m_i2c_base->SR1 & I2C_SR2_BUSY);
	}
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::SendNexByte(void)
{
	if(m_curr_offs < (m_buffs[m_curr_idx].len_flag & 0xffff))
	{
		// есть байты для передачи в текущем буфере
		m_stage = I2C_STAGE_DATA;
		uint8_t* cp = (uint8_t*) m_buffs[m_curr_idx].buff;

		m_i2c_base->DR = cp[m_curr_offs++];
		m_i2c_base->CR2 = I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_FREQ_v;
		// здесь уже можем прерваться

		if(m_last_SR1 & I2C_SR1_SB)
		{
			// после передачи адреса ожидаем либо завершения его передачи,
			// либо срабатывания Acknowledge failure (AF)
			// потому что прерываеня по AF не возникает
			for(;;)
			{
				uint16_t sr1 = m_i2c_base->SR1;
				if(sr1 & I2C_SR1_AF)
				{
					// по Acknowledge failure завершаем передачу
					Break();
					break;
				}
				if(sr1 & I2C_SR1_ADDR)
				{
					// после передачи адреса продолжаем работу
					break;
				}
			}
		}
	}
	else
	{
		// текущий буфер передан
		CheckMakeStop();
		CheckNextBuff();
	}
}

//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::StartCurrentBuff(void)
{
	m_curr_offs = 0;
	//m_i2c_base->CR2 = I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_FREQ_v;
	if(!CheckMakeStart())
	{
		// не нужен старт - запускаем передачу/приём
		SendNexByte();
	}
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::CheckNextBuff(void)
{
	// сюда попадаем, когда выполнили всю работу с текущим буфером
	if(++m_curr_idx < m_buffs_num)
	{
		// ещё есть буфер для передачи
		m_stage = I2C_STAGE_PREPARE;
		StartCurrentBuff();
	}
	else
	{
		// всё передали
		Break();
	}
}
//-------------------------------------------------------------------------------------------------------------
bool BusI2cHard::StartBuffers(const struct data_buff *buffs, int num)
{
	// стартуем вывод нескольких буферов
	// возврат:
	// false - передача не начата
	// true  - передача начата, надо подождать

	if(!num)                               return false;
	if(!buffs)                             return false;
	if((buffs[0].len_flag & 0xffff) == 0)  return false;
	if(!buffs[0].buff)                     return false;

	m_buffs         = buffs;
	m_buffs_num     = num;
	m_curr_idx      = 0;
	m_stage         = I2C_STAGE_PREPARE;
	m_i2c_base->CCR = m_speed | I2C_CCR_FS;

	StartCurrentBuff();
	return true;
}

//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::Break(void)
{
	m_i2c_base->CR2 = 0; // disable interrupts
	m_i2c_base->CR1 = 0; // disable i2c
	m_i2c_base->SR1 = 0; // clear all flags
	m_stage = I2C_STAGE_IDLE;
}
//-------------------------------------------------------------------------------------------------------------
void BusI2cHard::IsrI2cCallback(void)
{
	// clear event flags by reading status registers
	m_last_SR1 = m_i2c_base->SR1;
	m_i2c_base->SR2;  // read SR2

	if(m_last_SR1 & (I2C_SR1_AF | I2C_SR1_BERR))
	{
		Break();
	}
	else if(m_last_SR1 & (I2C_SR1_RXNE | I2C_SR1_TXE | I2C_SR1_SB))
	{
		SendNexByte();
	}
}
//-------------------------------------------------------------------------------------------------------------

// I2C ISR handlers

extern "C"
{
	// I2C ISR handlers
	void I2C1_EV_IRQHandler(void){if(s_I2cHard_I2cIsr_client[0]) s_I2cHard_I2cIsr_client[0]->IsrI2cCallback();}
	void I2C2_EV_IRQHandler(void){if(s_I2cHard_I2cIsr_client[1]) s_I2cHard_I2cIsr_client[1]->IsrI2cCallback();}
}
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

