#pragma once

#include "../Framework/Event.h"

#include "../Framework/Event/InterfaceJoinEvent.h"
#include "../Framework/Event/RestoreEvent.h"

namespace Framework
{
	struct EventConfiguratorView
	{
		virtual void Assign(void** eventHandlers) = 0;
	};

	template <int event>
	struct _EventConfiguratorBase
		:
		public EventConfiguratorView
	{
		typedef EventHandlerDefinition<(Event) event> Storage;

		Storage::Handler* handler = NULL;

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

	template <int event>
	class _EventConfigurator;

	template <>
	class _EventConfigurator<0>
		:
		private _EventConfiguratorBase<0>
	{
	public:
		template <typename OtherHandler>
		// event handlers should be created dynamically.
		// should not set same eventhandler twice. old
		// event handler is lost and the memory leaked
		inline void SetEventHandler(OtherHandler* const handler)
		{
			if (std::is_same<OtherHandler, Storage::Handler>::value)
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
			eventHandlers[0] = Steal();
		}
	};

	template <int event>
	class _EventConfigurator
		:
		private _EventConfiguratorBase<event>
	{
	public:
		template <typename OtherHandler>
		// ^^^
		inline void SetEventHandler(OtherHandler* const handler)
		{
			if (std::is_same<OtherHandler, Storage::Handler>::value)
			{
				this->handler = (Storage::Handler*) handler;
			}
			else
			{
				next.SetEventHandler(handler);
			}
		}

		// ^^^
		void Assign(void** eventHandlers) override
		{
			eventHandlers[event] = Steal();
			next.Assign(eventHandlers);
		}

	private:
		_EventConfigurator<event - 1> next;
	};

	typedef _EventConfigurator<(int) Event::_Length - 1> EventConfigurator;

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
			return (EventHandler*)eventHandlers[(int) EventHandler::event];
		}

	private:
		void* eventHandlers[(int)Event::_Length];
	};
}
