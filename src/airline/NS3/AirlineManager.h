#ifndef	_AIRLINEMANAGER_H_
#define	_AIRLINEMANAGER_H_

#include <common.h>
#include <Config.h>

class AirlineManager
{
private:
	int startNetwork(wf::Config & cfg);
	void getAllNodeLocation(void);
public:
	AirlineManager(wf::Config & cfg);
};

#endif	//_AIRLINEMANAGER_H_
