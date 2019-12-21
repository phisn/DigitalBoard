#pragma once

#include "../Device/FailureHandler.h"

namespace Extern
{
	extern const unsigned long SectorFaultSize;
	extern const unsigned long SectorGameSize;
	extern const unsigned long SectorValidationSize;
}

namespace Device
{
	enum class MemorySector : unsigned char
	{
		Validation,
		Fault,
		Game,

		_Length
	};

	typedef unsigned short ValidationSector;

	namespace MemoryManager
	{
		struct FID
		{
			enum
			{
				HEAP_OVERFLOW = 2,

				INVALID_SECTOR,

				EEPROM_OVERFLOW,
				EEPROM_CORRUPTED,
				EEPROM_WRITE_CLEAN,
				EEPROM_WRITE // has to be last | + MemorySector::_Length
			};
		};

		void Initialize();
		void Unintialize();

		void ReadSector(
			const MemorySector zone,
			char* const buffer);
		void WriteSector(
			const MemorySector zone,
			char* const buffer);

		void CleanEEPROM();

		void* AllocateDynamic(const unsigned long length);
		void DeallocateDynamic(void* memory);
	}
}
