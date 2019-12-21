#include "FaultHandler.h"
#include "../Device/NetworkManager.h"

// minus null byte
#define DEVICE_EMESSAGE_ERROR_LEN (sizeof(DEVICE_EMESSAGE_ERROR) - 1)
#define DEVICE_EMESSAGE_MODULE_LEN (sizeof(DEVICE_EMESSAGE_MODULE) - 1)
#define DEVICE_EMESSAGE_FID_LEN (sizeof(DEVICE_EMESSAGE_FID) - 1)

#define DEVICE_EMESSAGE_MESSAGE_LEN_FL DEVICE_LCD_WIDTH - DEVICE_EMESSAGE_ERROR_LEN
#define DEVICE_EMESSAGE_MODULE_NAME_LEN DEVICE_LCD_WIDTH - DEVICE_EMESSAGE_MODULE_LEN
#define DEVICE_EMESSAGE_FID_NUMBER_LEN DEVICE_LCD_WIDTH - DEVICE_EMESSAGE_FID_LEN

namespace
{
	FlashString emessageError = FPSTR(DEVICE_EMESSAGE_ERROR);
	FlashString emessageErrorNonFatal FPSTR(DEVICE_EMESSAGE_ERROR_NONFATAL);
	FlashString emessageModule = FPSTR(DEVICE_EMESSAGE_MODULE);
	FlashString emessageFID = FPSTR(DEVICE_EMESSAGE_FID);

	FlashString emessageModuleNames[] PROGMEM = // 11 + \0
	{
		FPSTR("FaultHandler\0"), // device
		FPSTR("NetworkMng  \0"),
		FPSTR("MapManager  \0"),

		FPSTR("BoardManager\0"), // game
		FPSTR("Collector   \0"),
		FPSTR("SetupManager\0"),
		FPSTR("GameManager \0"),
		FPSTR("GameCntrller\0"),

		FPSTR("InterfaceMng\0"), // communication
		FPSTR("WebInterface\0")
	};

	static_assert(sizeof(emessageModuleNames) / sizeof(*emessageModuleNames) == (int) Device::FaultModule::_Length,
		"Faultmodule length changed, adjust emessageModuleNames module names");

	FlashString fault_intmg_null = DEVICE_FAULT_MESSAGE("IntMg callback was null          ");
	FlashString fault_low_memory = DEVICE_FAULT_MESSAGE("Device has low memory            ");
	FlashString fault_heap_frag = DEVICE_FAULT_MESSAGE("Device heap is fragmented        ");

	namespace ScheduledFault
	{
		bool hasData = false;
		Device::Fault fault;
		bool isFatal;
	}
}

// prevent accidential change of nonfatal
static_assert(sizeof(DEVICE_EMESSAGE_ERROR) == sizeof(DEVICE_EMESSAGE_ERROR_NONFATAL),
	"Replacement 'non-fatal' must have the same size as the normal error");

// Force adjustments on lcd change
static_assert(DEVICE_LCD_WIDTH == 20,
	"LCD width changed, FaultHandler needs adjustments");

// ################### <- 20
// ########            <- 9
//         ########### <- 11 
// Module: InterfaceMg   

/*

ErrorMessage:
	vvvvvvvvvvvvvvvvvvvv
   >Error: ############# <- 13 ch error message
   >#################### <- + 20 ch -> 33 ch
   >Module: ############ <- 12 ch module name
   >FailureId: ######### <- 9 digits for failure id
    ^^^^^^^^^^^^^^^^^^^^
*/

namespace
{
	Device::FaultHandler::InterfaceNotifierCallback interfaceNotifierCallback = NULL;
}

namespace Device
{
	namespace FaultHandler
	{
		void DisplayFaultMessage(const Fault fault, const bool fatal);
		void HandleModuleFault(const Fault fault);
		void HandleCommonFault(const Fault fault);
		void HandleNetworkFault(const Fault fault);

		void RegisterFaultInterfaceNotifier(
			const InterfaceNotifierCallback callback)
		{
			interfaceNotifierCallback = callback;
		}

		void Initialize()
		{
		}

		void Unintialize()
		{
		}

		void Handle(const Fault fault, const bool fatal)
		{
			// schedule fault if current thread is not the loop thread
			if (!ExecutionManager::IsLoopThread())
			{
				ScheduledFault::hasData = true;
				ScheduledFault::fault = fault;
				ScheduledFault::isFatal = fatal;

				return;
			}

			/*
			if (interfaceNotifierCallback == NULL)
			{
				Handle(
				{
					FaultModule::FaultHandler,
					(FailureId) FID::INC_NULL,
					fault_intmg_null
				});
			}
			*/

			DEBUG_MESSAGE("Inside FaultHandler (text / module / module name / id / isFatal)");
			DEBUG_MESSAGE(fault.text);
			DEBUG_MESSAGE((int) fault.module);
			DEBUG_MESSAGE(emessageModuleNames[(int) fault.module]);
			DEBUG_MESSAGE((int) fault.id);
			DEBUG_MESSAGE((int) fatal);

			DisplayFaultMessage(fault, fatal);

			if (fatal)
			{
				// interfaceNotifierCallback(fault);
				HandleModuleFault(fault);
			}
			else
			{
				delay(5000);
			}
		}

		void HandleModuleFault(const Fault fault)
		{
			switch (fault.module)
			{
			case FaultModule::NetworkManager:
				HandleNetworkFault(fault);

				break;
			default:
				HandleCommonFault(fault);

				break;
			}
		}

		void HandleNetworkFault(const Fault fault)
		{
			static bool alreadyTried = false;

			if (alreadyTried)
			{
				HandleCommonFault(fault);
			}
			else // retry
			{
				alreadyTried = true;

				Timeout(30000); // 30 sec
				NetworkManager::Initialize();

				// reset for further retries
				alreadyTried = false;
			}
		}

		void HandleCommonFault(const Fault fault)
		{
			FailureHandler::Handle(
				FailureModule::FaultHandler,
				FID::_Length + (int)fault.module
			);
		}

		void ValidateDeviceState()
		{
			// ex. check if device memory overflows

			if (ESP.getFreeHeap() < DEVICE_MIN_REMAIN_MEMORY) // || 
//				ESP.getFreeContStack() < DEVICE_MIN_REMAIN_MEMORY)
			{
				FaultHandler::Handle(
				{
					FaultModule::FaultHandler,
					(FailureId) FID::LOW_MEMORY,
					fault_low_memory
				}, true);
			}

			if (ScheduledFault::hasData)
			{
				ScheduledFault::hasData = false;

				FaultHandler::Handle(
					ScheduledFault::fault,
					ScheduledFault::isFatal);

				return;
			}

			/*if (ESP.getHeapFragmentation() > DEVICE_MAX_HEAP_FRAGMENT)
			{
				FaultHandler::Handle(
				{
					FaultModule::FaultHandler,
					(FailureId) FID::HEAP_FRAG,
					fault_heap_frag
				}, true);
			}*/

			/* use to determine eeprom max size?
				ESP.getFreeSketchSpace();
			*/
		}

		void DisplayFaultMessage(const Fault fault, const bool fatal)
		{
			OutputManager::Lcd::Clear();
			char buffer[DEVICE_LCD_WIDTH + 1] = { };

			// print error message
			memcpy_P(
				buffer,  
				fatal ? emessageError : emessageErrorNonFatal,
				DEVICE_EMESSAGE_ERROR_LEN);
			memcpy_P(
				buffer + DEVICE_EMESSAGE_ERROR_LEN, 
				fault.text, 
				DEVICE_EMESSAGE_MESSAGE_LEN_FL);

			OutputManager::Lcd::DisplayLineType(
				0,
				buffer);

			memset(buffer, 0, DEVICE_LCD_WIDTH + 1);

			// print remaining error message
			// ignore space on next line
			const bool offset = pgm_read_byte((const char*) fault.text + DEVICE_EMESSAGE_MESSAGE_LEN_FL) == ' ';
			memcpy_P(
				buffer, 
				(const char*) fault.text + DEVICE_EMESSAGE_MESSAGE_LEN_FL + offset,
				DEVICE_LCD_WIDTH - offset);

			OutputManager::Lcd::DisplayLineType(
				1,
				buffer);

			// memset(buffer, 0, DEVICE_LCD_WIDTH + 1);

			// print module name
			memcpy_P(buffer, emessageModule, DEVICE_EMESSAGE_MODULE_LEN);
			memcpy_P(
				buffer + DEVICE_EMESSAGE_MODULE_LEN, 
				GetModuleName(fault.module), 
				DEVICE_EMESSAGE_MODULE_NAME_LEN);
			
			OutputManager::Lcd::DisplayLineType(
				2,
				buffer);

			// print failure id
			memcpy_P(buffer, emessageFID, DEVICE_EMESSAGE_FID_LEN);
			sprintf_P(
				buffer + DEVICE_EMESSAGE_FID_LEN,
				PSTR("%09d"),
				fault.id);
			
			OutputManager::Lcd::DisplayLineType(
				3,
				buffer);
		}

		FlashString GetModuleName(const FaultModule module)
		{
			return emessageModuleNames[(int) module];
		}
	}
}

