#define	_AIRLINK_CC_

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-module.h"

#include "Airlink.h"

using namespace ns3;

int Airlink::startNetwork(wf::Config & cfg)
{
	NodeContainer nodes;
	nodes.Create (cfg.getNumberOfNodes());
	INFO << "Creating " << cfg.getNumberOfNodes() << " nodes..\n";
	SeedManager::SetSeed (0xabcd);

	MobilityHelper mobility;
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

	Ptr<ListPositionAllocator> nodesPositionAlloc = CreateObject<ListPositionAllocator> ();
	nodesPositionAlloc->Add (Vector (0.0, 0.0, 0.0));
	nodesPositionAlloc->Add (Vector (50.0, 0.0, 0.0));
	mobility.SetPositionAllocator (nodesPositionAlloc);
	mobility.Install (nodes);

	NS_LOG_INFO ("Create channels.");
	LrWpanHelper lrWpanHelper;
	NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
	lrWpanHelper.AssociateToPan (devContainer, 0xabcd);

	return SUCCESS;
}

Airlink::Airlink(wf::Config & cfg)
{
	startNetwork(cfg);
	INFO << "Airlink started" << endl;
}
