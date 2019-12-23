#pragma once

namespace Game
{
	struct GameState
	{
		enum : unsigned char
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

	/*namespace Controller
	{
		GameState GetState();
	}*/
}
