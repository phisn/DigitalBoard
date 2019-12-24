#pragma once

#include "../Game/StateContainer.h"

namespace Game
{
	template <typename StateContainer>
	struct StateHandler;

	template <typename State>
	struct StateHandlerContent
	{
		virtual void update(State* const state) = 0;
	};

	template <typename StateContainer>
	struct StateHandler
	{
		static_assert(false, "StateHandler only accepts StateContainer");
	};

	struct StateHandlerView
	{
	};

	template <typename State>
	struct StateHandler<StateContainer<State>>
		:
		public StateHandlerContent<State>,
		public StateHandlerView
	{
	};

	template <typename State, typename... States>
	struct StateHandler<StateContainer<State, States...>>
		:
		public StateHandlerContent<State>,
		public StateHandler<StateContainer<States...>>
	{
	};
}
