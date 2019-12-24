#include "GameController.h"

#include "../Game/Collector.h"
#include "../Game/GameSector.h"

// can theoretically be accessed by
// extern, but should normally not.
// one of the exceptions is interfacemanager
namespace Current
{ 
	Game::StateManagerView* stateView;
	Game::GameContextView* contextView;
	void* data;
}


namespace
{
	bool finishRequested = false;

	Game::GameSector* sectorData;
	Game::GameContextView* rootView;
}

namespace Game
{
	namespace Controller
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
			case GameStateRaw::Shutdown:
				// ...

				break;
			case GameStateRaw::Prerunning:
				// ...

				break;
			case GameStateRaw::Error:
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

			const bool result = Current::stateView->Process();

			if (result)
			{
				Save();
				Current::stateView->UpdateRep();
			}

			return result;
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

		void Save()
		{

		}

		StateManagerView* GetStateManager()
		{
			return Current::stateView;
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
			Current::contextView = rootView->FindContext(sectorData->state - GameStateRaw::_Length);
			
			if (!Current::contextView->CanRestore())
			{
				return false;
			}

			Current::data = rootView->GetContextData(Current::contextView, Current::data);
			Current::stateView = Current::contextView->CreateState(Current::data);
		}

		GameStateRaw GetState()
		{
			return (GameStateRaw) sectorData->state;
		}
	}
}
