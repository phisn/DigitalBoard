#include "GameManager.h"

#include "../Game/GameController.h"

namespace Extern
{
	extern Game::GameData* gameData;
}

namespace
{
	bool needsUpdate = true;

	FlashString turn_success_win_detective FPSTR("Game over, detective won");
	FlashString turn_success_win_villian FPSTR("Game over, villian won");
	FlashString turn_fail_activeplayer = FPSTR("PlayerId not active");
	FlashString turn_fail_playernotfound = FPSTR("PlayerId not found");
	FlashString turn_fail_invalidturn = FPSTR("Got invalid turn");
	FlashString turn_fail_ticketnotfound = FPSTR("Ticket not found");
	FlashString turn_fail_ticketnotavailable = FPSTR("Villian ticket not avaiable");

	Game::Player dynamicPlayer[COMMON_MAX_PLAYERCOUNT];
}

namespace Game
{
	namespace GameManager
	{
		int FindVillianIndex();

		bool ValidateTurn(
			const MapPosition source,
			const Turn turn);
		bool ValidateTurnBlack(
			const MapPosition source,
			const MapPosition target);
		bool ValidateTurnSpecific(
			const MapPosition source,
			const MapPosition target,
			const Station::Type station);
		bool RemovePlayerTicket(
			const PlayerData::Type type,
			PlayerState* const player, 
			const Ticket ticket);
		bool AddPlayerTicket(
			const PlayerData::Type type,
			PlayerState* const player,
			const Ticket ticket);
		bool ValidatePlayerTicketAvailability(
			const Player* const player,
			const MapPosition position);
		
		void Initialize()
		{
		}

		void Create()
		{
			DEBUG_MESSAGE("GameManager create");

			Extern::gameData->state.round = 0;
			Extern::gameData->state.activePlayer = SetupManager::GetData()->playerContext.villian;
			Extern::gameData->state.activePlayerIndex = 0;

			PathManager::FindRandomStartsResult randomStarts = PathManager::FindRandomStarts();

			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
			{
				PlayerState* const player = &Extern::gameData->player[i];

				// init ticketcount
				if (SetupManager::GetData()->playerContext.data[i].type == PlayerData::Type::Villian)
				{
					player->ticket = SetupManager::GetData()->settings.beginCommonVillian;
					player->villian.ticket = SetupManager::GetData()->settings.beginSpecialVillian;
				}
				else
				{
					player->ticket = SetupManager::GetData()->settings.beginCommonDetective;
				}

				// intit startposition
				player->position = randomStarts.starts[i];

				dynamicPlayer[i].state = player;
				dynamicPlayer[i].data = &SetupManager::GetData()->playerContext.data[i];
			}
		}

		bool Process()
		{
			if (needsUpdate)
			{
				needsUpdate = false;
				DEBUG_MESSAGE("SEND NEEDS UPDATE");

				return true;
			}

			return false;
		}

		bool Restore()
		{
			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				dynamicPlayer[i].state = &Extern::gameData->player[i];
				dynamicPlayer[i].data = &SetupManager::GetData()->playerContext.data[i];
			}

			return true;
		}

		TurnResult SelectNextPlayer();
		TurnResult MakeTurn(
			const PlayerId playerId, 
			const Turn turn)
		{
			if (playerId != Extern::gameData->state.activePlayer)
			{
				return TurnResult::ActivePlayer;
			}

			const int playerIndex = FindPlayerIndex(playerId);

			if (playerIndex == -1)
			{
				return TurnResult::PlayerNotFound;
			}

			PlayerState* const playerState = &Extern::gameData->player[playerIndex];
			const PlayerData* const playerData = &SetupManager::GetData()->playerContext.data[playerIndex];
			
			if (playerData->type == PlayerData::Type::Detective && (turn.ticket == Ticket::Black || turn.doubleTicket))
			{
				return TurnResult::TicketNotAvailable;
			}

			if (!ValidateTurn(playerState->position, turn))
			{
				return TurnResult::InvalidTurn;
			}

			if (turn.doubleTicket && playerState->villian.ticket.doubleTicketCount <= 0)
			{
				return TurnResult::TicketNotFound;
			}

			// also checks if playe has ticket
			if (!RemovePlayerTicket(
					playerData->type,
					playerState,
					turn.ticket))
			{
				return TurnResult::TicketNotFound;
			}

			if (turn.doubleTicket)
			{
				--playerState->villian.ticket.doubleTicketCount;
			}

			playerState->path[Extern::gameData->state.round] = playerState->position;
			playerState->position = turn.position;
			
			// needs update, data changed
			needsUpdate = true;

			if (playerData->type == PlayerData::Type::Detective)
			{
				PlayerState* const villian = &Extern::gameData->player[0];

				if (villian->position == playerState->position)
				{
					Extern::gameData->result.winner = playerData->type;
					Game::Controller::RequestFinish();

					return TurnResult::WinDetective;
				}

				AddPlayerTicket(
					PlayerData::Type::Villian,
					villian,
					turn.ticket);
			}
			else
			{
				Extern::gameData->state.villianTicketUse[Extern::gameData->state.round] = turn.ticket;

				// skip villian
				for (int i = 1; i < Collector::GetData()->playerCount; ++i)
					if (Extern::gameData->player[i].position == playerState->position)
					{
						Extern::gameData->result.winner = playerData->type;
						Game::Controller::RequestFinish();

						return TurnResult::WinDetective;
					}

				if (Extern::gameData->state.round == 24)
				{
					Extern::gameData->result.winner = playerData->type;
					Game::Controller::RequestFinish();

					return TurnResult::WinVillian;
				}

				if (IsShowVillianPositionRound())
				{
					Extern::gameData->state.villianLastPosition = playerState->position;
				}
			}

			if (!turn.doubleTicket)
			{
				TurnResult result = SelectNextPlayer();
				
				if (result != TurnResult::Success)
				{
					return result;
				}
			}

			return TurnResult::Success;
		}

		int FindPlayerIndex(const PlayerId id)
		{
			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
				if (SetupManager::GetData()->playerContext.data[i].player == id)
				{
					return i;
				}

			return -1;
		}

		int FindVillianIndex()
		{
			for (int i = 0; i < Collector::GetData()->playerCount; ++i)
				if (SetupManager::GetData()->playerContext.data[i].type == PlayerData::Type::Villian)
				{
					return i;
				}

			// unreachable
		}

		bool ValidateTurn(
			const MapPosition source,
			const Turn turn)
		{
			if (turn.ticket == Ticket::Black)
			{
				return ValidateTurnBlack(source, turn.position);
			}
			else
			{
				return ValidateTurnSpecific(
					source,
					turn.position,
					(Station::Type) turn.ticket);
			}
		}

		bool ValidateTurnBlack(
			const MapPosition source,
			const MapPosition target)
		{
			Station sourceStation = PathManager::GetStationType(source);
			PathManager::FindOptionsSpecificResult result;

			switch (sourceStation.type)
			{ // fall though all
			case Station::Underground:
				if (ValidateTurnSpecific(
						source,
						target,
						Station::Type::Underground))
				{
					return true;
				}

			case Station::Bus:
				if (ValidateTurnSpecific(
						source,
						target,
						Station::Type::Bus))
				{
					return true;
				}

			case Station::Taxi:
				if (ValidateTurnSpecific(
						source,
						target,
						Station::Type::Taxi))
				{
					return true;
				}

				if (ValidateTurnSpecific(
						source,
						target,
						Station::Type::Ferry))
				{
					return true;
				}

				break;
			}
			
			return false;
		}

		bool ValidateTurnSpecific(
			const MapPosition source,
			const MapPosition target,
			const Station::Type station)
		{
			const PathManager::FindOptionsSpecificResult result = PathManager::FindOptionsSpecific(
				source,
				station);

			for (int i = 0; i < result.stationCount; ++i)
				if (result.station[i] == target)
				{
					return true;
				}

			return false;
		}

		bool RemovePlayerTicket(
			const PlayerData::Type type,
			PlayerState* const player,
			const Ticket ticket)
		{
			// optimizing not possible, bitfields are used
			switch (ticket)
			{
			case Ticket::Yellow:
				if (player->ticket.yellowTicketCount > 0)
				{
					--player->ticket.yellowTicketCount;
					return true;
				}

				break;
			case Ticket::Green:
				if (player->ticket.greenTicketCount > 0)
				{
					--player->ticket.greenTicketCount;
					return true;
				}

				break;
			case Ticket::Red:
				if (player->ticket.redTicketCount > 0)
				{
					--player->ticket.redTicketCount;
					return true;
				}

				break;
			case Ticket::Black:
				if (type == PlayerData::Type::Detective)
				{
					break;
				}

				if (player->villian.ticket.blackTicketCount > 0)
				{
					--player->villian.ticket.blackTicketCount;
					return true;
				}

				break;
			case Ticket::Double:
				if (type == PlayerData::Type::Detective)
				{
					break;
				}

				if (player->villian.ticket.doubleTicketCount > 0)
				{
					--player->villian.ticket.doubleTicketCount;
					return true;
				}

				break;
			}

			return false;
		}

		bool AddPlayerTicket(
			const PlayerData::Type type,
			PlayerState* const player,
			const Ticket ticket)
		{
			// optimizing not possible, bitfields are used
			switch (ticket)
			{
			case Ticket::Yellow:
				++player->ticket.yellowTicketCount;

				break;
			case Ticket::Green:
				++player->ticket.greenTicketCount;

				break;
			case Ticket::Red:
				++player->ticket.redTicketCount;

				break;
			case Ticket::Black:
				if (type == PlayerData::Type::Detective)
				{
					return false;
				}

				++player->villian.ticket.blackTicketCount;

				break;
			case Ticket::Double:
				if (type == PlayerData::Type::Detective)
				{
					return false;
				}

				++player->villian.ticket.doubleTicketCount;

				break;
			}

			return true;
		}

		// check if player has tickets to move from station
		bool ValidatePlayerTicketAvailability(
			const Player* const player,
			const MapPosition position)
		{
			const Station station = PathManager::GetStationType(position);

			if (player->data->type == PlayerData::Type::Villian &&
				player->state->villian.ticket.blackTicketCount > 0)
			{
				return true;
			}

			// fall though all
			switch (station.type)
			{
			case Station::Underground:
				if (player->state->ticket.redTicketCount > 0)
				{
					return true;
				}

			case Station::Bus:
				if (player->state->ticket.greenTicketCount > 0)
				{
					return true;
				}

			case Station::Taxi:
				if (player->state->ticket.yellowTicketCount > 0)
				{
					return true;
				}

				break;
			}

			return false;
		}

		TurnResult SelectNextPlayer()
		{
			TurnResult result = TurnResult::Success;

			// check for finished round
			if (++Extern::gameData->state.activePlayerIndex >= Collector::GetData()->playerCount)
			{
				++Extern::gameData->state.round;
				Extern::gameData->state.activePlayerIndex = 0;
			}

			// select next playerId as active player
			Extern::gameData->state.activePlayer = SetupManager::GetData()
				->playerContext.data[Extern::gameData->state.activePlayerIndex].player;

			// check if player has valid movement
			const Player* const activePlayer = ReadPlayerByIndex(Extern::gameData->state.activePlayerIndex);

			if (!ValidatePlayerTicketAvailability(activePlayer, activePlayer->state->position))
			{
				if (activePlayer->data->type == PlayerData::Type::Villian)
				{
					Extern::gameData->result.winner = PlayerData::Type::Detective;
					result = TurnResult::WinDetective;
				}
				else
				{
					Extern::gameData->result.winner = PlayerData::Type::Villian;
					result = TurnResult::WinVillian;
				}

				Game::Controller::RequestFinish();
			}

			return result;
		}

		FlashString GetTurnFailReason(
			const TurnResult result)
		{
			switch (result)
			{
			case TurnResult::ActivePlayer:
				return turn_fail_activeplayer;

			case TurnResult::InvalidTurn:
				return turn_fail_invalidturn;

			case TurnResult::PlayerNotFound:
				return turn_fail_playernotfound;

			case TurnResult::TicketNotAvailable:
				return turn_fail_ticketnotavailable;

			case TurnResult::TicketNotFound:
				return turn_fail_ticketnotfound;
				
			default:
				return FPSTR("Turn successfull");
			}
		}

		const Player* ReadPlayer(const PlayerId id)
		{
			const int index = FindPlayerIndex(id);
			return index == -1 ? NULL : ReadPlayerByIndex(index);
		}

		const Player* ReadPlayerByIndex(const int index)
		{
			// no out of bounds check
			return &dynamicPlayer[index];
		}

		const GameData* GetData()
		{
			return Extern::gameData;
		}

		MapPosition GetLastVillianPosition()
		{
			return Extern::gameData->state.villianLastPosition;
		}
	}
}
