#pragma once

#include "../../Communication/Interface.h"
#include "../../Framework/Event.h"

namespace Framework
{
	struct InterfaceRemoveEventData
	{
		Communication::InterfaceID interfaceID;
		Communication::InterfaceView* interface;
	};

	struct InterfaceRemoveHandlerDefault;
	struct InterfaceRemoveHandler
	{
		typedef InterfaceRemoveHandlerDefault Default;
		constexpr static Event event = Event::InterfaceRemove;

		virtual void Notify(InterfaceRemoveEventData* const data) = 0;
	};

	struct InterfaceRemoveHandlerDefault
		:
		public InterfaceRemoveHandler
	{
		// static_assert(false, "InterfaceRemove event has to be overwritten");
	};

	template <>
	struct EventHandlerDefinition<Event::InterfacePush>
	{
		typedef InterfaceRemoveHandler Handler;
		typedef InterfaceRemoveEventData Data;
		typedef InterfaceRemoveHandlerDefault DefaultHandler;
	};
}
