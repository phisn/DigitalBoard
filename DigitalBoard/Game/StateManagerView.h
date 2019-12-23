#pragma once

namespace Game
{

	// base for statemanagers
	// has to define Data as datastructure
	// has to define CanRestore as bool
	//   if is able to restore
	struct StateManagerView
	{
	public:
		virtual ~StateManagerView()
		{
		}

		// called to creat new state
		// and initialize data
		// cant be called with restore
		// and cant fail
		virtual void Create() = 0;

		// called if state needed to be
		// recreated. used in interrupt
		// for example
		virtual void Recreate() = 0;

		// called once each tick
		// returns if data has changed
		// since last tick
		virtual bool Process() = 0;

		// called on restore. returns if
		// restore is successfull
		virtual bool Restore() = 0;
		
		// called to ask for finish of
		// this state. returns if is currently
		// able or allowed to finsh
		virtual bool Finish() = 0;

		// called when update of representation
		// is needed. update should only ocurr
		// when this function is called. not when
		// process has found a update
		virtual void UpdateRep() = 0;
	};

	// interrupts work similar to StateManagers
	// but do not have to define values or types
	typedef StateManagerView InterruptView;
}
