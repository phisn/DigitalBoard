#pragma once

#include "../Communication/Interface.h"
#include "../Framework/EventConfigurator.h"

namespace Communication
{
	template <typename StateContainer>
	class InterfaceManager
	{
	public:
		InterfaceManager(Framework::EventConfiguration* const eventConfig)
			:
			eventConfig(eventConfig)
		{
		}

		// interface type needs to be able to cast into
		// interface with statecontainer and be always the
		// same. ijhandler needs to know interface type
		// because it will be directly casted to void
		template <typename InterfaceT = Interface<StateContainer>>
		void CreateInterface(InterfaceT* const interface)
		{
			Framework::InterfacePushEventData data{ (InterfaceView*) interface };
			
			if (eventConfig->GetHandler<Framework::InterfacePushHandler>()->Ask(&data))
			{
				interfaces[interfaceCount++] = interface;
			}
		}

	private:
		Framework::EventConfiguration* const eventConfig;
		Game::StateContext* const context;

		int interfaceCount = 0;
	};
}
