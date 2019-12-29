#pragma once

#include "../../Communication/Interface.h"

namespace Framework
{
	class InterfaceJoinEventData
	{
	public:

	private:
		Communication::InterfaceView* const interface;
	};

	struct InterfaceJoinHandlerDefault;
	struct InterfaceJoinHandler
	{
		typedef InterfaceJoinHandlerDefault Default;
		constexpr static Event event = Event::Restore;

		virtual bool Ask(InterfaceJoinEventData* const data) = 0;
	};

	struct InterfaceJoinHandlerDefault
		:
		public InterfaceJoinHandler
	{
	};
}
