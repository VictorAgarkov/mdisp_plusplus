#ifndef MDISP_DEVICE_H_INCLUDED
#define MDISP_DEVICE_H_INCLUDED

	#include <stdint.h>

	class MdispDevice
	{
		public:

			virtual void SendBuffer(const uint8_t *buf, int buf_size, int data_cmd) = 0;
			virtual void StartBuffer(const uint8_t *buf, int buf_size, int data_cmd) = 0;
			virtual bool IsBusy(void) = 0;
	};

#endif /* MDISP_DEVICE_H_INCLUDED */
