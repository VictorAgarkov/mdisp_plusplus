
#include "dma_isr.h"
#include "stm32f10x_conf.h"


static DmaIsrClient *s_Dma1Clients[7] = {0};
//static DmaIsrClient *s_Dma2Clients[5] = {0};


void SetDmaIsrClent(int dma, int channel, DmaIsrClient *client)
{
	channel--;

	if(channel < 0) return;
	switch(dma)
	{
		case 1:
			if(channel < 7)
			{
				s_Dma1Clients[channel] = client;
				enum IRQn irqn = (enum IRQn)(DMA1_Channel1_IRQn + channel);
				if(client) NVIC_EnableIRQ(irqn);
				else       NVIC_DisableIRQ(irqn);
			}
		break;
//		case 2:
//			if(channel < 5)
//			{
//				s_Dma2Clients[channel] = client;
//				enum IRQn irqn = (enum IRQn)(DMA2_Channel1_IRQn + channel);
//				if(client) NVIC_EnableIRQ(irqn);
//				else       NVIC_DisableIRQ(irqn);
//			}
//		break;
	}
}


extern "C"
{
	// DMA ISR handlers

	// ------------- DMA 1 channel 1 ----------------
	void DMA1_Channel1_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 0;
		if(s_Dma1Clients[0]) s_Dma1Clients[0]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 2 ----------------
	void DMA1_Channel2_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 1 * 4;
		if(s_Dma1Clients[1]) s_Dma1Clients[1]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 3 ----------------
	void DMA1_Channel3_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 2 * 4;
		if(s_Dma1Clients[2]) s_Dma1Clients[2]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 4 ----------------
	void DMA1_Channel4_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 3 * 4;
		if(s_Dma1Clients[3]) s_Dma1Clients[3]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 5 ----------------
	void DMA1_Channel5_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 4 * 4;
		if(s_Dma1Clients[4]) s_Dma1Clients[4]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 6 ----------------
	void DMA1_Channel6_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 5 * 4;
		if(s_Dma1Clients[5]) s_Dma1Clients[5]->DmaIsr(isr_val);
	}

	// ------------- DMA 1 channel 7 ----------------
	void DMA1_Channel7_IRQHandler(void)
	{
		uint32_t isr_val = DMA1->ISR;
		DMA1->IFCR = 0x0f << 6 * 4;
		if(s_Dma1Clients[6]) s_Dma1Clients[6]->DmaIsr(isr_val);
	}


//	// ------------- DMA 2 channel 1 ----------------
//	void DMA1_Channel1_IRQHandler(void)
//	{
//		uint32_t isr_val = DMA2->ISR;
//		DMA2->IFCR = 0x0f << 0;
//		if(s_Dma1Clients[0]) s_Dma1Clients[0]->DmaIsr(isr_val);
//	}
//
//	// ------------- DMA 2 channel 2 ----------------
//	void DMA1_Channel2_IRQHandler(void)
//	{
//		uint32_t isr_val = DMA2->ISR;
//		DMA2->IFCR = 0x0f << 1 * 4;
//		if(s_Dma1Clients[1]) s_Dma1Clients[1]->DmaIsr(isr_val);
//	}
//
//	// ------------- DMA 2 channel 3 ----------------
//	void DMA1_Channel3_IRQHandler(void)
//	{
//		uint32_t isr_val = DMA2->ISR;
//		DMA2->IFCR = 0x0f << 2 * 4;
//		if(s_Dma1Clients[2]) s_Dma1Clients[2]->DmaIsr(isr_val);
//	}
//
//	// ------------- DMA 2 channel 4 ----------------
//	void DMA1_Channel4_IRQHandler(void)
//	{
//		uint32_t isr_val = DMA2->ISR;
//		DMA2->IFCR = 0x0f << 3 * 4;
//		if(s_Dma1Clients[3]) s_Dma1Clients[3]->DmaIsr(isr_val);
//	}
//
//	// ------------- DMA 2 channel 5 ----------------
//	void DMA1_Channel5_IRQHandler(void)
//	{
//		uint32_t isr_val = DMA2->ISR;
//		DMA2->IFCR = 0x0f << 4 * 4;
//		if(s_Dma1Clients[4]) s_Dma1Clients[4]->DmaIsr(isr_val);
//	}


}
