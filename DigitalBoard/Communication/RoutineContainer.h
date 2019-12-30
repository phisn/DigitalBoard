#pragma once

namespace Communication
{
	template <typename Routine, typename... Routines>
	struct RoutineContainer;

	template <typename Routine>
	struct RoutineContainer<Routine>
	{
		static void Process()
		{
			Routine::Process();
		}
	};

	template <typename Routine, typename... Routines>
	struct RoutineContainer
	{
		typedef RoutineContainer<Routines...> Next;

		static void Process()
		{
			Routine::Process();
			Next::Process();
		}
	};
}
