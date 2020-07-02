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
 * @brief       Interface Handler for NS3
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#include <ns3/single-model-spectrum-channel.h>
#include <ns3/mobility-module.h>
#include <ns3/lr-wpan-module.h>
#include <ns3/spectrum-value.h>

#include <PropagationModel.h>
#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>
#include <IfaceHandler.h>

int setAllNodesParam(NodeContainer & nodes)
{
    Ptr<SingleModelSpectrumChannel> channel;
    string loss_model = CFG("lossModel");
    string del_model = CFG("delayModel");
    bool macAdd = CFG_INT("macHeaderAdd", 1);
    LrWpanSpectrumValueHelper svh;

    if (!loss_model.empty() || !del_model.empty()) {
        channel = CreateObject<SingleModelSpectrumChannel> ();
        if (!channel) {
            return FAILURE;
        }
        if (!loss_model.empty()) {
            static Ptr <PropagationLossModel> plm;
            string loss_model_param = CFG("lossModelParam");
            plm = getLossModel(loss_model, loss_model_param);
            if (!plm) {
                return FAILURE;
            }
            channel->AddPropagationLossModel(plm);
        }
        if (!del_model.empty()) {
            static Ptr <PropagationDelayModel> pdm;
            string del_model_param = CFG("delayModelParam");
            pdm = getDelayModel(del_model, del_model_param);
            if (!pdm) {
                return FAILURE;
            }
            channel->SetPropagationDelayModel(pdm);
        }
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
        if (!loss_model.empty() || !del_model.empty()) {
            dev->SetChannel (channel);
        }
	}
    return SUCCESS;
}

int lrwpanSetup(ifaceCtx_t *ctx)
{
    INFO << "setting up lrwpan\n";
    static LrWpanHelper lrWpanHelper;
    static NetDeviceContainer devContainer = lrWpanHelper.Install(ctx->nodes);
    lrWpanHelper.AssociateToPan (devContainer, CFG_PANID);

    INFO << "Using lr-wpan as PHY\n";
    string ns3_capfile = CFG("NS3_captureFile");
    if(!ns3_capfile.empty()) {
        INFO << "NS3 Capture File:" << ns3_capfile << endl;
        lrWpanHelper.EnablePcapAll (ns3_capfile, false /*promiscuous*/);
    }
    setAllNodesParam(ctx->nodes);
    return SUCCESS;
}

void lrwpanCleanup(ifaceCtx_t *ctx)
{
}

iface_t lrwpanIface = {
    .setup   = lrwpanSetup,
    .cleanup = lrwpanCleanup,
};

