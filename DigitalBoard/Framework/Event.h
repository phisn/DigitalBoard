#pragma once

#include "../Game/StateFactory.h"

namespace Framework
{
	enum class Event
	{
		Restore,
		_Length
	};

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

	template <typename EventHandler>
	struct _EventConfiguratorBase
	{
		EventHandler* handler = NULL;

		// steals handler if possible and creates
		// a default one if current is null. used
		// to withdraw values and prevent double
		// deletion
		void* Steal()
		{
			void* temp;

			if (handler)
			{
				temp = (void*)handler;
				handler = NULL;
			}
			else
			{
				temp = (void*) new EventHandler::Default{ };
			}

			return temp;
		}
	};

	template <typename EventHandler, typename... V>
	class _EventConfigurator;

	template <typename EventHandler>
	class _EventConfigurator<EventHandler>
		:
		private _EventConfiguratorBase<EventHandler>
	{
	public:
		template <typename OtherHandler>
		// event handlers should be created dynamically.
		// should not set same eventhandler twice. old
		// event handler is lost and the memory leaked
		inline void SetEventHandler(OtherHandler* const handler)
		{
			if (std::is_same<OtherHandler, EventHandler>::value)
			{
				this->handler = handler;
			}
			else
			{
				// error
			}
		}

		// assign should not be called outside of 
		// EventConfiguration. all handlers will be
		// cleared to prevent accidental double
		// deletion
		inline void Assign(void** eventHandlers)
		{
			eventHandlers[EventHandler::event] = Steal();
		}
	};

	template <typename EventHandler, typename... V>
	class _EventConfigurator
		:
		private _EventConfiguratorBase<EventHandler>
	{
	public:
		template <typename OtherHandler>
		// ^^^
		inline void SetEventHandler(OtherHandler* const handler)
		{
			if (std::is_same<OtherHandler, EventHandler>::value)
			{
				this->handler = handler;
			}
			else
			{
				next.SetEventHandler(handler);
			}
		}

		// ^^^
		void Assign(void** eventHandlers)
		{
			eventHandlers[EventHandler::event] = Steal();
			next.Assign(eventHandlers);
		}

	private:
		_EventConfigurator<V...> next;
	};

	// default implementation for EventConfigurator.
	// should normally allways be used
	typedef _EventConfigurator<RestoreEventHandler> EventConfigurator;

	class EventConfiguration
	{
	public:
		EventConfiguration(EventConfigurator* const configurator)
		{
			configurator->Assign(eventHandlers);
		}

		template <typename EventHandler>
		EventHandler* GetHandler() const
		{
			return (EventHandler*)eventHandlers[(int) EventHandler::event];
		}

	private:
		void* eventHandlers[(int)Event::_Length];
	};
}
