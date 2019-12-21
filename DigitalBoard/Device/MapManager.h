#pragma once

#include "../Device/FaultHandler.h"
#include "../Device/OutputManager.h"

namespace Device
{
	namespace MapManager
	{
		void Initialize();
		void Unintialize();

		Game::MapPosition Translate(const Game::MapPosition);
	}
}
