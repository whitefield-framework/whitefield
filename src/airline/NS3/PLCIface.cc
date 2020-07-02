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

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>
#include <IfaceHandler.h>
#include <PowerLineCommHandler.h>

int plcSetup(ifaceCtx_t *ctx)
{
    INFO << "setting up plc\n";
    return plcInstall(ctx->plcNodes);
}

void plcCleanup(ifaceCtx_t *ctx)
{
}

iface_t plcIface = {
    .setup   = plcSetup,
    .cleanup = plcCleanup,
};

