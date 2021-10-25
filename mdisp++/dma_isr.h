#ifndef DMA_ISR_H_INCLUDED
#define DMA_ISR_H_INCLUDED

	#include <stdint.h>

	class DmaIsrClient
	{
		public:
			virtual void DmaIsr(uint32_t dma_isr_value) = 0; // dma_isr_value - value of 'Interrupt status register'
	};

	void SetDmaIsrClent(int dma, int channel, DmaIsrClient *client);



#endif /* DMA_ISR_H_INCLUDED */
