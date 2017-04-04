#define _MANAGER_CC_

#include <Manager.h>
#include <Airlink.h>

Manager::Manager(const Config & cfg)
{
	startManager(cfg);
	INFO << "xyz" << endl;
}

int Manager::startManager(const Config & cfg)
{
	Airlink airlink(cfg);
	return SUCCESS;
}

