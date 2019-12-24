#pragma once

#include "../Communication/Interface.h"

namespace Communication
{
	template <typename StateContainer>
	class InterfaceManager
	{
	public:
		void initialize()
		{
		}

		void process(const bool update)
		{
		}

	private:
		Interface<StateContainer> interfaces[DEF_PLAYER_COUNT];
	};
}
