#pragma once

#include "../Device/FaultHandler.h"
#include "../Game/StateFactory.h"
#include "../Game/CollectData.h"

namespace Communication
{
	/*
		
		Add interfaces dynamically by external hidden force.
		"StaticInterfaces" should be called on process / update
		and be possible, to create new interfaces or do more
		interface stuff like menus
	
	*/

	typedef void InterfaceView;

	template <typename StateContainer>
	class Interface
		:
		public Game::StateHandler<StateContainer>
	{
	public:
		virtual ~Interface()
		{
		}

		virtual void initialize(const Game::PlayerId id)
		{
			this->id = id;
		}

		virtual void process() = 0;
		virtual void notifyFault(const Device::Fault fault) = 0;

	private:
		Game::PlayerId id;
	};
}
