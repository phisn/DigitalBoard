#pragma once

namespace Device
{
	namespace GameManager
	{
		enum class GameState
		{
			Collect,
			Setup,
			Running

		};

		void Initialize();
		void Uninitialize();

		void Process();

		void SetCurrentState(const GameState);
	}
}