#include "Framework/Framework.h"

typedef Game::StateContainer<Game::Collector> StateContainer;

class SerialInterface
	:
	public Communication::Interface<StateContainer>
{
public:
	void update(Game::Collector* const state) override
	{
		// print and save state
		// Game::CollectorAccess::GetData();
	}

	void process() override
	{
		// if (state != Game::GameState::Running)
		// {
		//     read and process input
		// }
	}

	void notifyFault(const Device::Fault fault) override
	{
		// print fault to serial
	}

private:
	Game::GameState state;
};

struct CustomRestoreHandler
	:
	Framework::RestoreEventHandler
{
	bool Ask(Framework::RestoreEventData* const data) override
	{
		return true;
	}
} restoreHandler;


Framework::Framework<void>* framework;

void setup() 
{

	/*
	
	-	Config for interfaces. interfacecontainer?
	-	additional configuration for device layer
	-	more events
	
	*/

	Framework::EventConfigurator configurator;
	configurator.SetEventHandler(&restoreHandler);
	framework = new Framework::Framework<void>(&configurator);
}

void loop() 
{
	framework->process();
}
