#include "Framework/Framework.h"

typedef Game::StateContainer<Game::Collector> StateContainer;
typedef Framework::FrameworkConfiguration<StateContainer> FrameworkConfig;

struct CustomRestoreHandler
	:
	Framework::RestoreEventHandler
{
	bool Ask(Framework::RestoreEventData* const data) override
	{
		return true;
	}
} restoreHandler;


Framework::Framework<FrameworkConfig>* framework;

void setup() 
{
	/*
	
	-	Config for interfaces. interfacecontainer?
	-	additional configuration for device layer
	-	more events
	
	*/

	Framework::EventConfigurator configurator;
	configurator.SetEventHandler(&restoreHandler);
	framework = new Framework::Framework<FrameworkConfig>(&configurator);
}

void loop() 
{
	framework->process();
}
