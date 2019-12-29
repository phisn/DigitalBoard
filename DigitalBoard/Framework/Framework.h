#pragma once

#include "../Game/GameController.h"
#include "../Communication/InterfaceManager.h"

namespace Framework
{
	template <
		typename StateContainer,
		typename CommController = Communication::CommController<StateContainer>,
		typename GameController = Game::GameController<StateContainer>
	>
	struct FrameworkConfiguration
	{
		typedef StateContainer StateContainer;
		typedef CommController CommController;
		typedef GameController GameController;
	};

	template <typename Configuration>
	class Framework
	{
		Configuration::GameController gameController;
		Configuration::CommController interfaceManager;

	public:
		Framework(EventConfigurator* const eventConfigurator)
			:
			eventConfiguration(eventConfigurator),
			gameController(&eventConfiguration),
			interfaceManager(&eventConfiguration, &gameController)
		{
		}

		void process()
		{
			interfaceManager.process(gameController.Process());
			// ...
		}

	private:
		EventConfiguration eventConfiguration;
	};
}
