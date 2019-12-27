#pragma once

namespace Game
{
	typedef unsigned char GameState;

	struct GameStateRaw
	{
		enum : GameState
		{
			// zero for fresh start / clean
			Shutdown = 0,
			Prerunning,
			Error,

			_Length // real state offset
		};
	};

	struct GameSector
	{
		unsigned char state;
	};
}
