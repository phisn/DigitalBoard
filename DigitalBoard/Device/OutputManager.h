#pragma once

#include "../Device/FailureHandler.h"
#include "../Device/MemoryManager.h"

#define DEVICE_LCD_ADDRESS 0x27
#define DEVICE_LCD_WIDTH 20
#define DEVICE_LCD_HEIGHT 4
#define DEVICE_FASTLED_MAP_LEDCOUNT 199
#define DEVICE_FASTLED_PATH_LEDCOUNT 24

#define DEVICE_SELECTION_SIZE (DEVICE_LCD_WIDTH - 3)

#define DEVICE_LCD_MESSAGE(txt) FPSTR(txt); static_assert(sizeof(txt) - 1 == DEVICE_LCD_WIDTH, \
	"LCD message has to have the length of LCD width (see DEVICE_LCD_WIDTH)")
#define DEVICE_SELECTION(txt) FPSTR(txt); static_assert(sizeof(txt) - 1 == DEVICE_SELECTION_SIZE), \
	"Selection has to have the length of LCD width minus 2"

namespace Device
{
	namespace OutputManager
	{
		struct FID
		{
			enum
			{
				LCD_DISPLAY = 3
			};
		};

		void Initialize();
		void Uninitialize();

		namespace Interact
		{
			FlashString GetCommonYesNo();
			FlashString GetCommonPressExit();

			int Select(FlashString* const selection, const int size);

			enum Choice
			{
				Empty	= 0b00000000,
				Left	= 0b00000001,
				Enter	= 0b00000010,
				Right	= 0b00000100,

				Yes = Left,
				No = Right,
				Up = Left,
				Down = Right
			};

			Choice GetChoice();
			Choice ForceGetChoice();

			void AwaitEnter();
		}

		namespace Lcd
		{
			LiquidCrystal_I2C* _GetDisplay();

			template <typename T>
			void DisplayLineType(
				const unsigned char row,
				const T value)
			{
				_GetDisplay()->setCursor(0, row);
				_GetDisplay()->print(value);
			}

			void DisplayLineType(
				const unsigned char row,
				const char* value,
				const unsigned int size);

			void Clear();
		}

		namespace PlayerMapLed
		{
			void Clear();
			void Show(const int pin, CRGB color);

			void Update();
			CRGB* _GetData();
		}

		namespace VillianPathLed
		{
			void Clear();
			void Show(const int pin, CRGB color);

			void Update();
			CRGB* _GetData();
		}

		// external
		void _InitializeFastLed();
	}
}
