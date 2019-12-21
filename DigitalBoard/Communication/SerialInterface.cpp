#include "SerialInterface.h"
#include "../Communication/InterfaceManager.h"

char Communication::SerialInterface::playerCharacterCounter = 'a';

void Communication::SerialInterface::UpdateCollect()
{
	const int size = Serial.available();

	if (size != 1)
	{
		while (Serial.available())
			Serial.read();

		return;
	}

	switch (Serial.read())
	{
	case Join:
		Serial.println("Creating interface");
		::Communication::InterfaceManager::CreatePushedInterface<SerialInterface>();

		break;
	case Start:
		Serial.println("Starting game");
		::Game::Controller::RequestFinish();

		break;
	}
}
