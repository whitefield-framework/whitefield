#define	_AIRLINEMANAGER_CC_

#include "AirlineManager.h"
#include "Airline.h"
#include "Command.h"
#include "mac_stats.h"

void AirlineManager::getAllNodeInfo(void)
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
				  //<< " ExtAddr:" << dev->GetMac()->GetExtendedAddress()
				  << " PanID:" << dev->GetMac()->GetPanId() 
				  << "\n"; 
	} 
}

void AirlineManager::setMobilityModel(MobilityHelper & mobility)
{
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue(.0),
			"MinY", DoubleValue(.0),
			"DeltaX", DoubleValue(stod(CFG("fieldX"))),
			"DeltaY", DoubleValue(stod(CFG("fieldY"))),
			"GridWidth", UintegerValue(stoi(CFG("deploymentMode"))),
			"LayoutType", StringValue("RowFirst"));

	//TODO: In the future this could support different types of mobility models
}

void msgrecvCallback(msg_buf_t *mbuf)
{
	NodeContainer const & n = NodeContainer::GetGlobal (); 

	if(mbuf->flags & MBUF_IS_CMD) {
		al_handle_cmd(mbuf);
		cl_sendto_q(MTYPE(MONITOR, CL_MGR_ID), mbuf, mbuf->len+sizeof(msg_buf_t));
		return;
	}
	Ptr<Application> nodeApp = n.Get(mbuf->src_id)->GetApplication(0);
	if(!nodeApp) {
		ERROR << "Could not handle msg_buf_t for node " << (int)mbuf->src_id << endl;
		return;
	}
	Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
	aline->tx(mbuf);
}

int AirlineManager::startNetwork(wf::Config & cfg)
{
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	GlobalValue::Bind ("SimulatorImplementationType", 
	   StringValue ("ns3::RealtimeSimulatorImpl"));

	wf::Macstats::clear();

	NodeContainer nodes;
	nodes.Create (cfg.getNumberOfNodes());
	INFO << "Creating " << cfg.getNumberOfNodes() << " nodes..\n";
	SeedManager::SetSeed(0xbabe);

	MobilityHelper mobility;
	setMobilityModel(mobility);
	mobility.Install (nodes);

	LrWpanHelper lrWpanHelper;
	NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
	lrWpanHelper.AssociateToPan (devContainer, CFG_PANID);

	string ns3_capfile = CFG("NS3_captureFile");
	if(!ns3_capfile.empty()) {
		INFO << "NS3 Capture File:" << ns3_capfile << endl;
		lrWpanHelper.EnablePcapAll (ns3_capfile, true);
	}

	AirlineHelper airlineApp;
	ApplicationContainer apps = airlineApp.Install(nodes);
	apps.Start(Seconds(0.0));

//	getAllNodeInfo();
	ScheduleCommlineRX();
	INFO << "NS3 Simulator::Run initiated...\n";
	Simulator::Run ();
	pause();
	Simulator::Destroy ();
	return SUCCESS;
}

void AirlineManager::ScheduleCommlineRX(void)
{
	m_sendEvent = Simulator::Schedule (Seconds(0.001), &AirlineManager::msgReader, this);
}

void AirlineManager::msgReader(void)
{
	DEFINE_MBUF(mbuf);
	while(1) {
		cl_recvfrom_q(MTYPE(AIRLINE,CL_MGR_ID), mbuf, sizeof(mbuf_buf));
		if(mbuf->len) {
			msgrecvCallback(mbuf);
			usleep(1);
		} else {
			break;
		}
	}
	ScheduleCommlineRX();
}

AirlineManager::AirlineManager(wf::Config & cfg)
{
	m_sendEvent = EventId ();
	startNetwork(cfg);
	INFO << "AirlineManager started" << endl;
}

AirlineManager::~AirlineManager() 
{
	Simulator::Cancel (m_sendEvent);
}
