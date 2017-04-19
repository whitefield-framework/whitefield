#ifndef	_AIRLINEMANAGER_H_
#define	_AIRLINEMANAGER_H_

#include <common.h>
#include <Config.h>

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

class AirlineManager
{
private:
	int startNetwork(wf::Config & cfg);
	void getAllNodeInfo(void);
	void setMobilityModel(MobilityHelper & mobility);
	void msgReader(void);
	void ScheduleCommlineRX(void);
	EventId m_sendEvent;
public:
	AirlineManager(wf::Config & cfg);
	~AirlineManager();
};

#endif	//_AIRLINEMANAGER_H_
