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
 * @brief       IfaceHandler Header
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _IFACEHANDLER_H_
#define _IFACEHANDLER_H_

#include <ns3/ptr.h>
#include <ns3/net-device.h>
#include <ns3/node-container.h>

#if PLC
#include <ns3/plc-node.h>
typedef std::vector<ns3::Ptr<ns3::PLC_Node> > PLC_NodeList;
#endif

extern "C" {
#include "commline/commline.h"
}

using namespace ns3;

typedef struct _iface_ctx_ {
    NodeContainer nodes;
#if PLC
    PLC_NodeList plcNodes;
#endif
} ifaceCtx_t;

typedef struct _iface_ {
    // Callbacks
    int (*setup)(ifaceCtx_t *ctx);
    int (*setTxPower)(ifaceCtx_t *ctx, int id, double txpow);
    int (*setPromiscuous)(ifaceCtx_t *ctx, int id);
    int (*setAddress)(ifaceCtx_t *ctx, int id, const char *buf, int sz);
    int (*sendPacket)(ifaceCtx_t *ctx, int id, msg_buf_t *mbuf);
    void (*cleanup)(ifaceCtx_t *ctx);

    uint8_t inited : 1;
} ifaceApi_t;

typedef enum {
    IFACE_LRWPAN, // lr-wpan
    IFACE_PLC,    // Power Line Comm
    IFACE_MAX
} IfaceType;

#define GET_IFACE(CTX, IFACE, API)                \
    IFACE = getIfaceApi(CTX);                     \
    if (!IFACE) {                                 \
        return FAILURE;                           \
    }                                             \
    if (!IFACE->API) {                            \
        CERROR << "Iface does not support API %s" \
               << __FUNCTION__ << "\n";           \
        return ERR_NOT_SUPP;                      \
    }

int               ifaceInstall(ifaceCtx_t *ctx);
ifaceApi_t *      getIfaceApi(ifaceCtx_t *ctx);
static inline int ifaceSetTxPower(ifaceCtx_t *ctx, int id, double txpow)
{
    ifaceApi_t *iface = NULL;

    GET_IFACE(ctx, iface, setTxPower);
    return iface->setTxPower(ctx, id, txpow);
}

static inline int ifaceSetPromiscuous(ifaceCtx_t *ctx, int id)
{
    ifaceApi_t *iface;

    GET_IFACE(ctx, iface, setPromiscuous);
    return iface->setPromiscuous(ctx, id);
}

static inline int ifaceSetAddress(ifaceCtx_t *ctx, int id,
                                  const char *buf, int sz)
{
    ifaceApi_t *iface;

    GET_IFACE(ctx, iface, setAddress);
    return iface->setAddress(ctx, id, buf, sz);
}

static inline int ifaceSendPacket(ifaceCtx_t *ctx, int id,
                                  msg_buf_t *mbuf)
{
    ifaceApi_t *iface;

    GET_IFACE(ctx, iface, sendPacket);
    return iface->sendPacket(ctx, id, mbuf);
}

#endif // _IFACEHANDLER_H_
