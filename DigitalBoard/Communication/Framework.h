#pragma once

#include "Game/GameController.h"
#include "Communication/InterfaceManager.h"

template <typename StateContainer>
class Framework
{
public:
	typedef Communication::InterfaceManager<StateContainer> InterfaceManager;
	typedef Game::GameController<StateContainer> GameController;

	Framework()
	{
	}

	void process()
	{
		interfaceManager.process(gameController.Process());
		// ...
	}

private:
	GameController gameController;
	InterfaceManager interfaceManager;
};
