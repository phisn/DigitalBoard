#pragma once

#include "../Communication/CommAccess.h"
#include "../Game/StateHandler.h"

namespace Communication
{
	struct RoutineView
	{
		virtual void initialize(CommAccess* const commAccess) = 0;
		virtual void process() = 0;
	};

	template <typename StateContainer>
	struct RoutineBase
		:
		public RoutineView,
		public Game::StateHandler<StateContainer>
	{
	};

	template <typename Routine, typename... Routines>
	struct RoutineContainer;

	template <typename Routine>
	struct RoutineContainerBase
	{
	protected:
		Routine routine;
	};

	template <typename Routine>
	struct RoutineContainer<Routine>
		:
		public RoutineContainerBase<Routine>
	{
		void Initialize(CommAccess* const commAccess)
		{
			routine.process(commAccess);
		}

		void Process()
		{
			routine.process();
		}

		void Update(
			Game::StateFactoryView* const factory,
			Game::StateManagerView* const manager)
		{
			factory->VisitHandler(&routine, manager);
		}
	};

	template <typename Routine, typename... Routines>
	struct RoutineContainer
		:
		public RoutineContainerBase<Routine>
	{
		void Initialize(CommAccess* const commAccess)
		{
			routine.process(commAccess);
			next.Initialize(commAccess);
		}

		void Process()
		{
			routine.process();
			next.Process();
		}

		void Update(
			Game::StateFactoryView* const factory, 
			Game::StateManagerView* const manager)
		{
			factory->VisitHandler(&routine, manager);
			next.Update(factory, manager);
		}

	private:
		RoutineContainer<Routines...> next;
	};
}
