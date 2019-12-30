#pragma once

#include "../Communication/Interface.h"

namespace Communication
{
	/*
		an interface is an actor, which can exists multiple
		times. actors that exists only once should be
		implmented using routines. an interface gets assigned
		a ID, that should be saved if possible. interfaces
		should be linked if an id is available and created
		if not
	*/

	struct CommAccess
	{
		template <typename StateContainer>
		bool pushInterface(Interface<StateContainer>* const interface)
		{
			return _pushInterface((InterfaceView*) interface);
		}

		template <typename StateContainer>
		bool linkInterface(
			Interface<StateContainer>* const interface,
			const InterfaceID interfaceID)
		{
			return _linkInterface(interface, interfaceID);
		}

		virtual bool existsInterface(const InterfaceID interfaceID) const = 0;

	protected:
		virtual bool _pushInterface(InterfaceView* const interface) = 0;
		virtual bool _linkInterface(
			InterfaceView* const interface,
			const InterfaceID interfaceID) = 0;
	};
	
	template <typename StateContainer>
	class CommController
		:
		public CommAccess
	{
	public:
		typedef Interface<StateContainer> Interface;

		CommController()
		{
		}

		void initialize()
		{
		}

		void process(const bool update)
		{
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
				Interface* const interface = (Interface*) interfaceView;
				interface->initialize(interfaceID);
				interfaces[interfaceCounter++] = interface;

				return true;
			}

			return false;
		}

		InterfaceID generateInterfaceID()
		{
		}

		Framework::EventConfiguration* const eventConfig;
		int interfaceCounter = 0;
		Interface* interfaces[DEF_INTERFACE_COUNT] = { };
	};
}
