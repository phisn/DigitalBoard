#pragma once

#include "../Game/StateFactory.h"

namespace Framework
{
	enum class Event
	{
		Restore,
		InterfacePush,
		InterfaceLink,
		InterfaceRemove,
		_Length
	};

	template <Event event>
	struct EventHandlerDefinition
	{
		/*
			Has to define:
			-	Handler type
			-	Data type
			-	DefaultHandler type
		*/

		static_assert(false, "Event not defined");
	};
}
