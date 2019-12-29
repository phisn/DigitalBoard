#pragma once

#include "../Communication/Interface.h"

namespace Communication
{
	template <typename StateContainer>
	class InterfaceManager
	{
	public:
		InterfaceManager()
		{
		}

		void CreateInterface(Interface<StateContainer>* const interface)
		{
			// send directly to event
			// has to be as dynamic as possible
		}

	private:
		Interface<StateContainer> interfaces[DEF_PLAYER_COUNT] = { };
	};
}
