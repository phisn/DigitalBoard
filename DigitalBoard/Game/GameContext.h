#pragma once

#include "../Game/StateManagerView.h"

#pragma pack(push, 1)

namespace Game
{
	struct GameContextView
	{
		virtual GameContextView* FindContext(const int index) = 0;
		virtual int FindIndex(GameContextView* const context) = 0;
		virtual StateManagerView* CreateState(void* const data) = 0;

		StateManagerView* CreateIndexState(const int index, void* const data)
		{
			return CreateIndexState(FindContext(index), data);
		}

		StateManagerView* CreateIndexState(GameContextView* const context, void* const data)
		{
			return context->CreateState(GetContextData(context, data));
		}

		virtual int GetRemainingDataSize() = 0;
		virtual int GetDataSize() = 0;

		virtual bool CanRestore() = 0;
		virtual bool IsFinal() = 0;

		int GetDataOffset(const int index)
		{
			return GetDataOffset(FindContext(index));
		}

		int GetDataOffset(GameContextView* const context)
		{
			return GetRemainingDataSize() - context->GetRemainingDataSize();
		}

		void* GetContextData(
			const int index, 
			void* const data)
		{
			return (void*) ((ptrdiff_t) data + GetDataOffset(index));
		}

		void* GetContextData(
			GameContextView* const context, 
			void* const data)
		{
			return (void*) ((ptrdiff_t) data + GetDataOffset(context));
		}
	};

	template <typename State>
	struct GameContextBase
		:
		public GameContextView
	{
		StateManagerView* CreateState(void* const data) override
		{
			return new State((State::Data*) data);
		}

		int GetDataSize() override
		{
			return sizeof(State::Data);
		}

		bool CanRestore() override
		{
			return State::CanRestore;
		}
	};

	template <typename State, typename... States>
	struct GameContext;

	template <typename State>
	struct GameContext<State>
		:
		public GameContextBase<State>
	{
		typedef State::Data Data;

		GameContextView* FindContext(const int index) override
		{
			if (next != 0)
			{
				// error
			}

			return this;
		}

		int FindIndex(GameContextView* const context) override
		{
			if (context != this)
			{
				// error
			}

			return 0;
		}

		int GetRemainingDataSize() override
		{
			return sizeof(Data);
		}

		bool IsFinal() override
		{
			return true;
		}
	};

	template <typename State, typename... States>
	struct GameContext
		:
		public GameContextBase<State>
	{
		struct Data
			:
			GameContext<States...>::Data
		{
			State::Data data;
		};

		GameContextView* FindContext(const int index) override
		{
			if (index == 0)
			{
				return this;
			}
			else
			{
				return next.FindContext(index - 1);
			}
		}

		int FindIndex(GameContextView* const context) override
		{
			if (context == this)
			{
				return 0;
			}
			else
			{
				return next.FindIndex(context) + 1;
			}
		}

		int GetRemainingDataSize() override
		{
			return sizeof(Data);
		}

		bool IsFinal() override
		{
			return false;
		}

	private:
		GameContext<States...> next = GameContext<States...>;
	};
}

#pragma pack(pop)
