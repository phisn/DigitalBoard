#pragma once

#include "../Common/Common.h"

namespace Device
{
	namespace ExecutionManager
	{
		void Initialize();
		void Uninitialize();

		bool IsLoopThread();
	}
}
