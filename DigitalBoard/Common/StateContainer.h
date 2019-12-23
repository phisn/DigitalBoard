#pragma once

template <typename State, typename... States>
struct StateContainer;

template <typename State>
struct StateContainer<State>
{
	typedef State State;
};

template <typename State, typename... States>
struct StateContainer
{
	typedef State State;
};
