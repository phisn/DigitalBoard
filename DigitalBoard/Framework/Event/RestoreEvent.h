#pragma once

#include "../../Framework/Event.h"

namespace Framework
{
	class RestoreEventData
	{
	public:
		RestoreEventData(Game::StateFactoryView* const factory, void* const data)
			:
			factory(factory),
			data(data)
		{
		}

		template <typename DataType = void>
		const DataType* Get(const int stateIndex)
		{
			return factory->GetContextData(stateIndex, data);
		}

	private:
		Game::StateFactoryView* const factory;
		void* const data;
	};

	struct RestoreEventHandlerDefault;
	struct RestoreEventHandler
	{
		typedef RestoreEventHandlerDefault Default;
		constexpr static Event event = Event::Restore;

		virtual bool Ask(RestoreEventData* const data) = 0;
	};

	struct RestoreEventHandlerDefault
		:
		public RestoreEventHandler
	{
		bool Ask(RestoreEventData* const data) override
		{
			return false;
		}
	};

	template <>
	struct EventHandlerDefinition<Event::Restore>
	{
		typedef RestoreEventHandler Handler;
		typedef RestoreEventData Data;
		typedef RestoreEventHandlerDefault DefaultHandler;
	};
}
