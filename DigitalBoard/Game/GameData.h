#pragma once

#include "../../Game/Data/PlayerData.h"

namespace Game
{
	struct GameData
	{
		PlayerState player[COMMON_MAX_PLAYERCOUNT];

		struct GameState
		{
			unsigned char round; // max 24

			// index in playersetup order
			unsigned char activePlayerIndex;
			PlayerId activePlayer;

			MapPosition villianLastPosition;
			Game::Ticket villianTicketUse[24];
		} state;

		struct
		{
			Game::PlayerData::Type winner;

		} result;
	};

	namespace GameManager
	{
		const GameData* GetData();

		inline bool IsShowVillianPositionRound()
		{
			return GetData()->state.round % 5 == 3;
		}

		inline int GetShowVillianPositionRound()
		{
			return 5 - (GetData()->state.round + 2) % 5;
		}
	}
}
