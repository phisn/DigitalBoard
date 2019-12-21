#include "LcdAccess.h"

#include "../Communication/InterfaceManager.h"
#include "../Device/OutputManager.h"

namespace
{
	FlashString selection_common_back = FPSTR("Back ---         ");
	FlashString selection_common_close = FPSTR("Close ---        ");

	// collecting
	struct SelectionCollectingGeneral
	{
		enum
		{
			Close,
			StartGame,
			Restart,
			Players,
			AddBot
		};
	};

	FlashString selection_collecting_general[] =
	{
		selection_common_close,
		FPSTR("Continue         "),
		FPSTR("Restart          "),
		FPSTR("Players          "),
		FPSTR("Add Bot          "),
	};

	FlashString message_startgame_0 = DEVICE_LCD_MESSAGE("Unable to start game");
	FlashString message_startgame_1 = DEVICE_LCD_MESSAGE("At least 4          ");
	FlashString message_startgame_2 = DEVICE_LCD_MESSAGE("players needed      ");
	
	// setup
	struct SelectionSetupGeneral
	{
		enum
		{
			Close,
			StartGame,
			Players,
			Abort
		};
	};

	FlashString selection_setup_general[] =
	{
		selection_common_close,
		FPSTR("Start Game       "),
		FPSTR("Players          "),
		FPSTR("Abort            ")
	};


	struct SelectionSetupSpecificPlayer
	{
		enum
		{
			Close,
			SetColor,
			SetVillian
		};
	};

	// running
	struct SelectionRunningGeneral
	{
		enum
		{
			Close,
			Settings,
			Players,
			Restart,
			Shutdown,
			Resave,
			PrintSector
		};
	};

	FlashString selection_running_general[] =
	{
		selection_common_close,
		FPSTR("Settings         "),
		FPSTR("Players          "),
		FPSTR("Restart          "),
		FPSTR("Resave           "),
		FPSTR("Shutdown         "),
		FPSTR("PrintSector      ")
	};

	struct SelectionRunningSpecificPlayer
	{
		enum
		{
			Back,
			ShowPid,
			Kick,
			IntType,
			PrintSector
		};
	};

	FlashString selection_running_specificPlayer[] =
	{
		selection_common_back,
		FPSTR("Show PlayerId    "),
		FPSTR("Kick             "),
		FPSTR("Interface Type   "),
		FPSTR("Print Sector     ")
	};
}

namespace Communication
{
	namespace LcdAccess
	{
		namespace MenuCollecting
		{
			void General();

			void StartGame();
			void Restart();

			void PlayerSelect();
			void PlayerSpecific(const Game::PlayerId playerId);
		}

		namespace MenuSetup
		{
			void General();

			void StartGame();
			void Restart();

			void PlayerSpecific();
		}

		namespace MenuRunning
		{
			void General();

			void Settings();

			void PlayerSelect();
			void PlayerSpecific(const Game::PlayerId playerId);

			void Restart();
			void Resave();
			void Shutdown();
			void PrintSector();
		}

		void Initialize()
		{
		}

		void Uninitialize()
		{
		}

		void Process()
		{
			const Game::GameState state = Game::Controller::GetState();

			if (state != Game::GameState::Collect &&
				state != Game::GameState::Running)
			{
				return;
			}

			if (Device::OutputManager::Interact::GetChoice() ==
				Device::OutputManager::Interact::Choice::Enter)
			{
				delay(300);

				switch (state)
				{
				case Game::GameState::Collect:
					MenuCollecting::General();

					break;
				case Game::GameState::Running:
					MenuRunning::General();

					break;
				}

				Game::BoardManager::ReloadLcd();
				delay(300);
			}
		}

		namespace MenuCollecting
		{
			void General()
			{
				while (true)
				{
					switch (Device::OutputManager::Interact::Select(
						selection_collecting_general,
						sizeof(selection_collecting_general) / sizeof(*selection_collecting_general)))
					{
					case SelectionCollectingGeneral::StartGame:
						StartGame();

						return;
					case SelectionCollectingGeneral::Restart:
						Restart();

						return;
					case SelectionCollectingGeneral::Players:
						PlayerSelect();

						break;
					default:
						return;
					}
				}
			}

			void StartGame()
			{
				if (Game::Collector::GetData()->playerCount >= 4)
				{
					Game::Controller::RequestFinish();
				}
				else
				{
					Device::OutputManager::Lcd::DisplayLineType(0, message_startgame_0);
					Device::OutputManager::Lcd::DisplayLineType(1, message_startgame_1);
					Device::OutputManager::Lcd::DisplayLineType(2, message_startgame_2);
					Device::OutputManager::Lcd::DisplayLineType(3, Device::OutputManager::Interact::GetCommonPressExit());

					Device::OutputManager::Interact::AwaitEnter();
				}
			}

			void Restart()
			{
			}

			void PlayerSelect()
			{
			}

			void PlayerSpecific(const Game::PlayerId playerId)
			{
			}
		}

		namespace MenuRunning
		{
			void General()
			{
				while (true)
				{
					switch (Device::OutputManager::Interact::Select(
						selection_running_general,
						sizeof(selection_running_general) / sizeof(*selection_running_general)))
					{
					case SelectionRunningGeneral::Settings:
						Settings();

						break;
					case SelectionRunningGeneral::Players:
						PlayerSelect();

						break;
					case SelectionRunningGeneral::Restart:
						Restart();

						return;
					case SelectionRunningGeneral::Resave:
						Resave();

						return;
					case SelectionRunningGeneral::Shutdown:
						Shutdown();

						return;
					case SelectionRunningGeneral::PrintSector:
						PrintSector();

						return;
					default:
						return;
					}
				}
			}

			void Settings()
			{
			}

			void PlayerSelect()
			{
				while (true)
				{
					const int selectionCount = Game::Collector::GetData()->playerCount + 1; // + back
					FlashString* selectionBuffer = new FlashString[selectionCount];

					selectionBuffer[0] = selection_common_back;

					for (int i = 1; i < selectionCount; ++i)
					{
						selectionBuffer[i] = Game::ColorToFlash(Game::SetupManager::GetData()->playerContext.data[i - 1].color);
					}

					const int result = Device::OutputManager::Interact::Select(
						selectionBuffer,
						selectionCount);
					delete[] selectionBuffer;

					if (result == 0)
					{
						return;
					}

					PlayerSpecific(Game::SetupManager::GetData()->playerContext.data[result - 1].player);
				}
			}

			void PlayerSpecific(const Game::PlayerId playerId)
			{
				while (true)
				{
					switch (Device::OutputManager::Interact::Select(
						selection_running_specificPlayer,
						sizeof(selection_running_specificPlayer) / sizeof(*selection_running_specificPlayer)))
					{
					case SelectionRunningSpecificPlayer::ShowPid:
					{
						Device::OutputManager::Lcd::Clear();

						char lcdBuffer[DEVICE_LCD_WIDTH] = "";

						const Game::Player* const player = Game::GameManager::ReadPlayer(playerId);

						sprintf_P(
							lcdBuffer,
							(const char*)F("PlayerId: %x"),
							playerId);
						Device::OutputManager::Lcd::DisplayLineType(
							0,
							lcdBuffer);

						sprintf_P(
							lcdBuffer,
							(const char*)F("Color: %s"),
							Game::ColorToFlash(player->data->color));
						Device::OutputManager::Lcd::DisplayLineType(
							1,
							lcdBuffer);

						Device::OutputManager::Lcd::DisplayLineType(
							3,
							Device::OutputManager::Interact::GetCommonPressExit()
						);

						Device::OutputManager::Interact::AwaitEnter();
					}

					break;
					case SelectionRunningSpecificPlayer::Kick:
						Communication::InterfaceManager::RemoveInterface(playerId);

						break;
					case SelectionRunningSpecificPlayer::IntType:

						break;
					case SelectionRunningSpecificPlayer::PrintSector:

						break;
					default:
						return;
					}
				}
			}

			void Restart()
			{
				ESP.restart();
			}

			void Resave()
			{
			}

			void Shutdown()
			{
			}

			void PrintSector()
			{
			}
		}
	}
}
