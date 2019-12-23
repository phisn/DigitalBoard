#pragma once

#include "../Device/MemoryManager.h"

#include "../Game/Collector.h"
#include "../Game/GameContext.h"
#include "../Game/GameSector.h"

namespace Game
{
	namespace GameController
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
