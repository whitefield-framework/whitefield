/*
 * Copyright (C) 2020 Rahul Jadhav <nyrahul@gmail.com>
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
 * @brief       NS3 PLC Airline Handler
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */
#if PLC
#define	_POWERLINECOMMHANDLER_CC_

#include <stdlib.h> // for atof()
#include <arpa/inet.h> // for htons()

#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/mobility-module.h>
#include <ns3/plc.h>

#include "Nodeinfo.h"
#include "Config.h"
#include "PowerLineCommHandler.h"
#include "IfaceHandler.h"

static PLC_SpectrumModelHelper g_smHelper;
static PLC_NetdeviceMap g_devMap;

Ptr<const SpectrumModel>  plcGetSpectrumModel(string str)
{
    Ptr<const SpectrumModel> sm = NULL;

    if (str.compare("narrowband") == 0)
        sm = g_smHelper.GetSpectrumModel(0, 500e3, 5);
    else if(str.empty())
        sm = g_smHelper.GetSpectrumModel(0, 10e6, 100);
    else if(str.compare("G3") == 0)
        sm = g_smHelper.GetG3SpectrumModel();
    else {
        ERROR << "Unknown PLC Spectrum Model " << str << "\n";
    }
    return sm;
}

Ptr<const SpectrumModel> plcGetSpectrumModel(void)
{
    return g_smHelper.GetSpectrumModel();
}

int plcGetCable(string cableStr, Ptr<PLC_Cable> & cable)
{
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();

    if (cableStr.compare("AL3x95XLPE") == 0)
        cable = CreateObject<PLC_AL3x95XLPE_Cable> (sm);
    else if (cableStr.compare("NYCY70SM35") == 0)
        cable = CreateObject<PLC_NYCY70SM35_Cable> (sm);
    else if (cableStr.compare("NAYY50SE") == 0)
        cable = CreateObject<PLC_NAYY50SE_Cable> (sm);
    else if (cableStr.compare("MV_Overhead") == 0)
        cable = CreateObject<PLC_MV_Overhead_Cable> (sm);
    else if (cableStr.compare("NAYY150SE") == 0)
        cable = CreateObject<PLC_NAYY150SE_Cable> (sm);
    else if(cableStr.empty())
        cable = CreateObject<PLC_NAYY150SE_Cable> (sm);
    else {
        ERROR << "Unknown PLC Cable " << cableStr << "\n";
        return FAILURE;
    }
    return SUCCESS;
}

int plcConnectLink(ifaceCtx_t *ctx, uint16_t i1, uint16_t i2, string cableStr)
{
    Ptr<PLC_Node> n1 = ctx->plcNodes[i1];
    Ptr<PLC_Node> n2 = ctx->plcNodes[i2];
    Ptr<PLC_Cable> cable;

    if (!n1 || !n2) {
        ERROR << "Cudnot get nodes. Check plc_link nodeid\n";
        return FAILURE;
    }

    INFO << i1 << "<->" << i2 << " cable=" << cableStr << "\n";
    if (plcGetCable(cableStr, cable) != SUCCESS) {
        ERROR << "Cud not get PLC Cable\n";
        return FAILURE;
    }
    CreateObject<PLC_Line> (cable, n1, n2);
    return SUCCESS;
}

int getNodePosition(ifaceCtx_t *ctx, uint16_t id, Vector & pos)
{
    Ptr<Node> n = ctx->nodes.Get(id);
    Ptr<MobilityModel> mob = n ? n->GetObject<MobilityModel> () : NULL;

    if (!n || !mob) {
        ERROR << "Cudnot get node/mobmodel at id=" << id << "\n";
        return FAILURE;
    }
    pos = mob->GetPosition (); 
    return SUCCESS;
}

Mac48Address getMacAddress(uint16_t id)
{
    uint8_t idbuf[2];
    char buf[64];

    id = htons(id);
    memcpy(idbuf, &id, 2);
    snprintf(buf, sizeof(buf), "00:00:00:00:%02x:%02x", *idbuf, *(idbuf+1));
    return Mac48Address(buf);
}

Ptr<SpectrumValue> getTxPsd(void)
{
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();
    Ptr<SpectrumValue> txPsd;
    string txPsdStr;

    txPsd  = Create<SpectrumValue> (sm);
	txPsdStr = CFG("plc_tx_psd");
    if (txPsdStr.empty()) {
        txPsdStr = "1e-8";
    }
    *txPsd = stod(txPsdStr);
    return txPsd;
}

/*
 * PLC Node has an outlet which has an interface.
 */
int plcAddNode(ifaceCtx_t *ctx, uint16_t id)
{
    Vector pos;
    Ptr<PLC_Node> n;
#if 0
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();
    Ptr<PLC_InformationRatePhy> phy;
    Ptr<SpectrumValue> txPsd = getTxPsd();
    Ptr<PLC_Outlet> outlet;
    Ptr<SpectrumValue> noiseFloor;
    Ptr<PLC_ArqMac> mac;
#endif
    char name[32];

    if (getNodePosition(ctx, id, pos) != SUCCESS) {
        ERROR << "failed getting node pos\n";
        return FAILURE;;
    }

    n      = CreateObject<PLC_Node> ();
#if 0
    phy    = CreateObject<PLC_InformationRatePhy> ();
    outlet = CreateObject<PLC_Outlet> (n);
    mac    = CreateObject<PLC_ArqMac> ();
    if (!n || !phy || !txPsd || !outlet || !mac) {
        ERROR << "Cudnot create node/phy/txPsd/outlet\n";
        return FAILURE;
    }
#endif
    // convert to cm
    pos.x *= 100;
    pos.y *= 100;
    pos.z *= 100;

    INFO << "PLC creating node=" << id << " Position="
         << pos.x << " " << pos.y << " " << pos.z << "\n"; 
    n->SetPosition(pos.x, pos.y, pos.z); // convert to cm
    snprintf(name, sizeof(name), "node%d", id);
    n->SetName(name);
    ctx->plcNodes.push_back(n);

#if 0
    noiseFloor = CreateWorstCaseBgNoise(sm)->GetNoisePsd();
    phy->SetNoiseFloor(noiseFloor);
    phy->SetHeaderModulationAndCodingScheme(
            ModulationAndCodingScheme(BPSK_1_2,0));
    phy->SetPayloadModulationAndCodingScheme(
            ModulationAndCodingScheme(BPSK_RATELESS,0));
    mac->SetPhy(phy);
    mac->SetAddress(getMacAddress(id));

    phy->CreateInterfaces(outlet, txPsd);
    phy->GetRxInterface()->AggregateObject(CreateObject<Node> ());
#endif
    return SUCCESS;
}

int plcAddChannel(ifaceCtx_t *ctx)
{
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();
    Ptr<PLC_Channel> channel;
    static PLC_ChannelHelper channelHelper(sm);

    channelHelper.Install(ctx->plcNodes);
    channel = channelHelper.GetChannel();
    if (!channel) {
        ERROR << "cudnot get PLC channel\n";
        return FAILURE;
    }
    channel->InitTransmissionChannels();
    channel->CalcTransmissionChannels();
    return SUCCESS;
}

int plcInstall(ifaceCtx_t *ctx)
{
	int numNodes = stoi(CFG("numOfNodes")), ret;
    uint16_t i, j;
    wf::Nodeinfo *ni = NULL;
    string plc_link, cableStr;
    Ptr<const SpectrumModel> sm;

    sm = plcGetSpectrumModel(CFG("plc_spectrum_model"));
    if (!sm) {
        ERROR << "Get Spectrum model failed\n";
        return FAILURE;
    }

    for (i = 0; i < numNodes; i++) {
        if (plcAddNode(ctx, i) != SUCCESS) {
            ERROR << "PLC Node addition failed\n";
            return FAILURE;
        }
    }

    for (i = 0; i < numNodes; i++) {
        ni = WF_config.get_node_info((uint16_t)i);
        if (!ni) {
            ERROR << "cudnot get nodeinfo\n";
            continue;
        }

        plc_link = ni->getkv("plc_link");
        if (plc_link.empty()) {
            continue;
        }
        vector<string> arr = split(plc_link, ',');
        if (arr.size() < 1) {
            ERROR << "Invalid plc_link format\n";
            return FAILURE;
        }
        j = (uint16_t)stoi(arr.at(0));
        if (!IN_RANGE(j, 0, numNodes)) {
            ERROR << "Invalid dst node in plc_link\n";
            return FAILURE;
        }
        ret = plcConnectLink(ctx, i, j,
                arr.size() > 1 ? arr.at(1) : "NAYY150SE");
        if (ret != SUCCESS) {
            ERROR << "PLC Link setup failed\n";
            return FAILURE;
        }
    }
    if (plcAddChannel(ctx) != SUCCESS) {
        ERROR << "plcAddChannel failed\n";
        return FAILURE;
    }

    static PLC_NetDeviceHelper deviceHelper(sm, getTxPsd(), ctx->plcNodes);
    deviceHelper.SetNoiseFloor(CreateWorstCaseBgNoise(sm)->GetNoisePsd());
    deviceHelper.Setup();
    g_devMap = deviceHelper.GetNetdeviceMap();

    return SUCCESS;
}

#endif // PLC
