#pragma once

#pragma pack(push, 1)

#include "../Common/Common.h"

#define DEVICE_TIMEOUT_COLOR (Device::RGB{ HIGH, LOW, HIGH })

namespace Device
{
	namespace StatusLED
	{
		void Initialize();
		void Unintialize();

		void Show(const RGB color);

		RGB GetCurrent();
	}

	// special delay / timeout
	// bound to StatusLED
	static void Timeout(const unsigned long time)
	{
		RGB old = StatusLED::GetCurrent();

		StatusLED::Show(DEVICE_TIMEOUT_COLOR);
		delay(time);
		StatusLED::Show(old);
	}

}

#pragma push(pop)
