#pragma once

#include "../Device/MemoryManager.h"

#include "../Game/BoardManager.h"
#include "../Game/Collector.h"
#include "../Game/Data/GameData.h"
#include "../Game/PathManager.h"
#include "../Game/SetupManager.h"

#pragma pack(push, 1)

namespace Game
{
	struct Turn
	{
		Ticket ticket;
		MapPosition position;
		bool doubleTicket; 
	};

	namespace GameManager
	{
		struct FID
		{
			enum
			{
				SECTOR_CORRUPTED = 2
			};
		};

		void Create();
		bool Process();
		bool Restore();

		enum class TurnResult
		{
			Success = 0,

			WinDetective,
			WinVillian,

			ActivePlayer,
			PlayerNotFound,
			InvalidTurn,
			TicketNotFound,
			TicketNotAvailable,

			_Length
		};
		
		TurnResult MakeTurn(
			const PlayerId playerId,
			const Turn turn);

		FlashString GetTurnFailReason(
			const TurnResult result);

		// avoid multiple calls!
		const Player* ReadPlayer(const PlayerId id);
		const Player* ReadPlayerByIndex(const int index);

		// can fail (-1)
		int FindPlayerIndex(const PlayerId id);

		MapPosition GetLastVillianPosition();
	}
}

#pragma pack(pop)
