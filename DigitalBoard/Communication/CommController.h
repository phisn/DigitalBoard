#pragma once

#include "../Communication/CommAccess.h"
#include "../Communication/Interface.h"
#include "../Communication/RoutineContainer.h"
#include "../Framework/EventConfigurator.h"
#include "../Game/GameController.h"

namespace Communication
{
	template <
		typename StateContainer,
		typename RoutineContainer
	>
	class CommController
		:
		public CommAccess
	{
	public:
		typedef Interface<StateContainer> Interface;

		CommController(
			Framework::EventConfiguration* const eventConfig,
			Game::GameController<StateContainer>* const gameController)
			:
			eventConfig(eventConfig),
			gameController(gameController)
		{
		}

		void initialize()
		{
			routineContainer.Initialize(this);
		}

		void process(const bool update)
		{
			if (update)
			{
				routineContainer.Update(
					gameController->getCurrentContext()->getFactory(),
					gameController->getCurrentContext()->getManager()
				);
			}

			routineContainer.Process();

			for (int i = 0; i < interfaceCounter; ++i)
			{
				if (update)
				{
					gameController->getCurrentContext()->getFactory()->VisitHandler(
						interfaces[i], 
						gameController->getCurrentContext()->getManager());
				}

				interfaces[i]->process();
			}
		}

		void removeInterface(const InterfaceID interfaceID) override
		{
			for (int i = 0; i < interfaceCounter; ++i)
				if (interfaces[i]->getID() == interfaceID)
				{
					Framework::InterfaceLinkEventData{ interfaceID, interfaces[i] };

					delete interfaces[i];

					if (i + 1 == interfaceCounter)
					{
						interfaces[i] = NULL;
					}
					else
					{
						for (int j = i; j < interfaceCounter - i - 1; ++j)
							interfaces[j] = interfaces[j + 1];
					}

					--interfaceCounter;

					return;
				}

			// error ...
		}

		bool existsInterface(const InterfaceID interfaceID) const override
		{
			for (int i = 0; i < interfaceCounter; ++i)
				if (interfaces[i]->getID() == interfaceID)
				{
					return true;
				}

			return false;
		}

	private:
		bool _pushInterface(InterfaceView* const interfaceView) override
		{
			const InterfaceID interfaceID = generateInterfaceID();

			Framework::InterfacePushEventData data{ interfaceID, interfaceView };
			if (eventConfig->GetHandler<Framework::InterfacePushHandler>()->Ask(&data))
			{
				addInterface(interfaceID, (Interface*)interfaceView);

				return true;
			}

			return false;
		}

		InterfaceID generateInterfaceID() const
		{
			while (true)
			{
				InterfaceID interfaceID = esp_random();

				if (!existsInterface(interfaceID))
				{
					return interfaceID;
				}
			}
		}

		bool _linkInterface(
			InterfaceView* const interfaceView,
			const InterfaceID interfaceID) override
		{
			if (existsInterface(interfaceID))
			{
				// error ...
			}

			Framework::InterfacePushEventData data{ interfaceID, interfaceView };
			if (eventConfig->GetHandler<Framework::InterfacePushHandler>()->Ask(&data))
			{
				addInterface(interfaceID, (Interface*) interfaceView);

				return true;
			}

			return false;
		}

		void addInterface(
			const InterfaceID interfaceID,
			Interface* const interface)
		{
			interface->initialize(interfaceID);
			interfaces[interfaceCounter++] = interface;
		}

		Framework::EventConfiguration* const eventConfig;
		Game::GameController<StateContainer>* const gameController;

		// number of current interfaces and points to next
		// interface index. obviously not restored
		int interfaceCounter = 0;
		Interface* interfaces[DEF_INTERFACE_COUNT] = { };

		RoutineContainer routineContainer;
	};
}
