#define	_AIRLINEMANAGER_CC_

#include "AirlineManager.h"
#include "Airline.h"
#include "Command.h"
#include "mac_stats.h"
#include "Nodeinfo.h"

int AirlineManager::cmd_node_position(uint16_t id, char *buf, int buflen)
{
	int n=0;
	ofstream of;
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 
	if(buf[0]) {
		of.open(buf);
		if(!of.is_open()) {
			return snprintf(buf, buflen, "could not open file %s", buf);
		} else {
			n = snprintf(buf, buflen, "SUCCESS");
		}
	}
	for (NodeContainer::Iterator i = nodes.Begin (); i != nodes.End (); ++i) 
	{ 
		Ptr<Node> node = *i; 
		//std::string name = Names::FindName (node);
		Ptr<MobilityModel> mob = node->GetObject<MobilityModel> (); 
		if (! mob) continue; // Strange, node has no mobility model installed. Skip. 

		Vector pos = mob->GetPosition (); 
		Ptr<LrWpanNetDevice> dev = node->GetDevice(0)->GetObject<LrWpanNetDevice>();
		if(id == 0xffff || id == node->GetId()) {
			if(of.is_open()) {
				of << "Node " << node->GetId() << " Location= " << pos.x << " " << pos.y << " " << pos.z 
					  << "\n"; 
			} else {
				n += snprintf(buf+n, buflen-n, "%d loc= %.2f %.2f %.2f\n", node->GetId(), pos.x, pos.y, pos.z);
				if(n > (buflen-50)) {
					n += snprintf(buf+n, buflen-n, "[TRUNC]");
					break;
				}
			}
		}
	}
	of.close();
	return n;
}

void AirlineManager::setPositionAllocator(NodeContainer & nodes)
{
	MobilityHelper mobility;
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	//TODO: In the future this could support different types of mobility models

	if(CFG("topologyType") == "grid") {
		INFO << "Using GridPositionAllocator\n";
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
				"MinX", DoubleValue(.0),
				"MinY", DoubleValue(.0),
				"DeltaX", DoubleValue(stod(CFG("fieldX"))),
				"DeltaY", DoubleValue(stod(CFG("fieldY"))),
				"GridWidth", UintegerValue(stoi(CFG("gridWidth"))),
				"LayoutType", StringValue("RowFirst"));
	} else if(CFG("topologyType") == "randrect") {
		char x_buf[128], y_buf[128];
		snprintf(x_buf, sizeof(x_buf), "ns3::UniformRandomVariable[Min=0.0|Max=%s]", CFG("fieldX").c_str());
		snprintf(y_buf, sizeof(y_buf), "ns3::UniformRandomVariable[Min=0.0|Max=%s]", CFG("fieldY").c_str());
		INFO << "Using RandomRectanglePositionAllocator\n";
		mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
				"X", StringValue(x_buf),
				"Y", StringValue(y_buf));
	} else {
		ERROR << "Unknown topologyType: " << CFG("topologyType") << " in cfg\n";
		throw FAILURE;
	}
	mobility.Install (nodes);
}

void AirlineManager::msgrecvCallback(msg_buf_t *mbuf)
{
	NodeContainer const & n = NodeContainer::GetGlobal (); 

	if(mbuf->flags & MBUF_IS_CMD) {
		if(0) { } 
		HANDLE_CMD(mbuf, cmd_node_position)	//NS3 Airline specific command
		else {
			al_handle_cmd(mbuf);
		}
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

void AirlineManager::nodePos(NodeContainer & nodes, uint16_t id, double & x, double & y, double & z)
{
	MobilityHelper mob;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (x, y, z));
	mob.SetPositionAllocator (positionAlloc);
	mob.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mob.Install(nodes.Get(id));
}

void AirlineManager::setNodeSpecificPosition(NodeContainer & nodes) 
{
	uint8_t is_set=0;
	double x, y, z;
	wf::Nodeinfo *ni=NULL;
	for(int i=0;i<(int)nodes.GetN();i++) {
		ni=WF_config.get_node_info(i);
		if(!ni) {
			ERROR << "GetN doesnt match nodes stored in config!!\n";
			return;
		}
		ni->getNodePosition(is_set, x, y, z);
		if(!is_set) continue;
		nodePos(nodes, i, x, y, z);
	}
}

int AirlineManager::startNetwork(wf::Config & cfg)
{
	try {
		GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
		GlobalValue::Bind ("SimulatorImplementationType", 
		   StringValue ("ns3::RealtimeSimulatorImpl"));

		wf::Macstats::clear();

		NodeContainer nodes;
		nodes.Create (cfg.getNumberOfNodes());
		INFO << "Creating " << cfg.getNumberOfNodes() << " nodes..\n";
		SeedManager::SetSeed(stoi(CFG("randSeed", "0xbabe"), nullptr, 0));

		setPositionAllocator(nodes);

		setNodeSpecificPosition(nodes);

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

		ScheduleCommlineRX();
		INFO << "NS3 Simulator::Run initiated...\n";
		Simulator::Run ();
		pause();
		Simulator::Destroy ();
	} catch (int e) {
		ERROR << "Configuration failed\n";
		return FAILURE;
	}
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
