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
typedef std::vector<ns3::Ptr<ns3::PLC_Node>> PLC_NodeList;
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
    int  (*setup)(ifaceCtx_t *ctx);
    void (*cleanup)(ifaceCtx_t *ctx);

    uint8_t inited:1;
} iface_t;

typedef enum {
    IFACE_LRWPAN,       // lr-wpan
    IFACE_PLC,          // Power Line Comm
    IFACE_MAX
} IfaceType;

int ifaceInstall(ifaceCtx_t *ctx);

#endif // _IFACEHANDLER_H_
