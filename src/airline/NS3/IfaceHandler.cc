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

extern iface_t lrwpanIface;

iface_t g_iflist[IFACE_MAX] = {
    [IFACE_LRWPAN] = lrwpanIface,
};

IfaceType getIfaceType(void)
{
    string phy = CFG("PHY");

    if (stricmp(phy, "plc") == 0)
        return IFACE_PLC;
    return IFACE_LRWPAN;
}

int ifaceInstall(ifaceCtx_t *ctx)
{
    iface_t *iface = NULL;
    IfaceType iftype;

    iftype = getIfaceType();
#if 0
    if (stricmp(phy, "plc") == 0) {
#if PLC
        INFO << "Using PLC as PHY\n";
        if (plcInstall(ctx->nodes) != SUCCESS) {
            return FAILURE;
        }
#else
        ERROR << "PLC phy is not enabled in NS3\n";
        return FAILURE;
#endif
    } else
#endif
    iface = &g_iflist[iftype];
    if (iface->inited) {
        ERROR << "Interface is already inited\n";
        return SUCCESS;
    }
    iface->setup(ctx);
    iface->inited = 1;
    return SUCCESS;
}

