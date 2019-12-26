#pragma once

#include "StateFactory.h"

namespace Game
{
	template <typename StateContainer>
	struct StateContext
	{
		StateFactory<StateContainer> factory;
		void* data;
	};
}
