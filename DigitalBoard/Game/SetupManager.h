#pragma once

#include "../Game/Data/SetupData.h"

#pragma pack(push, 1)

namespace Game
{
	namespace SetupManager
	{
		struct FID
		{
			enum
			{
			};
		};

		void Create();
		bool Process();
		bool Restore();

		void AdjustCommonDetectiveTickets(const CommonTickets);
		void AdjustCommonVillianTickets(const CommonTickets);
		void AdjustSpecialVillianTickets(const VillianTickets);

		namespace Preference
		{
			void SetVillian(const PlayerId player);
			void SetPlayerColor(
				const PlayerId player,
				const Color color);
			PlayerId GetColorPreference(const Color color);
		}

		bool Finish();
	}
}

#pragma pack(pop)
