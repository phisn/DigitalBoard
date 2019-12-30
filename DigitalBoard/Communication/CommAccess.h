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

	// function failure is only caused by event failure
	// or deny. if event handler never fails or denies then
	// can the return be ignored
	struct CommAccess
	{
		template <typename StateContainer>
		bool pushInterface(Interface<StateContainer>* const interface)
		{
			return _pushInterface((InterfaceView*)interface);
		}

		template <typename StateContainer>
		bool linkInterface(
			Interface<StateContainer>* const interface,
			const InterfaceID interfaceID)
		{
			return _linkInterface(interface, interfaceID);
		}

		virtual void removeInterface(const InterfaceID interfaceID) = 0;
		virtual bool existsInterface(const InterfaceID interfaceID) const = 0;

	protected:
		virtual bool _pushInterface(InterfaceView* const interface) = 0;
		virtual bool _linkInterface(
			InterfaceView* const interface,
			const InterfaceID interfaceID) = 0;
	};
}
