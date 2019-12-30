#pragma once

#include "../../Communication/Interface.h"
#include "../../Framework/Event.h"

namespace Framework
{
	struct InterfacePushEventData
	{
		InterfacePushEventData(
			Communication::InterfaceID interfaceID,
			Communication::InterfaceView* const interface)
			:
			interfaceID(interfaceID),
			interface(interface)
		{
		}

		Communication::InterfaceID interfaceID;
		Communication::InterfaceView* interface;
	};

	struct InterfacePushHandlerDefault;
	struct InterfacePushHandler
	{
		typedef InterfacePushHandlerDefault Default;
		constexpr static Event event = Event::InterfacePush;

		virtual bool Ask(InterfacePushEventData* const data) = 0;
	};

	struct InterfacePushHandlerDefault
		:
		public InterfacePushHandler
	{
		static_assert(false, "InterfacePush event has to be overwritten");
	};

	template <>
	struct EventHandlerDefinition<Event::InterfacePush>
	{
		typedef InterfacePushHandler Handler;
		typedef InterfacePushEventData Data;
		typedef InterfacePushHandlerDefault DefaultHandler;
	};
}
