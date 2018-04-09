/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     stackline
 * @{
 *
 * @file
 * @brief       Openthread PTY handler thread
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _WFOT_CMD_HANDLER_C_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <commline/commline.h>

#ifdef  __cplusplus
extern "C" {
#endif

int cmd_def_route(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_rtsize(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_rpl_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_route_table(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_node_osname(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return snprintf(buf, buflen, "openthread");
}

int cmd_ipv6_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_nd6_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_icmp_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_udp_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_tcp_stats(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}

int cmd_config_info(uint16_t id, char *buf, int buflen)
{
    INFO("handling cmd=%s\n", __FUNCTION__);
    return 0;
}


#ifdef  __cplusplus
}
#endif
