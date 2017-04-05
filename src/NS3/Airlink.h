#ifndef	_AIRLINK_H_
#define	_AIRLINK_H_

#include <common.h>
#include <Config.h>

class Airlink
{
private:
	int startNetwork(wf::Config & cfg);
	void getAllNodeLocation(void);
public:
	Airlink(wf::Config & cfg);
};

#endif	//	_AIRLINK_H_
