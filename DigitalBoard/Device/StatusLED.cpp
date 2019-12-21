#include "StatusLED.h"

#include "../Device/DevicePins.h"

namespace
{
	Device::RGB current = Device::RGB{ LOW, LOW, LOW };
}

namespace Device
{
	namespace StatusLED
	{
		void Initialize()
		{
			pinMode(DEVICE_PIN_STATUS_LED_R, OUTPUT);
			pinMode(DEVICE_PIN_STATUS_LED_G, OUTPUT);
			pinMode(DEVICE_PIN_STATUS_LED_B, OUTPUT);

			Show(RGB{ HIGH, HIGH, HIGH });
		}

		void Unintialize()
		{
		}

		void Show(const RGB color)
		{
			current = color;

			digitalWrite(DEVICE_PIN_STATUS_LED_R, color.r);
			digitalWrite(DEVICE_PIN_STATUS_LED_G, color.g);
			digitalWrite(DEVICE_PIN_STATUS_LED_B, color.b);
		}

		RGB GetCurrent()
		{
			return current;
		}
	}
}
