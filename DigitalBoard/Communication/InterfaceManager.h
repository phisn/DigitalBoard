#pragma once

#include "../Communication/Interface.h"

namespace Communication
{
	template <
		typename StateContainer,
		typename InterfaceFactory
	>
	class InterfaceManager
	{
	public:
		InterfaceManager()
		{
		}

		void initialize()
		{
		}

		void process(const bool update)
		{
			InterfaceFactory::Process();
		}

		template <typename InterfaceT>
		void CreateInterface()
		{
			Interface<StateContainer>* const interface = InterfaceFactory::Create<InterfaceT>();
			interface->initialize();
		}

	private:
		Interface<StateContainer> interfaces[DEF_PLAYER_COUNT];
	};

	namespace InterfaceManagerAccess
	{
	}
}
