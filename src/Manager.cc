#define _MANAGER_CC_

#include <Manager.h>
#include <AirlineManager.h>

Manager::Manager(wf::Config & cfg)
{
	startManager(cfg);
}

int Manager::startManager(wf::Config & cfg)
{
	try {
		AirlineManager airlineManager(cfg);
	} catch (exception & e) {
		ERROR << "Caught exception " << e.what() << endl;
		return FAILURE;
	}
	return SUCCESS;
}

