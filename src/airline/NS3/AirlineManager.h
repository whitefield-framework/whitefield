#ifndef	_AIRLINEMANAGER_H_
#define	_AIRLINEMANAGER_H_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

class AirlineManager
{
private:
	void msgrecvCallback(msg_buf_t *mbuf);
	int startNetwork(wf::Config & cfg);
	void nodePos(NodeContainer & nodes, uint16_t id, double & x, double & y, double & z);
	int cmd_node_position(uint16_t id, char *buf, int buflen);
	void setPositionAllocator(NodeContainer & nodes);
	void setNodeSpecificPosition(NodeContainer & nodes);
	void msgReader(void);
	void ScheduleCommlineRX(void);
	EventId m_sendEvent;
public:
	AirlineManager(wf::Config & cfg);
	~AirlineManager();
};

#endif	//_AIRLINEMANAGER_H_
