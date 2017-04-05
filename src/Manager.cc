#define _MANAGER_CC_

#include <Manager.h>
#include <Airlink.h>

Manager::Manager(wf::Config & cfg)
{
	startManager(cfg);
}

int Manager::startManager(wf::Config & cfg)
{
	try {
		Airlink airlink(cfg);
	} catch (exception & e) {
		ERROR << "Caught exception " << e.what() << endl;
		return FAILURE;
	}
	return SUCCESS;
}

