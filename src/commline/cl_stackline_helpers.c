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
 * @brief       Helper functions for stacklines
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_CL_STACKLINE_HELPERS_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include <commline.h>

static uint8_t serial_id[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

int cl_get_id2longaddr(const uint16_t id, uint8_t *addr, const int addrlen)
{
	if(addrlen != 8 || id == 0xffff) {
		ERROR("Invalid addrlen:%d id:%x\n", addrlen, id);
		return CL_FAILURE;
	}
	char *ptr=(char *)&id;

	memcpy(addr, serial_id, 8);
	addr[6] = ptr[1];
	addr[7] = ptr[0];
	return CL_SUCCESS;
}
