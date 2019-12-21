#include "FailureHandler.h"

// allow to access fid
#include "../Device/FaultHandler.h"
#include "../Device/MemoryManager.h"
#include "../Device/OutputManager.h"

namespace Device
{
	namespace FailureHandler
	{
		void HandleFatalError();

		void HandleMemoryFailure(const FailureId id);
		void HandleOutputFailure(const FailureId id);
		void HandleFaultFailure(const FailureId id);

		void DisplayFailureStatus(
			const RGB color,
			const FailureId id);
		void DisplayFailureStatusLoop(
			const RGB color,
			const FailureId id);

		void Initialize()
		{
		}

		void Unintialize()
		{
		}

		void Display(
			const RGB color, 
			const FailureId id)
		{
			DisplayFailureStatus(color, id);
		}

		void Handle(
			const FailureModule module,
			const FailureId id)
		{
			DEBUG_MESSAGE("Inside FailureHandler");
			DEBUG_MESSAGE((int) module);
			DEBUG_MESSAGE((int) id);

			switch (module)
			{
			case FailureModule::MemoryManager:
				HandleMemoryFailure(id);

				break;
			case FailureModule::OutputManager:
				HandleOutputFailure(id);

				break;
			case FailureModule::FaultHandler:
				HandleFaultFailure(id);

				break;
			default:
				DisplayFailureStatus(DEVICE_UNKOWN_COLOR, id);
				HandleFatalError();

				break;
			}
		}

		void HandleMemoryFailure(const FailureId id)
		{
			if (id == MemoryManager::FID::EEPROM_OVERFLOW ||
				id == MemoryManager::FID::INVALID_SECTOR)
			{
				DisplayFailureStatusLoop(DEVICE_MEMORY_MANAGER_COLOR, id);
			}

			DisplayFailureStatus(DEVICE_MEMORY_MANAGER_COLOR, id);

			if (id == MemoryManager::FID::EEPROM_CORRUPTED)
			{
				MemoryManager::CleanEEPROM();
			}

			// Cant be handled
			//  EEPROM_WRITE_CLEAN
			//  EEPROM_WRITE
			//  EEPROM_HEAP_OVERFLOW

			HandleFatalError();
		}

		void HandleOutputFailure(const FailureId id)
		{
			DisplayFailureStatus(DEVICE_OUTPUT_MANAGER_COLOR, id);
			HandleFatalError();
		}

		void HandleFaultFailure(const FailureId id)
		{
			if (id != 0)
			{
				DisplayFailureStatus(DEVICE_FAULT_MANAGER_COLOR, id);
			}

			HandleFatalError();
		}

		void HandleFatalError()
		{
			ESP.restart();
		}

		void DisplayFailureStatus(
			const RGB color,
			const FailureId id)
		{
			if (id > DEVICE_FID_OVERFLOW)
			{
				// handle bad FailureId
				StatusLED::Show(DEVICE_IFID_COLOR);
				delay(5000);
				StatusLED::Show(color);
				delay(5000);
				
				return;
			}

			for (int i = 0; i < 3; ++i)
			{
				for (FailureId i = 0; i < id; ++i)
				{
					StatusLED::Show(color);
					delay(400);
					StatusLED::Show(RGB{ LOW, LOW, LOW });
					delay(400);
				}

				delay(2000);
			}
		}

		void DisplayFailureStatusLoop(const RGB color, const FailureId id)
		{
			while (true)
			{
				DisplayFailureStatus(color, id);

				StatusLED::Show(DEVICE_LOOP_COLOR);
				delay(1000);
			}
		}
	}
}
