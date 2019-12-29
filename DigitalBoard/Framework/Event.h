#pragma once

#include "../Game/StateFactory.h"

namespace Framework
{
	enum class Event
	{
		Restore,
		InterfaceJoin,
		_Length
	};

	struct EventConfiguratorView
	{
		virtual void Assign(void** eventHandlers) = 0;
	};

	template <typename EventHandler>
	struct EventConfiguratorBase
		:
		public EventConfiguratorView
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
	class EventConfigurator;

	template <typename EventHandler>
	class EventConfigurator<EventHandler>
		:
		private EventConfiguratorBase<EventHandler>
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
		void Assign(void** eventHandlers) override
		{
			eventHandlers[EventHandler::event] = Steal();
		}
	};

	template <typename EventHandler, typename... V>
	class EventConfigurator
		:
		private EventConfiguratorBase<EventHandler>
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
		void Assign(void** eventHandlers) override
		{
			eventHandlers[EventHandler::event] = Steal();
			next.Assign(eventHandlers);
		}

	private:
		EventConfigurator<V...> next;
	};

	class EventConfiguration
	{
	public:
		EventConfiguration(EventConfiguratorView* const configurator)
		{
			configurator->Assign(eventHandlers);
		}

		template <typename EventHandler>
		EventHandler* GetHandler() const
		{
			return (EventHandler*) eventHandlers[(int) EventHandler::event];
		}

	private:
		void* eventHandlers[(int)Event::_Length];
	};
}
