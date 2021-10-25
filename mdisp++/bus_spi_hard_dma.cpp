
#include "bus_spi_hard_dma.h"
#include "config.h"
#include <string.h>

//-------------------------------------------------------------------------------------------------------------


static const struct spi_pins s_spi_pins[] =
{
	//  SCK           MOSI          MISO
	{ {GPIOA,  5},  {GPIOA,  7},  {GPIOA,  6} },   // SPI1
	{ {GPIOB, 13},  {GPIOB, 15},  {GPIOB, 14} }    // SPI2
};

//-------------------------------------------------------------------------------------------------------------

BusSpiHardDma::BusSpiHardDma(SPI_TypeDef * const spi_base, int bit_rate)
:
	m_spi_base(spi_base),
	m_dma_tx_channel(-1),
	m_dma_tx_base(0),
	m_buffs_num(-1),
	m_bit_rate(bit_rate & 7)
{
	const struct spi_pins *pins;
	if(m_spi_base == SPI1)
	{
		pins = s_spi_pins + 0;
		m_dma_tx_channel = 2; // DMA1 channel 3
	}
	else if(m_spi_base == SPI2)
	{
		pins = s_spi_pins + 1;
		m_dma_tx_channel = 4; // DMA1 channel 5
	}
	else return;

	// инициализируем первые 2 вывода (SCK, MOSI) на выход
	struct pin_def *pd = (struct pin_def *)pins;
	for(unsigned int i = 0; i < 2 ; i++)
	{
		if(pd[i].port)
		{
			GPIO_OUT_VAL(pd[i], 1);
			GPIO_SET_MODE(pd[i], GPIO_Mode_AF_PP | GPIO_Speed_10MHz);
		}
	}
	// вычисляем базу регистров нужного канала DMA
	uint32_t addr = (uint32_t)DMA1_Channel1;
	addr += m_dma_tx_channel * 20;
	m_dma_tx_base = (DMA_Channel_TypeDef*)addr;

	// регистрируемся как обработчик прерываний от DMA
	SetDmaIsrClent(1, m_dma_tx_channel + 1, this);
}
//-------------------------------------------------------------------------------------------------------------

bool BusSpiHardDma::StartBuffers(const struct data_buff *buffs, int num)
{
	// стартуем вывод нескольких буферов
	// возврат:
	// false - передача не начата
	// true  - передача начата, надо подождать

	if(m_dma_tx_channel < 0)     return false;
	if(!num)                               return false;
	if(!buffs)                             return false;
	if((buffs[0].len_flag & 0xffff) == 0)  return false;
	if(!buffs[0].buff)                     return false;

	m_buffs = buffs;
	m_buffs_num = num;
	m_curr_idx = 0;
	StartCurrentBuff();
	return true;
}
//-------------------------------------------------------------------------------------------------------------
void BusSpiHardDma::DmaIsr(uint32_t dma_isr_value)
{
	CheckNextBuff();
}
//-------------------------------------------------------------------------------------------------------------
void BusSpiHardDma::Break(void)
{
	while(m_spi_base->SR & SPI_SR_BSY);
	m_spi_base->CR1 = 0;
	m_dma_tx_base->CCR   = 0;
	m_buffs_num = -1;
}
//-------------------------------------------------------------------------------------------------------------
void BusSpiHardDma::StartCurrentBuff(void)
{
	// начинаем передачу по DMA
	uint16_t len = m_buffs[m_curr_idx].len_flag;

	// init DMA
	m_dma_tx_base->CCR   = 0;
	m_dma_tx_base->CNDTR = len;
	m_dma_tx_base->CPAR  = (uint32_t) &m_spi_base->DR;
	m_dma_tx_base->CMAR  = (uint32_t) m_buffs[m_curr_idx].buff;
	m_dma_tx_base->CCR   = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_TCIE | DMA_CCR1_EN;

	// init SPI
	m_spi_base->CR2 = SPI_CR2_TXDMAEN;
	// start!!!
	m_spi_base->CR1 = SPI_CR1_MSTR | (m_bit_rate << 3) | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE;

}
//-------------------------------------------------------------------------------------------------------------
void BusSpiHardDma::CheckNextBuff(void)
{
	// сюда попадаем, когда выполнили всю работу с текущим буфером


	if(++m_curr_idx < m_buffs_num)
	{
		// ещё есть буфер для передачи
		StartCurrentBuff();
	}
	else
	{
		// всё передали
		Break();
	}
}
//-------------------------------------------------------------------------------------------------------------

