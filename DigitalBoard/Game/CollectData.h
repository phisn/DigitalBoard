#pragma once

#include "../Common/Common.h"
#include "../Communication/Interface.h"

namespace Game
{
	typedef Communication::InterfaceID PlayerID;

	struct CollectData
	{
		unsigned char playerCount;
		PlayerID playerIds[DEF_PLAYER_COUNT];
	};

	namespace CollectorAccess
	{
		// should not be accessed before
		// collect phase was created first time
		const CollectData* GetData();
	}
}
