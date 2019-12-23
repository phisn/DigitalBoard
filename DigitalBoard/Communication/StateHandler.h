#pragma once

namespace Communication
{
	template <typename StateContainer>
	struct StateHandler;

	template <typename State>
	struct StateHandlerContent
	{
		virtual void process(State* const state) = 0;
		virtual void update(State* const state) = 0;
	};

	template <typename StateContainer>
	struct StateHandler
	{
		static_assert(false, "StateHandler only accepts StateContainer");
	};

	template <typename State>
	struct StateHandler<StateContainer<State>>
		:
		public StateHandlerContent<State>
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
