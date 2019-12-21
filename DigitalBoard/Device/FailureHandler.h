#pragma once

#include "../Device/StatusLED.h"

#define DEVICE_FID_OVERFLOW 16

// unkown module color is also used as timeout color in statusled (static light)
#define DEVICE_UNKOWN_COLOR  (Device::RGB{ HIGH, LOW, HIGH })		// PURPLE
#define DEVICE_LOOP_COLOR (Device::RGB{ HIGH, HIGH, LOW })			// YELLOW
#define DEVICE_IFID_COLOR (Device::RGB{ LOW, HIGH, HIGH })			// 

#define DEVICE_MEMORY_MANAGER_COLOR (Device::RGB{ HIGH, LOW, LOW })	// RED
#define DEVICE_OUTPUT_MANAGER_COLOR (Device::RGB{ LOW, HIGH, LOW })	// GREEN
#define DEVICE_FAULT_MANAGER_COLOR (Device::RGB{ LOW, LOW, HIGH })	// BLUE

namespace Device
{
	typedef unsigned int FailureId;

	enum class FailureModule : unsigned char
	{
		MemoryManager,
		OutputManager,
		FaultHandler
	};

	namespace FailureHandler
	{
		void Initialize();
		void Unintialize();

		void Display(
			const RGB color,
			const FailureId id);
		void Handle(
			const FailureModule module,
			const FailureId id);
	}
}
