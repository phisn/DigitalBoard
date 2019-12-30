#pragma once

#include "../../Communication/Interface.h"
#include "../../Framework/Event.h"

namespace Framework
{
	struct InterfaceLinkEventData
	{
		Communication::InterfaceID interfaceID;
		Communication::InterfaceView* interface;
	};

	struct InterfaceLinkHandlerDefault;
	struct InterfaceLinkHandler
	{
		typedef InterfaceLinkHandlerDefault Default;
		constexpr static Event event = Event::InterfaceLink;

		virtual bool Ask(InterfaceLinkEventData* const data) = 0;
	};

	struct InterfaceLinkHandlerDefault
		:
		public InterfaceLinkHandler
	{
		static_assert(false, "InterfaceLink event has to be overwritten");
	};

	template <>
	struct EventHandlerDefinition<Event::InterfacePush>
	{
		typedef InterfaceLinkHandler Handler;
		typedef InterfaceLinkEventData Data;
		typedef InterfaceLinkHandlerDefault DefaultHandler;
	};
}
