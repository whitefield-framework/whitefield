#define	_AIRLINEMANAGER_CC_

#include <unistd.h>
#include <thread>
#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-module.h"

#include "AirlineManager.h"
#include "Airline.h"
extern "C" {
#include "commline/commline.h"
}

using namespace ns3;

void AirlineManager::getAllNodeLocation(void)
{
	NodeContainer const & n = NodeContainer::GetGlobal (); 
	for (NodeContainer::Iterator i = n.Begin (); i != n.End (); ++i) 
	{ 
		Ptr<Node> node = *i; 
		//std::string name = Names::FindName (node);
		Ptr<MobilityModel> mob = node->GetObject<MobilityModel> (); 
		if (! mob) continue; // Strange, node has no mobility model installed. Skip. 

		Vector pos = mob->GetPosition (); 
		Ptr<LrWpanNetDevice> dev = node->GetDevice(0)->GetObject<LrWpanNetDevice>();
		std::cout << "Node " << node->GetId() << " is at (" << pos.x << ", " << pos.y << ", " << pos.z 
				  << ") shortaddr=" << dev->GetMac()->GetShortAddress()
				  << " ExtAddr:" << dev->GetMac()->GetExtendedAddress()
				  << " PanID:" << dev->GetMac()->GetPanId() 
				  << ")\n"; 
	} 
}

void AirlineManager::commline_thread(void)
{
	uint16_t len;
	uint8_t buf[COMMLINE_MAX_BUF];
	cl_mgr_info_t info;
	int slptime=1;

	INFO << "Commline Thread created\n";
	while(1)
	{
		usleep(slptime);
		slptime=1000;

		memset(&info, 0, sizeof(info));
		len = sizeof(info);
		if(CL_SUCCESS==cl_recvfrom_q(CL_MANAGER_ID, (uint8_t *)&info, &len) && len>0) {
			len = sizeof(buf);
			if(CL_SUCCESS!=cl_recvfrom_q(info.sndr_id, buf, &len)) {
				ERROR << "recv failed " << info.sndr_id << endl;
				break;
			}
			INFO << "Received msg len:" << len << " for id:" << info.sndr_id << endl;
			slptime=1;
		}

#if 0
		{
			NodeContainer const & n = NodeContainer::GetGlobal (); 
			Ptr<Application> nodeApp = n.Get(2)->GetApplication(0);
			if(nodeApp) {
				Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
				aline->rxPacketFromStackline(buf, len);
			}
		}
#endif
	}
}

int AirlineManager::startNetwork(wf::Config & cfg)
{
	if(CL_SUCCESS != cl_init(CL_CREATEQ)) {
		ERROR << "Failure creating commline\n";
		return FAILURE;
	}
	NodeContainer nodes;
	nodes.Create (cfg.getNumberOfNodes());
	INFO << "Creating " << cfg.getNumberOfNodes() << " nodes..\n";
	SeedManager::SetSeed(0xbabe);

	MobilityHelper mobility;
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue(.0),
			"MinY", DoubleValue(.0),
			"DeltaX", DoubleValue(stod(CFG("fieldX"))),
			"DeltaY", DoubleValue(stod(CFG("fieldY"))),
			"GridWidth", UintegerValue(stoi(CFG("deploymentMode"))),
			"LayoutType", StringValue("RowFirst"));

//	INFO << "FieldX: " << stod(cfg.get("fieldX")) << " fieldY: " << stod(cfg.get("fieldY")) << " mode: " << stoi(cfg.get("deploymentMode")) << endl;
	mobility.Install (nodes);

	NS_LOG_INFO ("Create channels.");
	LrWpanHelper lrWpanHelper;
	NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
	lrWpanHelper.AssociateToPan (devContainer, CFG_PANID);

	INFO << CFG("captureFileAll") << endl;
	lrWpanHelper.EnablePcapAll (string(CFG("NS3_captureFile")), true);

	AirlineHelper airlineApp;
	ApplicationContainer apps = airlineApp.Install(nodes);
	apps.Start(Seconds(0.0));

	getAllNodeLocation();
	thread t1(commline_thread);
	t1.detach();
	Simulator::Run ();
	sleep(1);
	Simulator::Destroy ();
	INFO << "Execution done\n";
	return SUCCESS;
}

AirlineManager::AirlineManager(wf::Config & cfg)
{
	startNetwork(cfg);
	INFO << "AirlineManager started" << endl;
}

AirlineManager::~AirlineManager() 
{
	cl_cleanup();
}
