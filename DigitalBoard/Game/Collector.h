#pragma once

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
		constexpr static bool CanJoin{ true };
		constexpr static bool CanRestore{ false };
		
		typedef CollectData Data;

		Collector(Data* const data);

		void Create() override;
		
		bool Process() override;
		bool Finish() override; 

		void UpdateRep() override;
		
		// playerid is not player index
		// playerid is a random number

		PlayerId CreatePlayer();
		bool RemovePlayer(const PlayerId player);
		bool ExistsPlayer(const PlayerId player) const;

		// -1 if not found
		int FindPlayerIndex(const PlayerId player) const;

	private:
		bool needsUpdate;
	};

	namespace CollectorAccess
	{
		bool ExistsPlayer(const PlayerId player);
		// -1 if not found
		int FindPlayerIndex(const PlayerId player);
	}
}
