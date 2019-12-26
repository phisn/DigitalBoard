#pragma once

namespace Game
{
	enum class Event
	{
		Restore,
		_Length
	};

	struct RestoreEventHandlerDefault;
	struct RestoreEventHandler
	{
		constexpr static Event event = Event::Restore;
		typedef RestoreEventHandlerDefault Default;
	};

	template <typename EventHandler>
	struct _EventHandlerContainerBase
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
				temp = (void*) handler;
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
	class _EventHandlerContainer;

	template <typename EventHandler>
	class _EventHandlerContainer<EventHandler>
		:
		private _EventHandlerContainerBase<EventHandler>
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
		// EventHandlerManager. all handlers will be
		// cleared to prevent accidental double
		// deletion
		inline void Assign(void** eventHandlers)
		{
			eventHandlers[EventHandler::event] = Steal();
		}
	};

	template <typename EventHandler, typename... V>
	class _EventHandlerContainer
		:
		private _EventHandlerContainerBase<EventHandler>
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
		_EventHandlerContainer<V...> next;
	};

	// default implementation for EventHandlerContainer.
	// should normally allways be used
	typedef _EventHandlerContainer<RestoreEventHandler> EventHandlerContainer;

	class EventHandlerManager
	{
	public:
		EventHandlerManager(EventHandlerContainer* const container)
		{
			container->Assign(eventHandlers);
		}

		template <typename EventHandler>
		EventHandler* Get() const
		{
			return (EventHandler*) eventHandlers[(int) EventHandler::event];
		}

	private:
		void* eventHandlers[(int) Event::_Length];
	};
}
