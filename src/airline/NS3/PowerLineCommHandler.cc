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

#include <ns3/nstime.h>
#include <ns3/simulator.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/plc.h>

#include "Nodeinfo.h"
#include <Config.h>
#include "PowerLineCommHandler.h"

int plcGetSpectrumModel(string str, Ptr<const SpectrumModel> & sm)
{
    PLC_SpectrumModelHelper smHelper;

    if (str.compare("narrowband") == 0)
        sm = smHelper.GetSpectrumModel(0, 500e3, 5);
    else if(str.empty())
        sm = smHelper.GetSpectrumModel(0, 10e6, 100);
    else {
        ERROR << "Unknown PLC Spectrum Model " << str << "\n";
        return FAILURE;
    }
    return SUCCESS;
}

int plcGetCable(string cableStr, Ptr<PLC_Cable> & cable)
{
    int ret;
    Ptr<const SpectrumModel> sm;

    ret = plcGetSpectrumModel(CFG("plc_spectrum_model"), sm);
    if (ret != SUCCESS) {
        return FAILURE;
    }
    if (cableStr.compare("AL3x95XLPE") == 0)
        cable = CreateObject<PLC_AL3x95XLPE_Cable> (sm);
    else if (cableStr.compare("NYCY70SM35") == 0)
        cable = CreateObject<PLC_NYCY70SM35_Cable> (sm);
    else if (cableStr.compare("NAYY50SE") == 0)
        cable = CreateObject<PLC_NAYY50SE_Cable> (sm);
    else if (cableStr.compare("MV_Overhead") == 0)
        cable = CreateObject<PLC_MV_Overhead_Cable> (sm);
    else if(cableStr.empty())
        cable = CreateObject<PLC_NAYY150SE_Cable> (sm);
    else {
        ERROR << "Unknown PLC Cable " << cableStr << "\n";
        return FAILURE;
    }
    return SUCCESS;
}

int plcConnectCable(uint16_t n1, uint16_t n2, string cableStr)
{
    INFO << n1 << "<->" << n2 << " cable=" << cableStr << "\n";
    return SUCCESS;
}

int plcInstall(NodeContainer & nodes)
{
	int numNodes = stoi(CFG("numOfNodes"));
    uint16_t i, j;
    wf::Nodeinfo *ni = NULL;
    string plc_link, cableStr;

    for (i = 0; i < numNodes; i++) {
        ni = WF_config.get_node_info((uint16_t)i);
        if (!ni) {
            ERROR << "cudnot get nodeinfo\n";
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
        plcConnectCable(i, j, arr.size() > 1 ? arr.at(1) : "NAYY150SE");
    }
    return SUCCESS;
}

#endif // PLC
