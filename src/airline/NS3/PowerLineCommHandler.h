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
 * @brief       NS3 PLC Handler
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _POWERLINECOMMHANDLER_H_
#define _POWERLINECOMMHANDLER_H_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>

#include <ns3/core-module.h>
#include <ns3/node-container.h>
#include <ns3/plc.h>

using namespace ns3;

static inline int getDevName(uint16_t id, char *name, size_t len)
{
    return snprintf(name, len, "node%d", id);
}

int                plcSend(uint16_t id, Mac48Address dst, Ptr<Packet> pkt);
Mac48Address       getMacAddress(uint16_t id);
Ptr<PLC_NetDevice> getPlcNetDev(void *ctx, int id);

#endif //  _POWERLINECOMMHANDLER_H_
