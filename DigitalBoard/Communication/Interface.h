#pragma once

#include "../Game/GameContext.h"
#include "../Game/Collector.h"

namespace Communication
{
	template <typename StateContainer>
	struct Interface
		:
		public InterfaceContainer<StateContainer>
	{
		virtual ~Interface()
		{
		}

		virtual void initialize(const Game::PlayerId id) = 0;
		virtual const Game::PlayerId getPlayerId() const = 0;

		virtual void notifyFault(const Device::Fault fault) = 0;
	};
}
