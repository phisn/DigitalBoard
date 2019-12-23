#include "SetupManager.h"
#include "../Game/GameController.h"

namespace Extern
{
	extern Game::SetupData* setupData;
}

namespace
{
	bool needsUpdate = true;
	Game::PlayerId colorPreferences[COMMON_MAX_PLAYERCOUNT] = { };
}

namespace Game
{
	namespace SetupManager
	{
		int FindPlayerIndex(const PlayerId player);

		void CreateTicketCount();
		void CreatePlayerContext();

		void Create()
		{
			DEBUG_MESSAGE("SetupManager Create");

			CreateTicketCount();
			CreatePlayerContext();
		}

		void CreateTicketCount()
		{
			Extern::setupData->settings.beginCommonDetective = DefaultTickets::beginCommonDetective;
			Extern::setupData->settings.beginCommonVillian = DefaultTickets::beginCommonVillian;
			Extern::setupData->settings.beginSpecialVillian = DefaultTickets::beginSpecialVillian;

			// for every detective
			Extern::setupData->settings.beginSpecialVillian.blackTicketCount = Collector::GetData()->playerCount - 1;
		}

		void CreatePlayerContext()
		{
			// shuffle players for random order
			PlayerId playerOrder[COMMON_MAX_PLAYERCOUNT];

			memcpy(playerOrder,
				Collector::GetData()->playerIds,
				Collector::GetData()->playerCount * sizeof(PlayerId)
			);

			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
			{
				int randomIndex = random(Collector::GetData()->playerCount);

				// swap
				PlayerId temp = playerOrder[randomIndex];
				playerOrder[randomIndex] = playerOrder[i];
				playerOrder[i] = temp;
			}

			// create playerData
			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
			{
				Extern::setupData->playerContext.data[i].player = playerOrder[i];
				Extern::setupData->playerContext.data[i].type = i == 0
					? PlayerData::Type::Villian
					: PlayerData::Type::Detective;

				// change later by preference
				Extern::setupData->playerContext.data[i].color = (Color) i;
			}

			// first is villian
			Extern::setupData->playerContext.villian = playerOrder[0];
		}

		bool Process()
		{
			// TODO: change late, currently in debug
			DEBUG_MESSAGE("Setup auto request finish");
			Game::Controller::RequestFinish();

			return false;
		}

		bool Restore()
		{
			// TODO: Implement check
			return true;
		}

		void AdjustCommonDetectiveTickets(const CommonTickets ticketCount)
		{
			Extern::setupData->settings.beginCommonDetective = ticketCount;
			needsUpdate = true;
		}

		void AdjustCommonVillianTickets(const CommonTickets ticketCount)
		{
			Extern::setupData->settings.beginCommonVillian = ticketCount;
			needsUpdate = true;
		}

		void AdjustSpecialVillianTickets(const VillianTickets ticketCount)
		{
			Extern::setupData->settings.beginSpecialVillian = ticketCount;
			needsUpdate = true;
		}

		namespace Preference
		{
			void SetVillian(const PlayerId player)
			{
				const int index = FindPlayerIndex(player);

				if (index != -1)
				{
					const PlayerId temp = Extern::setupData->playerContext.data[0].player;
					Extern::setupData->playerContext.data[0].player = Extern::setupData->playerContext.data[index].player;
					Extern::setupData->playerContext.data[index].player = temp;
				}
				else
				{
					// ...
				}
			}

			void SetPlayerColor(
				const PlayerId player,
				const Color color)
			{
				const int sourceIndex = FindPlayerIndex(player);
				const PlayerId targetPlayer = GetColorPreference(color);

				if (sourceIndex == -1)
				{
					// ...
				}

				if (targetPlayer == NULL)
				{
					Extern::setupData->playerContext.data[sourceIndex].color = color;
				}
				else
				{
					const int targetIndex = FindPlayerIndex(player);

					const PlayerId temp = Extern::setupData->playerContext.data[targetIndex].player;
					Extern::setupData->playerContext.data[targetIndex].player = Extern::setupData->playerContext.data[sourceIndex].player;
					Extern::setupData->playerContext.data[sourceIndex].player = temp;
				}
			}

			PlayerId GetColorPreference(const Color color)
			{
				for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
					if (Extern::setupData->playerContext.data[i].color == color)
					{
						return Extern::setupData->playerContext.data[i].player;
					}

				return NULL;
			}
		}

		int FindPlayerIndex(const PlayerId player)
		{
			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
				if (Extern::setupData->playerContext.data[i].player == player)
				{
					return i;
				}

			return -1;
		}

		const Game::SetupData* GetData()
		{
			return Extern::setupData;
		}

		bool Finish()
		{
			CreatePlayerContext();

			return true;
		}
	}
}
