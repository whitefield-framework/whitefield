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

#define __IFACEHANDLER_CC_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>
#include <IfaceHandler.h>

extern ifaceApi_t lrwpanIface;
#if PLC
extern ifaceApi_t plcIface;
#endif

ifaceApi_t g_iflist[IFACE_MAX] = {
    [IFACE_LRWPAN] = lrwpanIface,
#if PLC
    [IFACE_PLC]    = plcIface
#endif
};

ifaceApi_t *getIfaceApi(ifaceCtx_t *ctx)
{
    IfaceType iftype = IFACE_LRWPAN;
    string phy = CFG("PHY");

    if (!stricmp(phy, "plc")) iftype = IFACE_PLC;
    return &g_iflist[iftype];
}

int ifaceInstall(ifaceCtx_t *ctx)
{
    int ret;
    ifaceApi_t *iface = NULL;

    iface = getIfaceApi(ctx);
    if (iface->inited) {
        ERROR("Interface is already inited\n");
        return SUCCESS;
    }
    ret = iface->setup(ctx);
    if (ret == SUCCESS) {
        iface->inited = 1;
    }
    return ret;
}

