#pragma once

#include "../../Game/Data/CollectData.h"
#include "../../Game/Data/PlayerData.h"
#include "../../Game/Data/SettingsData.h"

namespace Game
{
	struct SetupData
	{
		Settings settings;

		struct PlayerContext
		{
			// ordered
			PlayerData data[COMMON_MAX_PLAYERCOUNT];
			PlayerId villian;

		} playerContext;
	};

	namespace SetupManager
	{
		const SetupData* GetData();
	}
}
