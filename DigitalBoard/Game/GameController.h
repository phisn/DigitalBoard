#pragma once

#include "../Device/MemoryManager.h"

#include "../Game/Collector.h"
#include "../Game/EventHandler.h"
#include "../Game/GameSector.h"
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
		};

		GameSector* dataSector;

	public:
		typedef StateFactory<StateContainer> Context;

		GameController()
			:
			GameControllerView()
		{
			Data* const dataPtr = (Data*) Device::MemoryManager::AllocateDynamic(sizeof(Data));

			Device::MemoryManager::ReadSector(
				Device::MemorySector::Game,
				(char*)dataPtr);

			dataSector = &dataPtr->sector;
			stateIterator.data = (void*) &dataPtr->context;

			switch (dataSector->state)
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

			stateIterator.context = &rootContext;
			stateIterator.manager = rootContext.CreateState(stateIterator.data);
		}

		bool Process()
		{
			if (finishRequested)
			{
				finishRequested = false;
				FinishState();

				return false;
			}

			const bool result = stateIterator.manager->Process();

			if (result)
			{
				Save();
				stateIterator.manager->UpdateRep();
			}

			return result;
		}

		bool RequestFinish() override
		{
			if (!finishRequested && stateIterator.manager->Finish())
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
			stateIterator.context = rootContext.FindContext(dataSector->state - GameStateRaw::_Length);
			
			if (!stateIterator.context->CanRestore())
			{
				return false;
			}

			if (!eventHandlerManager.Get<RestoreEventHandler>()->Ask())
			{
			}
		}

		void FinishState()
		{
			if (stateIterator.context->IsFinal())
			{
				// request restart or error
			}

			delete stateIterator.manager;

			stateIterator.data = stateIterator.context->GetContextData(1, stateIterator.data);
			stateIterator.context = stateIterator.context->FindContext(1);
			stateIterator.manager = stateIterator.context->CreateState(stateIterator.data);

			stateIterator.manager->Create();
		}

		struct StateIterator
		{
			StateFactoryView* context;
			StateManagerView* manager;
			void* data;

		} stateIterator;

		bool finishRequested = false;

		EventHandlerManager eventHandlerManager;
		StateFactory<StateContainer> rootContext;
	};
}
