#pragma once

#include "../Device/ExecutionManager.h"
#include "../Device/FailureHandler.h"
#include "../Device/MemoryManager.h"
#include "../Device/OutputManager.h"

#define DEVICE_EMESSAGE_ERROR "Error: "
#define DEVICE_EMESSAGE_ERROR_NONFATAL "Wrnng: "
#define DEVICE_EMESSAGE_ERROR_FATAL "Fatal: "

#define DEVICE_EMESSAGE_MODULE "Module: "
#define DEVICE_EMESSAGE_FID "FailureId: "

// minus null byte
#define DEVICE_EMESSAGE_ERROR_LEN (sizeof(DEVICE_EMESSAGE_ERROR) - 1)
#define DEVICE_EMESSAGE_MESSAGE_LEN (DEVICE_LCD_WIDTH * 2 - DEVICE_EMESSAGE_ERROR_LEN)

// device fault message has a special case. when the character after
// last character of the first line is a space, then this space is omited
// and therefore is not taken into account
#define DEVICE_FAULT_ISLASTSPACE(txt) ((txt)[DEVICE_LCD_WIDTH - DEVICE_EMESSAGE_ERROR_LEN] == ' ')
#define DEVICE_FAULT_GET_EXPECTED_LEN(txt) (DEVICE_FAULT_ISLASTSPACE(txt) \
	? DEVICE_EMESSAGE_MESSAGE_LEN + 1 : DEVICE_EMESSAGE_MESSAGE_LEN)

// usage: FlashString fault_xxx = DEVICE_FAULT_MESSAGE("xxx");
#define DEVICE_FAULT_MESSAGE(txt) FPSTR(txt); static_assert(sizeof(txt) - 1 == DEVICE_FAULT_GET_EXPECTED_LEN(txt), \
	"Fault message has to be have full length (see DEVICE_EMESSAGE_MESSAGE_LEN)")

#define DEVICE_MIN_REMAIN_MEMORY 50000
#define DEVICE_MAX_HEAP_FRAGMENT 50 // in percent

namespace Device
{
	enum class FaultModule
	{
		FaultHandler,
		NetworkManager,
		MapManager,

		BoardManager,
		Collector,
		SetupManager,
		GameManager,
		GameController,

		InterfaceManager,
		WebInterface,

		_Length
	};
	
	struct Fault
	{
		FaultModule module;
		FailureId id;
		FlashString text;
	};

	namespace FaultHandler
	{
		struct FID
		{
			// FailureIds over _Length represent faultmodules index 
			// by i + _Length -> first is at _Length

			enum
			{
				INC_NULL = 2, // interfacenotifiercallback
				LOW_MEMORY,
				HEAP_FRAG,
				_Length // for fault module conversion in HandleCommonFault
			};
		};

		typedef void (*InterfaceNotifierCallback)(const Fault);

		// called in interfacemanager
		void RegisterFaultInterfaceNotifier(
			const InterfaceNotifierCallback callback);

		void Initialize();
		void Unintialize();

		void Report(); // -> eeprom

		// faults can be non-fatal to continue normally
		void Handle(const Fault fault, const bool fatal);
		void ValidateDeviceState();

		FlashString GetModuleName(const FaultModule module);
	}
}
