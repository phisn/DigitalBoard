#include "DeviceManager.h"

#include "../Communication/InterfaceManager.h"
#include "../Communication/WebInterface/WebServerManager.h"

#include "../Device/MapManager.h"
#include "../Device/NetworkManager.h"

#include "../Game/GameController.h"

namespace Device
{
	namespace GameManager
	{
		void InitializeDevice();
		void InitializeGame();
		void InitializeCommunication();

		void Initialize()
		{
			InitializeDevice();
			InitializeGame();
			InitializeCommunication();
		}

		void Uninitialize()
		{
		}

		void InitializeDevice()
		{
			DEBUG_MESSAGE("Device Init (low)");

			StatusLED::Initialize();

			FailureHandler::Initialize();
			MemoryManager::Initialize();
			OutputManager::Initialize();
			ExecutionManager::Initialize();
			

			DEBUG_MESSAGE("Device Init (high)");

			FaultHandler::Initialize();
			MapManager::Initialize();
			NetworkManager::Initialize();

			DEBUG_MESSAGE("Device Init finished");
		}

		void InitializeGame()
		{
			Game::Controller::Initialize();
		}

		void InitializeCommunication()
		{
			Communication::InterfaceManager::Initialize();
			Communication::WebServerManager::Initialize();
			Communication::LcdAccess::Initialize();
		}

		void Process()
		{
			NetworkManager::Process();
			
			const bool update = Game::Controller::Process();
			Communication::InterfaceManager::Process(update);

			FaultHandler::ValidateDeviceState();
		}
	}
}
