#include "InterfaceManager.h"

#include "../Communication/SerialInterface.h"
#include "../Communication/WebInterface/WebInterface.h"

namespace
{
	Communication::Interface* interfaces[COMMON_MAX_PLAYERCOUNT];
	FlashString fault_remove_invalid_playerid = DEVICE_FAULT_MESSAGE("Got invalid PlayerId in remove   ");
}

namespace Communication
{
	namespace InterfaceManager
	{
		void FaultNotifier(const Device::Fault fault);
		void CollectInterfaces();

		void InitializeInterface(Interface* const interface);

		bool RemoveInterfaceCollect(const Game::PlayerId playerId);
		bool RemoveInterfaceCommon(const Game::PlayerId playerId);

		void RegisterFaultNotifier()
		{
			Device::FaultHandler::RegisterFaultInterfaceNotifier(
				(Device::FaultHandler::InterfaceNotifierCallback) FaultNotifier
			);
		}

		void FaultNotifier(const Device::Fault fault)
		{
			if (Game::Controller::GetState() == Game::GameState::PreRunning)
			{
				return;
			}

			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				interfaces[i]->notifyFault(fault);
			}
		}

		void Initialize()
		{
			memset(interfaces, 0, sizeof(Interface*) * COMMON_MAX_PLAYERCOUNT);
		}

		void Unintialize()
		{
		}

		void Process(const bool update)
		{
			if (Game::Controller::GetState() == Game::GameState::Collect)
			{
				CollectInterfaces();
			}

			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				Interface* const interface = interfaces[i];

				if (interface == NULL)
				{
					continue;
				}

				if (update)
				{
					interface->update();
				}

				// important change. needs refresh<
				if (interface->process())
				{
					break;
				}
			}

			LcdAccess::Process();
		}

		// not all interfaces are collectable 
		// ex. webinterface because async
		void CollectInterfaces()
		{
			Communication::SerialInterface::UpdateCollect();
		}

		void PushInterface(Interface* const interface)
		{
			interfaces[Game::Collector::GetData()->playerCount] = interface;
			interface->initialize(Game::Collector::CreatePlayer());
		}

		void LinkInterface(Interface* const interface, const int playerIndex)
		{
			interfaces[playerIndex] = interface;
			interface->initialize(Game::Collector::GetData()->playerIds[playerIndex]);
		}

		void RemoveInterface(const Game::PlayerId playerId)
		{
			if (Game::Controller::GetState() == Game::GameState::Collect
					? RemoveInterfaceCollect(playerId)
					: RemoveInterfaceCommon(playerId))
			{
				return;
			}

#ifdef VM_DEBUG
			DEBUG_MESSAGE("GOT");
			DEBUG_MESSAGE(playerId);
			DEBUG_MESSAGE("HAVE");

			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
				if (interfaces[i] != NULL)
				{
					DEBUG_MESSAGE(interfaces[i]->getPlayerId());
				}

			DEBUG_MESSAGE("END HAVE");
#endif

			Device::FaultHandler::Handle(
				{
					Device::FaultModule::InterfaceManager,
					InterfaceManager::FID::REMOVE_INVALID_PLAYERID,
					fault_remove_invalid_playerid
				}, true);
		}

		bool RemoveInterfaceCollect(const Game::PlayerId playerId)
		{
			const int playerIndex = Game::Collector::FindPlayerIndex(playerId);
			if (playerIndex == -1 || !Game::Collector::RemovePlayer(playerId))
			{
				return false; // throw common removeinterface failure
			}

			delete interfaces[playerIndex];

			// playercount is already one smaller from RemovePlayer
			for (int j = playerIndex; j < Game::Collector::GetData()->playerCount; ++j)
			{
				interfaces[j] = interfaces[j + 1];
			}

			return true;
		}

		bool RemoveInterfaceCommon(const Game::PlayerId playerId)
		{
			const int playerIndex = Game::Collector::FindPlayerIndex(playerId);

			if (playerIndex == -1)
			{
				return false;
			}

			delete interfaces[playerIndex];
			interfaces[playerIndex] = NULL;

			return true;
		}

		bool ExistsInterface(const Game::PlayerId playerId)
		{
			return interfaces[Game::Collector::FindPlayerIndex(playerId)] != NULL;
		}
	}
}
