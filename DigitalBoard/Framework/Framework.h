#pragma once

#include "../Communication/CommController.h"
#include "../Framework/EventConfigurator.h"
#include "../Game/GameController.h"

namespace Framework
{
	template <typename StateContainer>
	class Framework
	{
	public:
		Framework(EventConfigurator* const eventConfigurator)
			:
			eventConfig(eventConfigurator),
			gameController(&eventConfig),
			commController()
		{
		}

		void process()
		{
			commController.process(gameController.Process());
			// ...
		}

		Game::GameControllerView* getGame()
		{
			return &gameController;
		}

	private:
		Game::GameController<StateContainer> gameController;
		Communication::CommController<StateContainer> commController;

		EventConfiguration eventConfig;
	};
}
