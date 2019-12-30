#pragma once

#include "../Framework/Event/RestoreEvent.h"

#include "../Game/CollectData.h"
#include "../Game/StateManagerView.h"

namespace Game
{
	struct FID
	{
		enum
		{
			// interface has the responsibility to ensure playercount
			PLAYER_OVERFLOW = 2
		};
	};

	// collects all players and
	// assigns playerids
	class Collector
		:
		public StateManagerView
	{
	public:
		constexpr static bool CanRestore{ false };
		
		typedef CollectData Data;

		Collector(Data* const data);

		void Create() override;
		
		bool Process() override;
		bool Restore() override;
		bool Finish() override;
		void UpdateRep() override { } // should be virtual later
		
		// playerid is not player index
		// playerid is a random number
		bool CreatePlayer(const PlayerID player);
		bool RemovePlayer(const PlayerID player);
		bool ExistsPlayer(const PlayerID player) const;

		// -1 if not found
		int FindPlayerIndex(const PlayerID player) const;

	private:
		bool needsUpdate;
	};

	namespace CollectorAccess
	{
		bool ExistsPlayer(const PlayerID player);
		// -1 if not found
		int FindPlayerIndex(const PlayerID player);
	}

	struct CustomInterfaceJoinHandler
	{
	};
}
