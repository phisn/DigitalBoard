#pragma once

#include "StateFactory.h"

namespace Game
{
	class StateContext
	{
	public:
		StateContext(StateFactoryView* const root, void* data)
			:
			factory(factory),
			data(data)
		{
		}

		~StateContext()
		{
			if (manager)
			{
				delete manager;
			}
		}

		inline void Begin(const int index = 0)
		{
			Begin(factory->FindFactory(index));
		}

		inline void Begin(StateFactoryView* const newFactory)
		{
			data = factory->GetContextData(newFactory, data);
			factory = newFactory;
			manager = factory->CreateState(data);
		}

		inline void Next(const int offset = 1)
		{
			Next(factory->FindFactory(offset));
		}
		
		inline void Next(StateFactoryView* const newFactory)
		{
			delete manager;
			Begin(newFactory);
		}

		inline StateManagerView* operator->() const
		{
			return manager;
		}

		inline StateFactoryView* getFactory() const
		{
			return factory;
		}

		template <typename DataType = void>
		inline DataType* getData() const
		{
			return (DataType*) data;
		}

	private:
		StateFactoryView* factory;
		void* data;
		StateManagerView* manager = NULL;
	};
}
