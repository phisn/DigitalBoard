#pragma once

#include "../Device/MemoryManager.h"

#include "../Game/Collector.h"
#include "../Game/EventHandler.h"
#include "../Game/GameSector.h"
#include "../Game/StateContext.h"
#include "../Game/StateFactory.h"

namespace Game
{
	class GameControllerView
	{
	public:
		GameControllerView()
		{
			instance = this;
		}

		virtual bool RequestFinish() = 0;
		virtual void Save() = 0;

		static GameControllerView* Access()
		{
			if (instance == NULL)
			{
				// error
			}

			return instance;
		}

	private:
		static GameControllerView* instance;
	};

	template <typename StateContainer>
	class GameController
		:
		public GameControllerView
	{
		struct Data
		{
			GameSector sector;
			StateFactory<StateContainer>::Data context;
		} data;

	public:
		typedef StateFactory<StateContainer> Context;

		GameController(EventHandlerContainer* const container)
			:
			eventHandlerManager(container),
			context(&rootFactory, (void*) &data),
			GameControllerView()
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

			context.Begin();
			context->Create();
		}

		bool Process()
		{
			if (finishRequested)
			{
				finishRequested = false;
				FinishState();

				return false;
			}

			const bool result = context->Process();

			if (result)
			{
				Save();
				context->UpdateRep();
			}

			return result;
		}

		bool RequestFinish() override
		{
			if (!finishRequested && context->Finish())
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

			RestoreEventData data(factory, context.getData());
			if (!eventHandlerManager.Get<RestoreEventHandler>()->Ask(&data))
			{
				return false;
			}

			context.Begin(factory);
			context->Restore();
		}

		void FinishState()
		{
			if (context.getFactory()->IsFinal())
			{
				// request restart or error
			}

			context.Next();
			context->Create();
		}

		bool finishRequested = false;

		EventHandlerManager eventHandlerManager;
		StateContext context;
		StateFactory<StateContainer> rootFactory;
	};
}
