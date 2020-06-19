/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       NS3 specific airline module for managing virtual node set.
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_AIRLINEMANAGER_CC_

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "AirlineManager.h"
#include "Airline.h"
#include "Command.h"
#include "mac_stats.h"
#include "Nodeinfo.h"
#if PLC
#include "PowerLineCommHandler.h"
#endif

int getNodeConfigVal(int id, char *key, char *val, int vallen)
{
	wf::Nodeinfo *ni=NULL;
    ni=WF_config.get_node_info(id);
    if (!ni) {
        snprintf(val, vallen, "cudnot get nodeinfo id=%d", id);
        ERROR << "Unable to get node config\n";
        return 0;
    }
    if (!strcmp(key, "nodeexec")) {
        return snprintf(val, vallen, "%s", (char *)ni->getNodeExecutable().c_str());
    }
    snprintf(val, vallen, "unknown key [%s]", key);
    ERROR << "Unknown key " << key << "\n";
    return 0;
}

int AirlineManager::cmd_node_exec(uint16_t id, char *buf, int buflen)
{
    return getNodeConfigVal(id, (char *)"nodeexec", buf, buflen);
}

int AirlineManager::cmd_node_position(uint16_t id, char *buf, int buflen)
{
	int n=0;
	ofstream of;
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 
	if(buf[0]) {
		of.open(buf);
		if(!of.is_open()) {
            char tmpbuf[256];
            snprintf(tmpbuf, sizeof(tmpbuf), "%s", buf);
			return snprintf(buf, buflen, "could not open file %s", tmpbuf);
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
				of << "Node " << node->GetId() << " Location= "
                   << pos.x << " " << pos.y << " " << pos.z << "\n"; 
			} else {
				n += snprintf(buf+n, buflen-n, "%d loc= %.2f %.2f %.2f\n",
                        node->GetId(), pos.x, pos.y, pos.z);
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
		int gw=stoi(CFG("gridWidth"));
		INFO << "Using GridPositionAllocator\n";
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
				"MinX", DoubleValue(.0),
				"MinY", DoubleValue(.0),
				"DeltaX", DoubleValue(stod(CFG("fieldX"))/gw),
				"DeltaY", DoubleValue(stod(CFG("fieldY"))/gw),
				"GridWidth", UintegerValue(gw),
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

int AirlineManager::cmd_802154_set_short_addr(uint16_t id, char *buf, int buflen)
{
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 

	Ptr<Application> nodeApp = nodes.Get(id)->GetApplication(0);
	if(!nodeApp) {
		ERROR << "Could not handle msg_buf_t for node " << (int)id << endl;
		return snprintf(buf, buflen, "could not get node app");
	}
	Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
    INFO << "id " << id << " Got a cmd to set SHORT ADDRESS " << buf << "\n";
	aline->setShortAddress(atoi(buf));
    return snprintf(buf, buflen, "SUCCESS");
}

int AirlineManager::cmd_802154_set_ext_addr(uint16_t id, char *buf, int buflen)
{
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 

	Ptr<Application> nodeApp = nodes.Get(id)->GetApplication(0);
	if(!nodeApp) {
		ERROR << "Could not handle msg_buf_t for node " << (int)id << endl;
		return snprintf(buf, buflen, "could not get node app");
	}
	Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
    INFO << "id " << id << " Got a cmd to set EXT ADDRESS " << buf << "\n";
	aline->setExtendedAddress(buf);
    return snprintf(buf, buflen, "SUCCESS");
}

int AirlineManager::cmd_802154_set_panid(uint16_t id, char *buf, int buflen)
{
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 

	Ptr<Application> nodeApp = nodes.Get(id)->GetApplication(0);
	if(!nodeApp) {
		ERROR << "Could not handle msg_buf_t for node " << (int)id << endl;
		return snprintf(buf, buflen, "could not get node app");
	}
	Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
    INFO << "id " << id << " Got a cmd to set PANID " << buf << "\n";
	aline->setPanID(atoi(buf));
    return snprintf(buf, buflen, "SUCCESS");
}

int AirlineManager::cmd_set_node_position(uint16_t id, char *buf, int buflen)
{
	char *ptr, *saveptr;
	double x, y, z=0;
	NodeContainer const & nodes = NodeContainer::GetGlobal (); 
	int numNodes = stoi(CFG("numOfNodes"));

	if(!IN_RANGE(id, 0, numNodes)) {
		return snprintf(buf, buflen,
                "NodeID mandatory for setting node pos id=%d", id);
	}
	ptr = strtok_r(buf, " ", &saveptr);
	if(!ptr) return snprintf(buf, buflen, "invalid loc format! No x pos!");
	x=stod(ptr);
	ptr = strtok_r(NULL, " ", &saveptr);
	if(!ptr) return snprintf(buf, buflen, "invalid loc format! No y pos!");
	y=stod(ptr);
	ptr = strtok_r(NULL, " ", &saveptr);
	if(ptr) z = stod(ptr);

	Ptr<MobilityModel> mob = nodes.Get(id)->GetObject<MobilityModel>();
	Vector m_position = mob->GetPosition();
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	mob->SetPosition(m_position);
	return snprintf(buf, buflen, "SUCCESS");
}

void AirlineManager::msgrecvCallback(msg_buf_t *mbuf)
{
	NodeContainer const & n = NodeContainer::GetGlobal (); 
	int numNodes = stoi(CFG("numOfNodes"));

	if(mbuf->flags & MBUF_IS_CMD) {
        if(0) {}
		HANDLE_CMD(mbuf, cmd_node_exec)
		HANDLE_CMD(mbuf, cmd_node_position)
		HANDLE_CMD(mbuf, cmd_set_node_position)
		HANDLE_CMD(mbuf, cmd_802154_set_short_addr)
		HANDLE_CMD(mbuf, cmd_802154_set_ext_addr)	
		HANDLE_CMD(mbuf, cmd_802154_set_panid)	
		else {
			al_handle_cmd(mbuf);
		}
        if(!(mbuf->flags & MBUF_DO_NOT_RESPOND)) {
            cl_sendto_q(MTYPE(MONITOR, CL_MGR_ID), mbuf, mbuf->len+sizeof(msg_buf_t));
        }
		return;
	}
	if(!IN_RANGE(mbuf->src_id, 0, numNodes)) {
        ERROR << "rcvd src id=" << mbuf->src_id << " out of range!!\n";
		return;
	}
    if(mbuf->dst_id == CL_DSTID_MACHDR_PRESENT) {
        if(CFG_INT("macHeaderAdd", 1)) {
            ERROR << "rcvd a packet from stackline with DSTID_MACHDR_PRESENT set but config file does not have macHeaderAdd=0\n";
            ERROR << "If you are using openthread, please set macHeaderAdd=0 to prevent Airline from adding its own mac hdr\n";
            return;
        }
    }
	Ptr<Application> nodeApp = n.Get(mbuf->src_id)->GetApplication(0);
	if(!nodeApp) {
		ERROR << "Could not handle msg_buf_t for node " << (int)mbuf->src_id << endl;
		return;
	}
	Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
	aline->tx(mbuf);
}

void AirlineManager::nodePos(NodeContainer const & nodes, uint16_t id, double & x, double & y, double & z)
{
	MobilityHelper mob;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (x, y, z));
	mob.SetPositionAllocator (positionAlloc);
	mob.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mob.Install(nodes.Get(id));
}

Ptr <PropagationLossModel> getLogDistancePLM(map<string, string, ci_less> & m)
{
    Ptr <LogDistancePropagationLossModel> plm =
        CreateObject<LogDistancePropagationLossModel> ();

    if (!plm) {
        ERROR << "Cud not get Log Distance prop loss model\n";
        return plm;
    }
    if (!m["PathLossExp"].empty()) {
        INFO << "using PathLossExp=" << m["PathLossExp"] << "\n";
        plm->SetPathLossExponent(stod(m["PathLossExp"]));
        m.erase("PathLossExp");
    }
    if (!m["refDist"].empty() && !m["refLoss"].empty()) {
        INFO << "using refDist=" << m["refDist"]
             << "refLoss=" << m["refLoss"] << "\n";
        plm->SetReference(stod(m["refDist"]), stod(m["refLoss"]));
        m.erase("refDist");
        m.erase("refLoss");
    }
    return plm;
}

Ptr <PropagationLossModel> getFriisPLM(map<string, string, ci_less> & m)
{
    Ptr <FriisPropagationLossModel> plm =
        CreateObject<FriisPropagationLossModel> ();

    if (!plm) {
        ERROR << "Cud not get Friis prop loss model\n";
        return plm;
    }
    if (!m["freq"].empty()) {
        INFO << "using freq=" << m["freq"] << "\n";
        plm->SetFrequency(stod(m["freq"]));
        m.erase("freq");
    }
    if (!m["minloss"].empty()) {
        INFO << "using minloss=" << m["minloss"] << "\n";
        plm->SetMinLoss(stod(m["minloss"]));
        m.erase("minloss");
    }
    if (!m["sysloss"].empty()) {
        INFO << "using sysloss=" << m["sysloss"] << "\n";
        plm->SetSystemLoss(stod(m["sysloss"]));
        m.erase("sysloss");
    }
    return plm;
}

/* Set Loss and Delay Propagation Model */
int getLossModel(string loss_model, Ptr <PropagationLossModel> & plm)
{
    string cfg = CFG("lossModelParam");
    auto cfgmap = splitKV(cfg);

    INFO << "Using loss model [" << loss_model << "]\n";
    if (stricmp(loss_model, "LogDistance") == 0) {
        plm = getLogDistancePLM(cfgmap);
    } else if (stricmp(loss_model, "Friis") == 0) {
        plm = getFriisPLM(cfgmap);
        plm = CreateObject<FriisPropagationLossModel> ();
    } else {
        ERROR << "Unknown loss model [" << loss_model << "]\n";
        return FAILURE;
    }
    if (cfgmap.size() != 0) {
        map<string, string, ci_less>::iterator i;
        for (i = cfgmap.begin(); i != cfgmap.end(); ++i) {
            ERROR << "Unprocessed loss model param: " 
                  << i->first << "=" << i->second << "\n";
        }
    }
    return SUCCESS;
}

void AirlineManager::setNodeSpecificParam(NodeContainer & nodes)
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
		if(is_set) {
    		nodePos(nodes, i, x, y, z);
        }
		if(ni->getPromisMode()) {
            INFO << "Set promiscuous mode for node:" << i << "\n";
            Ptr<Node> node = nodes.Get(i); 
            Ptr<LrWpanNetDevice> dev = node->GetDevice(0)->GetObject<LrWpanNetDevice>();
            if (dev) {
                INFO << "in Set promiscuous mode for node:" << i << "\n";
                dev->GetMac()->SetPromiscuousMode(1);
            }
        }
	}
}

int AirlineManager::setAllNodesParam(NodeContainer & nodes)
{
    Ptr<SingleModelSpectrumChannel> channel;
    static Ptr <PropagationLossModel> plm;
    string loss_model = CFG("lossModel");
    bool macAdd = CFG_INT("macHeaderAdd", 1);

    if (!loss_model.empty()) {
        channel = CreateObject<SingleModelSpectrumChannel> ();
        if (!channel || getLossModel(loss_model, plm) != SUCCESS) {
            return FAILURE;
        }
        channel->AddPropagationLossModel(plm);
    }

	for (NodeContainer::Iterator i = nodes.Begin (); i != nodes.End (); ++i) 
	{ 
		Ptr<Node> node = *i; 
		Ptr<LrWpanNetDevice> dev = node->GetDevice(0)->GetObject<LrWpanNetDevice>();
        if (!dev) {
            ERROR << "Coud not get device\n";
            continue;
        }
        if(!macAdd) {
            dev->GetMac()->SetMacHeaderAdd(macAdd);

            //In case where stackline itself add mac header, the airline needs
            //to be set in promiscuous mode so that all the packets with
            //headers are transmitted as is to the stackline on reception
            //dev->GetMac()->SetPromiscuousMode(1);
        }
        if (!loss_model.empty()) {
            dev->SetChannel (channel);
        }
	}
    return SUCCESS;
}

int AirlineManager::phyInstall(NodeContainer & nodes)
{
    string phy = CFG("PHY");

    if (stricmp(phy, "plc") == 0) {
#if PLC
        INFO << "Using PLC as PHY\n";
        if (plcInstall(nodes) != SUCCESS) {
            return FAILURE;
        }
#else
        ERROR << "PLC phy is not enabled in NS3\n";
        return FAILURE;
#endif
    } else {
        static LrWpanHelper lrWpanHelper;
        static NetDeviceContainer devContainer = lrWpanHelper.Install(nodes);
        lrWpanHelper.AssociateToPan (devContainer, CFG_PANID);

        INFO << "Using lr-wpan as PHY\n";
        string ns3_capfile = CFG("NS3_captureFile");
        if(!ns3_capfile.empty()) {
            INFO << "NS3 Capture File:" << ns3_capfile << endl;
            lrWpanHelper.EnablePcapAll (ns3_capfile, false /*promiscuous*/);
        }
    }

    return setAllNodesParam(nodes);
}

int AirlineManager::startNetwork(wf::Config & cfg)
{
	try {
		GlobalValue::Bind ("ChecksumEnabled", 
            BooleanValue (CFG_INT("macChecksumEnabled", 1)));
		GlobalValue::Bind ("SimulatorImplementationType", 
		   StringValue ("ns3::RealtimeSimulatorImpl"));

		wf::Macstats::clear();

		NodeContainer nodes;
		nodes.Create (cfg.getNumberOfNodes());
		INFO << "Creating " << cfg.getNumberOfNodes() << " nodes..\n";
		SeedManager::SetSeed(stoi(CFG("randSeed", "0xbabe"), nullptr, 0));

		setPositionAllocator(nodes);

        if (phyInstall(nodes) != SUCCESS) {
            return FAILURE;
        }

		setNodeSpecificParam(nodes);

		AirlineHelper airlineApp;
		ApplicationContainer apps = airlineApp.Install(nodes);
		apps.Start(Seconds(0.0));

		ScheduleCommlineRX();
		INFO << "NS3 Simulator::Run initiated...\n";
        fflush(stdout);
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
		cl_recvfrom_q(MTYPE(AIRLINE,CL_MGR_ID), mbuf, sizeof(mbuf_buf), CL_FLAG_NOWAIT);
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
