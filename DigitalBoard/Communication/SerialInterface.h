#pragma once

#include "../Communication/Interface.h"
#include "../Game/GameController.h"

#define COM_SERIAL_TURN_SIZE_MIN 3
#define COM_SERIAL_TURN_SIZE_MAX 6

namespace Communication
{
	class SerialInterface
		:
		public Interface
	{
		static char playerCharacterCounter;
	public:
		enum CollectCommand
		{
			Join = 'j',
			Start = 's'
		};

		static void UpdateCollect();

		enum Command
		{
			Turn = 't'
		};

		enum Ticket
		{
			Yellow = 'y',
			Green = 'g',
			Red = 'r',
			Black = 'b',
			Double = 'd'
		};

		void initialize(const Game::PlayerId id) override
		{
			playerCharacter = playerCharacterCounter++;
			playerId = id;

			Serial.print(F("Created interface as "));
			Serial.print(id);
			Serial.print(F(" with '"));
			Serial.print(playerCharacter);
			Serial.println('\'');
		}

		void notifyFault(const Device::Fault fault) override
		{
			Serial.println(F("Fault: "));

			Serial.print(F("Message: "));
			Serial.println(fault.text);

			Serial.print(F("Module: "));
			Serial.println(
				Device::FaultHandler::GetModuleName(fault.module)
			);

			Serial.print(F("Id: "));
			Serial.println(fault.id, 16);
		}

		bool process() override
		{
			if (Game::Controller::GetState() != Game::GameState::Running)
			{
				return false;
			}

			if (Serial.available() > 0)
			{
				delay(200); // wait for data
			}

			const int size = Serial.available();

			if (size < 2)
			{
				goto CLEAR_BUFFER;
			}

			{
				const char inputCharacter = Serial.peek();
				if (inputCharacter != playerCharacter)
				{
					if (inputCharacter >= playerCharacterCounter ||
						inputCharacter < 'a')
					{
						DEBUG_MESSAGE("Got invalid character: ");
						DEBUG_MESSAGE(inputCharacter);

						goto CLEAR_BUFFER;
					}

					DEBUG_MESSAGE("Ignoring character: ");
					DEBUG_MESSAGE(inputCharacter);

					return false;
				}
				else
				{
					DEBUG_MESSAGE("Got valid character: (character, length)");
					DEBUG_MESSAGE(inputCharacter);
					DEBUG_MESSAGE(size);

					Serial.read();
				}
			}

			switch (Serial.read())
			{
			case Turn:
				if (size >= COM_SERIAL_TURN_SIZE_MIN ||
					size <= COM_SERIAL_TURN_SIZE_MAX)
				{
					processTurn();
				}
				else
				{
					Serial.println(F("Got invalid length for command 'Turn'"));
				}

				break;
			default:
				Serial.println(F("Invalid command"));

				break;
			}

		CLEAR_BUFFER:
			while (Serial.available())
				Serial.read();

			return false;
		}

		void update() override
		{
			switch (Game::Controller::GetState())
			{
			case Game::GameState::Collect:
				updateCollect();

				break;
			case Game::GameState::Running:
				updateRunning();

				break;
			case Game::GameState::Setup:
				updateSetup();

				break;
			}
		}

		const Game::PlayerId getPlayerId() const
		{
			return playerId;
		}

	private:
		char playerCharacter;
		Game::PlayerId playerId;

		void processTurn()
		{
			Game::Turn turn;

			turn.ticket = parseTicket();

			if (turn.ticket == Game::Ticket::Double)
			{
				turn.doubleTicket = true;
				turn.ticket = parseTicket();
			}

			if (turn.ticket == Game::Ticket::_Invalid)
			{
				Serial.println(F("Got invalid ticket"));
				return;
			}

			if (Serial.available() <= 0)
			{
				Serial.println(F("Got empty target"));
			}

			turn.position = parseTarget();

			if (turn.position == 0)
			{
				Serial.println(F("Got invalid target"));
				return;
			}

			const Game::GameManager::TurnResult result = Game::GameManager::MakeTurn(playerId, turn);

			if (result == Game::GameManager::TurnResult::Success)
			{
				Serial.println(F("Turn successfully made"));
			}
			else
			{
				Serial.println(
					Game::GameManager::GetTurnFailReason(result)
				);
			}
		}

		Game::Ticket parseTicket() const
		{
			if (Serial.available() <= 0)
			{
				return Game::Ticket::_Invalid;
			}

			switch (Serial.read())
			{
			case Yellow:
				return Game::Ticket::Yellow;

			case Green:
				return Game::Ticket::Green;

			case Red:
				return Game::Ticket::Red;

			case Black:
				return Game::Ticket::Black;

			case Double:
				return Game::Ticket::Double;

			default:
				return Game::Ticket::_Invalid;
			}
		}

		int parseTarget() const
		{
			const int targetLength = Serial.available();

			int target = 0;
			for (int i = 0;;)
			{
				char input = Serial.read();

				if (input == '\n' || input == '\r')
				{
					break;
				}

				target += input - '0'; // convert char to int

				if (++i >= targetLength)
				{
					break;
				}

				target *= 10;
			}

			if (target == 0 || target > COMMON_MAP_SIZE)
			{
				Serial.println("Invalid target");

				return 0;
			}

			return target;
		}

		void updateCollect()
		{
			Serial.print(F("Collect update for "));
			Serial.println(playerCharacter);

			Serial.print(F(" - playercount: "));
			Serial.println(Game::Collector::GetData()->playerCount);
		}

		void updateRunning()
		{
			if (Game::GameManager::GetData()->state.activePlayer != playerId)
			{
				return;
			}

			const Game::Player* const player = Game::GameManager::ReadPlayer(playerId);

			if (player == NULL)
			{
				Serial.print(F("PlayerId not found"));
				return;
			}

			Serial.print(F("Next player: "));
			Serial.print(Game::ColorToFlash(player->data->color));
			Serial.print(F(" ("));
			Serial.print(player->data->player, 16);
			Serial.println(F(") ... Enter to view data"));

			while (Serial.available() == 0) delay(250);

			Serial.print(F("Your character: "));
			Serial.println(playerCharacter);

			Serial.print(F(" - position: "));
			Serial.println(player->state->position);

			Serial.print(F(" - yellow count: "));
			Serial.println(player->state->ticket.yellowTicketCount);

			Serial.print(F(" - green count: "));
			Serial.println(player->state->ticket.greenTicketCount);

			Serial.print(F(" - red count: "));
			Serial.println(player->state->ticket.redTicketCount);

			Serial.print(F(" - villian reveal: "));
			Serial.println(Game::GameManager::GetShowVillianPositionRound());

			Serial.println(F(" -> other tickets: "));

			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				const Game::Player* const player = Game::GameManager::ReadPlayer(Game::SetupManager::GetData()->playerContext.data[i].player);

				Serial.print(F(" - -> ("));
				Serial.print(Game::ColorToFlash(player->data->color));
				Serial.println(')');

				Serial.print(F(" - -> yellow count: "));
				Serial.println(player->state->ticket.yellowTicketCount);

				Serial.print(F(" - -> green count: "));
				Serial.println(player->state->ticket.greenTicketCount);

				Serial.print(F(" - -> red count: "));
				Serial.println(player->state->ticket.redTicketCount);

				if (player->data->type == Game::PlayerData::Type::Villian)
				{
					Serial.print(F(" - -> black count: "));
					Serial.println(player->state->villian.ticket.blackTicketCount);

					Serial.print(F(" - -> double count: "));
					Serial.println(player->state->villian.ticket.doubleTicketCount);
				}
			}

			if (player->data->type == Game::PlayerData::Type::Villian)
			{
				Serial.print(F(" - black count: "));
				Serial.println(player->state->villian.ticket.blackTicketCount);

				Serial.print(F(" - double count: "));
				Serial.println(player->state->villian.ticket.doubleTicketCount);
			}
			else
			{
				Serial.print(F(" -> last villian position: "));
				Serial.println(Game::GameManager::GetLastVillianPosition());

				Serial.println(" - -> villian last used:");

				for (int j = 0; j < Game::GameManager::GetData()->state.round; ++j)
				{
					Serial.print(" - -> (");
					Serial.print(j);
					Serial.print(") ");
					Serial.println(Game::TicketToFlash(Game::GameManager::GetData()->state.villianTicketUse[j]));
				}
			}
		}

		void updateSetup()
		{
		}
	};
}
