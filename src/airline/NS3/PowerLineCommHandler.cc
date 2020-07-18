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

#include <arpa/inet.h>

#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/mobility-module.h>
#include <ns3/plc.h>

#include "Nodeinfo.h"
#include "Config.h"
#include "PowerLineCommHandler.h"
#include "IfaceHandler.h"

PLC_SpectrumModelHelper g_smHelper;
PLC_NetdeviceMap g_devMap;
Ptr<PLC_Channel> g_channel;
Ptr<PLC_Graph> g_plcGraph;

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
        CERROR << "Unknown PLC Spectrum Model " << str << "\n";
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
        CERROR << "Unknown PLC Cable " << cableStr << "\n";
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
        ERROR("Cudnot get nodes. Check plc_link nodeid\n");
        return FAILURE;
    }

    INFO("%d <----[%s]----> %d\n", i1, cableStr.c_str(), i2);
    if (plcGetCable(cableStr, cable) != SUCCESS) {
        ERROR("Cud not get PLC Cable\n");
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
        ERROR("Cudnot get node/mobmodel at id=%d\n", id);
        return FAILURE;
    }
    pos = mob->GetPosition (); 
    return SUCCESS;
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

Mac48Address getMacAddress(uint16_t id)
{
    uint8_t idbuf[2];
    char buf[64];

    if (id == 0xffff) {
        return Mac48Address("ff:ff:ff:ff:ff:ff");
    }

    id = htons(id+1);
    memcpy(idbuf, &id, 2);
    snprintf(buf, sizeof(buf), "00:00:00:00:%02x:%02x", *idbuf, *(idbuf+1));
    return Mac48Address(buf);
}

int plcAddOutlet(Ptr<PLC_Node> n, uint16_t id)
{
    Ptr<PLC_Outlet> outlet;
    string outletimp;

    outletimp = WF_config.getNodeCfg(id, "plc_outlet_impedance");
    if (outletimp.empty()) {
        outlet = CreateObject<PLC_Outlet> (n);
    } else {
        INFO("Adding outlet with impedance=%s\n", outletimp.c_str());
        outlet = CreateObject<PLC_Outlet> (n,
                    Create<PLC_ConstImpedance> (plcGetSpectrumModel(), stod(outletimp)));
    }
    return outlet ? SUCCESS : FAILURE;
}

int plcAddInterface(Ptr<PLC_Node> n, uint16_t id)
{
    string intf;
    bool tx = true, rx = true;

    intf = WF_config.getNodeCfg(id, "plc_interface");
    if (!intf.empty()) {
        if (!stricmp(intf, "none")) {
            tx = rx = false;
        } else if (!stricmp(intf, "rx")) {
            tx = false;
        } else if (!stricmp(intf, "tx")) {
            rx = false;
        } else {
            ERROR("PLC Interface unknown dir:%s\n", intf.c_str());
            return FAILURE;
        }
    }
    if (tx) g_channel->AddTxInterface(CreateObject<PLC_TxInterface> (n, plcGetSpectrumModel()));
    if (rx) {
        Ptr<PLC_RxInterface> ri = CreateObject<PLC_RxInterface> (n, plcGetSpectrumModel());
        ri->AggregateObject(CreateObject<Node> ());
        g_channel->AddRxInterface(ri);
    }
    return SUCCESS;
}

/*
 * PLC Node has an outlet which has an interface.
 */
int plcAddNode(ifaceCtx_t *ctx, uint16_t id)
{
    Vector pos;
    Ptr<PLC_Node> n;
    char name[32];

    if (getNodePosition(ctx, id, pos) != SUCCESS) {
        ERROR("failed getting node pos\n");
        return FAILURE;;
    }

    n      = CreateObject<PLC_Node> ();
    // convert to cm
    pos.x *= 100;
    pos.y *= 100;
    pos.z *= 100;

    INFO("PLC creating node=%d Position=%f,%f,%f\n",
         id, pos.x, pos.y, pos.z);
    n->SetPosition(pos.x, pos.y, pos.z); // convert to cm
    getDevName(id, name, sizeof(name));
    n->SetName(name);

    if (plcAddOutlet(n, id) != SUCCESS) {
        ERROR("cudnot create PLC outlet\n");
        return FAILURE;
    }

#if 0
    if (plcAddInterface(n, id) != SUCCESS) {
        ERROR("cudnot add PLC interface\n");
        return FAILURE;
    }
#endif

    g_plcGraph->AddNode(n);
    ctx->plcNodes.push_back(n);

    return SUCCESS;
}

#if 0
int plcAddChannel(ifaceCtx_t *ctx)
{
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();
    Ptr<PLC_Channel> channel;
    static PLC_ChannelHelper channelHelper(sm);

    if (ctx) {
        channelHelper.Install(ctx->plcNodes);
    }
    channel = channelHelper.GetChannel();
    if (!channel) {
        ERROR("cudnot get PLC channel\n");
        return FAILURE;
    }
    channel->InitTransmissionChannels();
    channel->CalcTransmissionChannels();
    return SUCCESS;
}
#endif

int plcAddChannel(ifaceCtx_t *ctx)
{
    Ptr<const SpectrumModel> sm = plcGetSpectrumModel();

    if (!g_plcGraph) {
        g_plcGraph = CreateObject<PLC_Graph> ();
    }
    if (!g_channel) {
        g_channel = CreateObject<PLC_Channel> ();
        g_channel->SetGraph(g_plcGraph);
    }

    if (!g_plcGraph || !g_channel) {
        ERROR("cudnot get create PLC channel\n");
        return FAILURE;
    }
    return SUCCESS;
}

int plcInitChannel(void)
{
    if (!g_channel) {
        ERROR("Channel not created!\n");
        return FAILURE;
    }
    g_channel->InitTransmissionChannels();
    g_channel->CalcTransmissionChannels();
    return SUCCESS;
}

static uint16_t getIdFromMacAddr(Mac48Address addr)
{
    uint8_t buf[6];
    uint16_t id;

    if (addr.IsBroadcast()) {
        return 0xffff;
    }
    addr.CopyTo(buf);
    memcpy(&id, &buf[4], 2);
    id = ntohs(id) - 1;
    if (id >= 0xff00) {
        CERROR << "Problem parsing ucast addr=" << addr;
        return 0;
    }
    CINFO << "Mac48Address=" << addr << " converted to id=" << id << "\n";
    return id;
}

void plcHandleAck(uint16_t id, uint32_t retries, Ptr<Packet> p,
        Mac48Address snd, Mac48Address rcv)
{
    uint16_t dst_id;

    dst_id = getIdFromMacAddr(rcv);
    INFO("PLC rcvd ACK id=%d dst_id=%d retries=%d\n", id, dst_id, retries);
    SendAckToStackline(id, dst_id, WF_STATUS_ACK_OK, retries+1);
}

void plcHandleTxFail(uint16_t id, Ptr<const Packet> p,
                Mac48Address sndr, Mac48Address rcvr)
{
    uint16_t dst_id;

    dst_id = getIdFromMacAddr(rcvr);
    INFO("PLC TX FAILED %d -> %d len:%d\n", id, dst_id, p->GetSize());
    SendAckToStackline(id, dst_id, WF_STATUS_NO_ACK, 0);
}

void plcHandleData(uint16_t id, Ptr<Packet> pin,
        Mac48Address sndr, Mac48Address rcvr)
{
    // HACK: I dont know why PLC module adds extra 8 bytes!
    Ptr<Packet> p = pin->CreateFragment(8, pin->GetSize()-8);
    DEFINE_MBUF(mbuf);

    if (p->GetSize() >= COMMLINE_MAX_BUF) {
        ERROR("Pkt len=%d bigger than max(%d)\n",
                p->GetSize(), COMMLINE_MAX_BUF);
        return;
    }
    CINFO << "PLC rcvd Mac DATA sndr=" << sndr << " rcvr=" << rcvr << "\n";

    mbuf->len           = p->CopyData(mbuf->buf, COMMLINE_MAX_BUF);
    mbuf->src_id        = getIdFromMacAddr(sndr);
    mbuf->dst_id        = getIdFromMacAddr(rcvr);
    mbuf->info.sig.lqi  = 0;
    mbuf->info.sig.rssi = 0;
    INFO("PLC DATA %d -> %d len:%d\n", mbuf->src_id, mbuf->dst_id, mbuf->len);
    SendPacketToStackline(id, mbuf);
}

int plcConfigMac(uint16_t id, Ptr<PLC_NetDevice> dev)
{
    Ptr<PLC_Mac> mac = dev->GetMac();

    INFO("Configuring PLC MAC for id=%d\n", id);
#if 0
    string macstr = CFG("plc_mac");

    if (macstr.empty()) macstr = "ARQ";

    if (macstr.compare("HARQ") == 0) {
        mac = CreateObject<PLC_HarqMac> ();
    } else if (macstr.compare("ARQ") == 0) {
        mac = CreateObject<PLC_ArqMac> ();
    } else {
        CERROR << "Unknown MAC=" << macstr << "\n";
        return FAILURE;
    }

    if (!mac) {
        CERROR << "cud not creat mac obj MAC=" << macstr << "\n";
        return FAILURE;
    }

    mac->SetPhy(dev->GetPhy());
    dev->SetMac(mac);
#endif

    CINFO << "PLC MAC id=" << id << " addr=" << mac->GetAddress() << "\n";
    mac->SetMacAcknowledgementCallback(
            MakeBoundCallback(plcHandleAck, id));
    mac->SetMacDataCallback(
            MakeBoundCallback(plcHandleData, id));
    mac->SetTransmissionFailedCallback(
            MakeBoundCallback(plcHandleTxFail, id));

    return SUCCESS;
}

Ptr<PLC_NetDevice> getPlcNetDev(void *arg, int id)
{
    ifaceCtx_t *ctx = (ifaceCtx_t *)arg;
    extern PLC_NetdeviceMap g_devMap;
    char name[64];
    (void)ctx;
    getDevName(id, name, sizeof(name));
    return g_devMap[name];
}

int plcConfigAllNodes(ifaceCtx_t *ctx)
{
    uint16_t i;
    Ptr<PLC_NetDevice> dev;

    for (i = 0; i < g_devMap.size(); ++i) {
        dev = getPlcNetDev(ctx, i);
        if (!dev) {
            ERROR("cudnot get dev at id=%d\n", i);
            return FAILURE;
        }
        plcConfigMac(i, dev);
    }
    return SUCCESS;
}

void plcEnableLog(void)
{
	string modstr = CFG("plc_log_enable");
    vector<string> mods = split(modstr, ',');
    const char *modname;

    for (uint32_t i = 0; i < mods.size(); ++i) {
        modname = mods.at(i).c_str();
        INFO("Enabling NS3-PLC log for mod [%s]\n", modname);
        LogComponentEnable(modname, LOG_LEVEL_INFO);
    }
}

int plcInstall(ifaceCtx_t *ctx)
{
	int numNodes = stoi(CFG("numOfNodes")), ret;
    uint16_t i, j;
    string plc_link, cableStr;
    Ptr<const SpectrumModel> sm;

    plcEnableLog();

    sm = plcGetSpectrumModel(CFG("plc_spectrum_model"));
    if (!sm) {
        ERROR("Get Spectrum model failed\n");
        return FAILURE;
    }

    if (plcAddChannel(ctx) != SUCCESS) {
        ERROR("plc Add Channel failed\n");
        return FAILURE;
    }

    for (i = 0; i < numNodes; i++) {
        if (plcAddNode(ctx, i) != SUCCESS) {
            ERROR("PLC Node addition failed\n");
            return FAILURE;
        }
    }

    for (i = 0; i < numNodes; i++) {
        plc_link = WF_config.getNodeCfg((uint16_t)i, "plc_link");
        if (plc_link.empty()) {
            continue;
        }
        vector<string> arr = split(plc_link, ',');
        if (arr.size() < 1) {
            ERROR("Invalid plc_link format\n");
            return FAILURE;
        }
        j = (uint16_t)stoi(arr.at(0));
        if (!IN_RANGE(j, 0, numNodes)) {
            ERROR("Invalid dst node(%d) in plc_link\n", j);
            return FAILURE;
        }
        ret = plcConnectLink(ctx, i, j,
                arr.size() > 1 ? arr.at(1) : "NAYY150SE");
        if (ret != SUCCESS) {
            ERROR("PLC Link setup failed\n");
            return FAILURE;
        }
    }

    static PLC_NetDeviceHelper deviceHelper(sm, getTxPsd(), ctx->plcNodes);
    deviceHelper.SetNoiseFloor(CreateWorstCaseBgNoise(sm)->GetNoisePsd());
    deviceHelper.Setup();
    g_devMap = deviceHelper.GetNetdeviceMap();

    plcInitChannel();

    return plcConfigAllNodes(ctx);
}

int plcSend(uint16_t id, Mac48Address dst, Ptr<Packet> pkt)
{
    char name[64];

    getDevName(id, name, sizeof(name));
    Simulator::ScheduleNow(&PLC_NetDevice::Send, g_devMap[name], pkt, dst, 0);
    return SUCCESS;
}

#endif // PLC
