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
 * @brief       PLC Interface Handler for NS3
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#if PLC

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>
#include <IfaceHandler.h>
#include <PowerLineCommHandler.h>

static int plcSetup(ifaceCtx_t *ctx)
{
    extern int plcInstall(ifaceCtx_t *ctx);

    INFO("setting up plc\n");
    return plcInstall(ctx);
}

static int plcSetAddress(ifaceCtx_t *ctx, int id, const char *buf, int sz)
{
    Ptr<PLC_NetDevice> dev;
    Mac48Address addr;

    if (sz == 6) {
        addr.CopyFrom((const uint8_t *)buf);
//    } else if(sz == 2) {
//        addr = getMacAddress((uint16_t)id);
    } else {
        CERROR << "PLC Invalid address size=" << sz << "\n";
        return FAILURE;
    }
    dev = getPlcNetDev(ctx, id);
    dev->GetMac()->SetAddress(addr);
    return SUCCESS;
}

static int plcSendPacket(ifaceCtx_t *ctx, int id, msg_buf_t *mbuf)
{
    Ptr<Packet> pkt;
    Mac48Address dst;

    pkt = Create<Packet> (mbuf->buf, (uint32_t)mbuf->len);
    dst = getMacAddress(mbuf->dst_id);

    INFO("Sending PLC pkt id=%d dst=%d len=%d\n",
            id, mbuf->dst_id, pkt->GetSize());
    return plcSend(id, dst, pkt);
}

static void plcCleanup(ifaceCtx_t *ctx)
{
    INFO("PLC Cleanup called\n");
}

ifaceApi_t plcIface = {
    setup          : plcSetup,
    setTxPower     : NULL,
    setPromiscuous : NULL,
    setAddress     : plcSetAddress,
    sendPacket     : plcSendPacket,
    cleanup        : plcCleanup,
};

#endif
