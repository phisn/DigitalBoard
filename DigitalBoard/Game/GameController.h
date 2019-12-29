#pragma once

#include "../Device/MemoryManager.h"

#include "../Framework/Event.h"

#include "../Game/Collector.h"
#include "../Game/GameSector.h"
#include "../Game/StateContext.h"
#include "../Game/StateFactory.h"

namespace Game
{
	class GameControllerView
	{
	public:
		virtual bool RequestFinish() = 0;
		virtual void Save() = 0;
	};

	template <typename StateContainer>
	class GameController
		:
		public GameControllerView
	{
		struct Data
		{
			GameSector sector;
			StateFactory<StateContainer>::Data stateContext;
		} data;

	public:
		typedef StateFactory<StateContainer> Context;

		GameController(Framework::EventConfigurator* const container)
			:
			eventHandlerManager(container),
			stateContext(&rootFactory, (void*) &data)
		{
			Device::MemoryManager::ReadSector(
				Device::MemorySector::Game,
				(char*) &data);

			switch (data.sector.state)
			{
			case GameStateRaw::Shutdown:
			case GameStateRaw::Prerunning:
			case GameStateRaw::Error:
				break;

			default:
				if (Restore())
				{
					return;
				}
			}

			stateContext.Begin();
			stateContext->Create();
		}

		bool Process()
		{
			if (finishRequested)
			{
				finishRequested = false;
				FinishState();

				return false;
			}

			const bool result = stateContext->Process();

			if (result)
			{
				Save();
				stateContext->UpdateRep();
			}

			return result;
		}

		bool RequestFinish() override
		{
			if (!finishRequested && stateContext->Finish())
			{
				finishRequested = true;
			}

			return finishRequested;
		}

		void Save() override
		{
		}

	private:
		bool Restore()
		{
			StateFactoryView* const factory = rootFactory.FindFactory(data.sector.state - GameStateRaw::_Length);

			if (!factory->CanRestore())
			{
				return false;
			}

			Framework::RestoreEventData data(factory, stateContext.getData());
			if (!eventHandlerManager.GetHandler<Framework::RestoreEventHandler>()->Ask(&data))
			{
				return false;
			}

			stateContext.Begin(factory);
			stateContext->Restore();

			return true;
		}

		void FinishState()
		{
			if (stateContext.getFactory()->IsFinal())
			{
				// request restart or error
			}

			stateContext.Next();
			stateContext->Create();
		}

		bool finishRequested = false;

		Framework::EventConfiguration eventHandlerManager;
		StateContext stateContext;
		StateFactory<StateContainer> rootFactory;
	};

	namespace GameAccess
	{
		bool RequestFinish();
		GameState GetState();
	}
}
