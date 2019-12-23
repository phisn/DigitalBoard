#include "Game/GameController.h"

enum class CustomGameState
{
	Collect
};

void setup() 
{
	Game::GameController::Initialize<Game::Collector>();
}

void loop() 
{
  
}
