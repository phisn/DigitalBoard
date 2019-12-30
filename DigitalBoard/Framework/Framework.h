#pragma once

#include "../Communication/CommController.h"

#include "../Framework/Event/InterfaceJoinEvent.h"
#include "../Framework/Event/RestoreEvent.h"

#include "../Game/GameController.h"

namespace Framework
{
	template <typename Configuration>
	class Framework
	{
		Configuration::GameController gameController;
		Configuration::CommController interfaceManager;

	public:
		Framework(_EventConfigurator* const eventConfigurator)
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
