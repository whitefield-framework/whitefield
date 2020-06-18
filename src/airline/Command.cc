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
 * @brief       OAM/Monitor command handler for Airline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_COMMAND_CC_

#include "Command.h"
#include "mac_stats.h"

int cmd_mac_stats(uint16_t nodeid, char *buf, int buflen)
{
	return wf::Macstats::get_summary(nodeid, buf, buflen);
}

void al_handle_cmd(msg_buf_t *mbuf)
{
	if(0) { } 
	HANDLE_CMD(mbuf, cmd_mac_stats)
	else {
        char tmpbuf[256];
        snprintf(tmpbuf, sizeof(tmpbuf), "%s", mbuf->buf);
		mbuf->len = snprintf((char*)mbuf->buf, mbuf->max_len,
                "AL_INVALID_CMD(%s)", tmpbuf);
	}
}

