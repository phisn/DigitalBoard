#pragma once

#include "../Common/Common.h"

namespace Game
{
	typedef unsigned char PlayerId;

	struct CollectData
	{
		unsigned char playerCount;
		PlayerId playerIds[COMMON_MAX_PLAYERCOUNT];
	};

	namespace CollectorAccess
	{
		// should not be accessed before
		// collect phase was created first time
		const CollectData* GetData();
	}
}
