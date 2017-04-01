#define _MANAGER_CC_

#include <Manager.h>

Manager::Manager(const Config & cfg)
{
	startManager(cfg);
	INFO << "xyz" << endl;
}

int Manager::startManager(const Config & cfg)
{
	return SUCCESS;
}

