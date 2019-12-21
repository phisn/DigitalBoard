#pragma once

#include "../Communication/InterfaceManager.h"

namespace Communication
{
	class BotInterface
		:
		public Interface
	{
	public:
		void initialize(const Game::PlayerId playerId) override
		{
			this->playerId = playerId;
		}

		const Game::PlayerId getPlayerId() const override
		{
			return playerId;
		}

		bool process() override
		{
		}

		void update() override
		{
		}

		void notifyFault(const Device::Fault fault) override
		{
		}

	private:
		Game::PlayerId playerId;
	};
}
