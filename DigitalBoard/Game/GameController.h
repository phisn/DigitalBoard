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

	template <typename StateContainer>
	class GameController
		:
		public GameControllerView
	{
		struct Data
		{
			GameSector sector;
			GameContext<StateContainer>::Data context;
		}* data;

	public:
		GameController()
		{
			data = (Data*) Device::MemoryManager::AllocateDynamic(sizeof(Data));

			Device::MemoryManager::ReadSector(
				Device::MemorySector::Game,
				(char*) data);

			current = &root;
		}

		bool RequestFinish() override
		{
		}

		void Save() override
		{
		}

	private:
		GameContextView* current;
		GameContext<StateContainer> root;
	};

	namespace _Controller
	{
		// should be called with needed
		// states -> StateManagerViews
		template <typename... States>
		void Initialize()
		{
			static GameContext<States...> context;

			struct Data
			{
				GameSector sector;
				decltype(context)::Data context;
			};

			Initialize(
				(GameContextView*) &context,
				Device::MemoryManager::AllocateDynamic(sizeof(Data))
			);
		}

		// should normaly not be called
		// is used by template initialize
		void Initialize(GameContextView* rootView, void* const data);
		
		// return indicates if data
		// has changed. important for
		// communication layer
		bool Process(); 
		
		// finish allow, if return is true
		bool RequestFinish();

		// manually saves data to sector
		// should normally not be called
		// by other modules. stay for debug
		void Save();
	}
}
