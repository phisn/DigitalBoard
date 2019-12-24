#pragma once

#include "../Device/MemoryManager.h"
#include "../Game/GameContext.h"

namespace Game
{
	struct GameControllerView
	{
		virtual bool RequestFinish() = 0;
		virtual void Save() = 0;
	};

	namespace ControllerAccess
	{

	}

	template <typename StateContainer>
	class GameController
		:
		public GameControllerView
	{
		struct Data
		{
			GameSector sector;
			GameContext<StateContainer>::Data context;
		};

		GameSector* dataSector;
		void* dataContext;

	public:
		GameController()
		{
			Data* const dataPtr = (Data*) Device::MemoryManager::AllocateDynamic(sizeof(Data));

			Device::MemoryManager::ReadSector(
				Device::MemorySector::Game,
				(char*)dataPtr);

			dataSector = &dataPtr->sector;
			dataContext = (void*) &dataPtr->context;

			switch (dataSector->state)
			{
			}

			currentContext = &root;
			currentManager = root.CreateState(dataContext);
		}

		bool Process()
		{
			if (finishRequested)
			{
				finishRequested = false;
				FinishState();

				return false;
			}

			const bool result = currentManager->Process();

			if (result)
			{
				Save();
				currentManager->UpdateRep();
			}

			return result;
		}

		bool RequestFinish() override
		{
		}

		void Save() override
		{
		}

	private:
		void FinishState()
		{
		}

		GameContextView* currentContext;
		StateManagerView* currentManager;

		bool finishRequested = false;

		GameContext<StateContainer> root;
	};
}
