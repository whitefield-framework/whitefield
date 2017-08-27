/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     commline
 * @{
 *
 * @file
 * @brief       Exported interfaces for commline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_COMMLINE_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <commline.h>
#include <errno.h>
#include <cl_msgq.h>

int cl_init(const uint8_t flags)
{
	return msgq_init(flags);
}

void cl_cleanup(void)
{
	msgq_cleanup();
}

int cl_sendto_q(const long mtype, msg_buf_t *mbuf, uint16_t len)
{
	if(!mbuf || !len) {
		ERROR("sendto invalid parameters passed buf:%p, buflen:%d\n", mbuf, len);
		return CL_FAILURE;
	}
	return msgq_sendto(mtype, mbuf, len);
}

int cl_recvfrom_q(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags)
{
	if(!mbuf || !len) {
		ERROR("invalid parameters passed buf:%p, buflen:%d\n", mbuf, len);
		return CL_FAILURE;
	}
	memset(mbuf, 0, sizeof(msg_buf_t));
	return msgq_recvfrom(mtype, mbuf, len, flags);
}

