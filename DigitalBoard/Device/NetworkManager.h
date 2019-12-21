#pragma once

#include "../Device/FaultHandler.h"
#include "../Device/OutputManager.h"

#define DEVICE_NET_SSID "Digital Detectives"
#define DEVICE_NET_PASS "scotyard"

#define DEVICE_NET_LOCAL_IP { 192, 168, 0, 1 }
#define DEVICE_NET_SUBNET_MASK { 255, 255, 255, 0 }; 

#define DEVICE_NET_CHANNEL 1
#define DEVICE_NET_SSID_HIDDEN 0
#define DEVICE_NET_MAX_CONN 6

namespace Device
{
	namespace NetworkManager
	{
		typedef uint8_t StationCount;

		struct FID
		{
			enum
			{
				SOFT_AP_MODE = 2,
				SOFT_AP_CONFIG,
				SOFT_AP_CREATE
			};
		};

		void Initialize();
		void Uninitialize();

		StationCount GetStationCount();

		void Process();
	}
}
