#pragma once

#include "../Game/Collector.h"

namespace Communication
{
	struct Interface
	{
		virtual ~Interface()
		{
		}

		virtual void initialize(const Game::PlayerId id) = 0;
		virtual const Game::PlayerId getPlayerId() const = 0;

		// return == has critical change
		virtual bool process() = 0;
		virtual void update() = 0;

		virtual void notifyFault(const Device::Fault fault) = 0;
	};
}
