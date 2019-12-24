#pragma once

#include "../Common/Common.h"

namespace Game
{
	typedef unsigned char PlayerId;

	struct CollectData
	{
		unsigned char playerCount;
		PlayerId playerIds[DEF_PLAYER_COUNT];
	};

	namespace CollectorAccess
	{
		// should not be accessed before
		// collect phase was created first time
		const CollectData* GetData();
	}
}
