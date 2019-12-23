#include "GameController.h"

namespace
{
	bool interrupted = false;
	bool finishRequested = false;

	namespace Current
	{
		Game::StateManagerView* stateView;
		Game::GameContextView* contextView;
		void* data;
	}

	Game::GameSector* sectorData;
	Game::GameContextView* rootView;
}

namespace Game
{
	namespace GameController
	{
		void FinishState();

		bool Restore();

		void Initialize(GameContextView* rootView, void* const data)
		{
			sectorData = (Game::GameSector*) data;
			Current::data = (void*)((ptrdiff_t) data + sizeof(Game::GameSector));

			::rootView = rootView;

			Device::MemoryManager::ReadSector(
				Device::MemorySector::Game,
				(char*) sectorData);

			switch (sectorData->state)
			{
			case GameState::Shutdown:
				// ...

				break;
			case GameState::Prerunning:
				// ...

				break;
			case GameState::Error:
				// ...

				break;
			default:
				if (Restore())
				{
					return;
				}

				break;
			}

			Current::contextView = rootView;
			Current::stateView = rootView->CreateState(data);
		}

		bool Process()
		{
			if (finishRequested)
			{
				finishRequested = false;
				FinishState();

				return false;
			}

			bool result = Current::stateView->Process();

			if (result)
			{
			}


		}

		void Interrupt(InterruptView* const interruptView)
		{
			delete Current::stateView;
			Current::stateView = interruptView;

			interrupted = true;
		}

		bool RequestFinish()
		{
			if (!finishRequested && Current::stateView->Finish())
			{
				finishRequested = true;
				return true;
			}

			return false;
		}

		void FinishState()
		{
			if (Current::contextView->IsFinal())
			{
				// request restart
			}

			delete Current::stateView;

			// set data and context of index 1
			// -> set to next

			// data is before contextview, because in
			// this way no temp variable is needed
			Current::data = Current::contextView->GetContextData(1, Current::data);
			Current::contextView = Current::contextView->FindContext(1);
			Current::stateView = Current::contextView->CreateState(Current::data);

			Current::stateView->Create();
		}

		bool Restore()
		{
			Current::contextView = rootView->FindContext(sectorData->state - GameState::_Length);
			
			if (!Current::contextView->CanRestore())
			{
				return false;
			}

			Current::data = rootView->GetContextData(Current::contextView, Current::data);
			Current::stateView = Current::contextView->CreateState(Current::data);
		}
	}
}
