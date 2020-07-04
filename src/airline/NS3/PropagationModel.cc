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
 * @brief       Propagation Loss Models from NS3 for Whitefield
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_PROPMODEL_CC_

#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "common.h"
#include "PropagationModel.h"

Ptr <PropagationLossModel> getLogDistancePLM(map<string, string, ci_less> & m)
{
    Ptr <LogDistancePropagationLossModel> plm =
        CreateObject<LogDistancePropagationLossModel> ();
    string ple = getMapCfg(m, "PathLossExp");
    string refdist = getMapCfg(m, "refDist");
    string refloss = getMapCfg(m, "refLoss");

    if (!plm) {
        CERROR << "Cud not get Log Distance prop loss model\n";
        return plm;
    }
    if (!ple.empty()) {
        plm->SetPathLossExponent(stod(ple));
    }
    if (!refdist.empty() && !refloss.empty()) {
        plm->SetReference(stod(refdist), stod(refloss));
    } else if(!refdist.empty() || !refloss.empty()) {
        CERROR << "refDist and refLoss both are needed for LogDistancePropagationLossModel\n";
    }
    return plm;
}

Ptr <PropagationLossModel> getFriisPLM(map<string, string, ci_less> & m)
{
    Ptr <FriisPropagationLossModel> plm =
        CreateObject<FriisPropagationLossModel> ();
    string freq = getMapCfg(m, "freq");
    string minloss = getMapCfg(m, "minloss");
    string sysloss = getMapCfg(m, "sysloss");

    if (!plm) {
        CERROR << "Cud not get Friis prop loss model\n";
        return plm;
    }
    if (!freq.empty()) {
        plm->SetFrequency(stod(freq));
    }
    if (!minloss.empty()) {
        plm->SetMinLoss(stod(minloss));
    }
    if (!sysloss.empty()) {
        plm->SetSystemLoss(stod(sysloss));
    }
    return plm;
}

Ptr <PropagationLossModel> getFixedRssPLM(map<string, string, ci_less> & m)
{
    Ptr <FixedRssLossModel> plm = CreateObject<FixedRssLossModel> ();
    string rss = getMapCfg(m, "rss");

    if (!plm) {
        CERROR << "Cud not get FixedRss loss model\n";
        return plm;
    }
    if (!rss.empty()) {
        plm->SetRss(stod(rss));
    }
    return plm;
}

Ptr <PropagationLossModel> getMatrixPLM(map<string, string, ci_less> & m)
{
    Ptr <MatrixPropagationLossModel> plm =
        CreateObject<MatrixPropagationLossModel> ();
    string defloss = getMapCfg(m, "defloss");

    if (!plm) {
        CERROR << "Cud not get matrix loss model\n";
        return plm;
    }
    if (!defloss.empty()) {
        plm->SetDefaultLoss(stod(defloss));
    }
    return plm;
}

Ptr <PropagationLossModel> getTwoRayGroundPLM(map<string, string, ci_less> & m)
{
    Ptr <TwoRayGroundPropagationLossModel> plm =
        CreateObject<TwoRayGroundPropagationLossModel> ();
    string sysloss = getMapCfg(m, "sysloss");
    string freq = getMapCfg(m, "freq");
    string htabovez = getMapCfg(m, "HeightAboveZ");
    string mindist = getMapCfg(m, "minDist");

    if (!plm) {
        CERROR << "Cud not get matrix loss model\n";
        return plm;
    }
    if (!sysloss.empty()) {
        plm->SetSystemLoss(stod(sysloss));
    }
    if (!freq.empty()) {
        plm->SetFrequency(stod(freq));
    }
    if (!htabovez.empty()) {
        plm->SetHeightAboveZ(stod(htabovez));
    }
    if (!mindist.empty()) {
        plm->SetMinDistance(stod(mindist));
    }
    return plm;
}

/* Get Loss Propagation Model */
Ptr <PropagationLossModel> getLossModel(string loss_model,
                                        string loss_model_param)
{
    Ptr <PropagationLossModel> plm;
    auto cfgmap = splitKV(loss_model_param);

    CINFO << "Using loss model [" << loss_model
         << "] param [" << loss_model_param << "]\n";
    if (stricmp(loss_model, "LogDistance") == 0) {
        plm = getLogDistancePLM(cfgmap);
    } else if (stricmp(loss_model, "Friis") == 0) {
        plm = getFriisPLM(cfgmap);
    } else if (stricmp(loss_model, "FixedRss") == 0) {
        plm = getFixedRssPLM(cfgmap);
    } else if (stricmp(loss_model, "Matrix") == 0) {
        plm = getMatrixPLM(cfgmap);
    } else if (stricmp(loss_model, "TwoRayGround") == 0) {
        plm = getTwoRayGroundPLM(cfgmap);
    } else if (stricmp(loss_model, "Random") == 0) {
        plm = CreateObject<RandomPropagationLossModel> ();
    } else if (stricmp(loss_model, "Range") == 0) {
        plm = CreateObject<RangePropagationLossModel> ();
    } else if (stricmp(loss_model, "ThreeLogDistance") == 0) {
        plm = CreateObject<ThreeLogDistancePropagationLossModel> ();
    } else {
        CERROR << "Unknown loss model [" << loss_model << "]\n";
        return NULL;
    }
    if (!plm) {
        CERROR << "Cud not get " << loss_model << " loss model\n";
        return plm;
    }
    if (cfgmap.size() != 0) {
        map<string, string, ci_less>::iterator i;
        for (i = cfgmap.begin(); i != cfgmap.end(); ++i) {
            CERROR << "Unprocessed loss model param: " 
                  << i->first << "=" << i->second << "\n";
        }
    }
    return plm;
}

Ptr <PropagationDelayModel> getConstantSpeedPDM(map<string, string, ci_less> & m)
{
    Ptr <ConstantSpeedPropagationDelayModel> pdm =
        CreateObject<ConstantSpeedPropagationDelayModel> ();
    string speed = getMapCfg(m, "speed");

    if (!pdm) {
        CERROR << "Cud not get Log Distance prop loss model\n";
        return pdm;
    }
    if (!speed.empty()) {
        pdm->SetSpeed(stod(speed));
    }
    return pdm;
}

/* Get Delay Propagation Model */
Ptr <PropagationDelayModel> getDelayModel(string del_model,
                                        string del_model_param)
{
    Ptr <PropagationDelayModel> pdm;
    auto cfgmap = splitKV(del_model_param);

    CINFO << "Using delay model [" << del_model
         << "] param [" << del_model_param << "]\n";
    if (stricmp(del_model, "ConstantSpeed") == 0) {
        pdm = getConstantSpeedPDM(cfgmap);
    } else if (stricmp(del_model, "Random") == 0) {
        pdm = CreateObject<RandomPropagationDelayModel> ();
    } else {
        CERROR << "Unknown delay model [" << del_model << "]\n";
        return NULL;
    }
    if (!pdm) {
        CERROR << "Cud not get " << del_model << " delay model\n";
        return pdm;
    }
    if (cfgmap.size() != 0) {
        map<string, string, ci_less>::iterator i;
        for (i = cfgmap.begin(); i != cfgmap.end(); ++i) {
            CERROR << "Unprocessed delay model param: " 
                  << i->first << "=" << i->second << "\n";
        }
    }
    return pdm;
}

