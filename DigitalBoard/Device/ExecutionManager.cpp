#include "ExecutionManager.h"

namespace
{
	TaskHandle_t loopThreadHandle;
}

namespace Device
{
	namespace ExecutionManager
	{
		void Initialize()
		{
			loopThreadHandle = xTaskGetCurrentTaskHandle();
		}

		void Uninitialize()
		{
		}

		bool IsLoopThread()
		{
			return xTaskGetCurrentTaskHandle() == loopThreadHandle;
		}
	}
}