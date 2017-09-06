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
#include <dlfcn.h>

#include <commline.h>

static uint8_t g_serial_id[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

int cl_get_id2longaddr(const uint16_t id, uint8_t *addr, const int addrlen)
{
	if(addrlen != 8) {
		ERROR("Invalid addrlen:%d\n", addrlen);
		return CL_FAILURE;
	}
	if(id == 0xffff) {
		memset(addr, 0, addrlen);
		return CL_SUCCESS;
	}

	char *ptr=(char *)&id;

	memcpy(addr, g_serial_id, 8);
	addr[6] = ptr[1];
	addr[7] = ptr[0];
	return CL_SUCCESS;
}

uint16_t cl_get_longaddr2id(const uint8_t *addr)
{
	if(!addr || !(addr[0]|addr[1]|addr[2]|addr[3]|addr[4]|addr[5]|addr[6]|addr[7])) { 
		return 0xffff;
	}
	uint16_t nodeid;
	uint8_t *ptr=(uint8_t*)&nodeid;
	ptr[0] = addr[7];
	ptr[1] = addr[6];
	return nodeid;
}

#if USE_DL
void *g_dl_lib_handle=NULL;
typedef int (*cmd_handler_func_t)(uint16_t src_id, char*buf, int len);
#else
#define	PLAY_CMD(MBUF, CMD)	\
	else if(!strcmp(cmd, #CMD))	\
	{\
		extern int CMD(uint16_t, char *, int);\
		(MBUF)->len = CMD(mbuf->src_id, (char*)(MBUF)->buf, COMMLINE_MAX_BUF);\
	}
#endif

void sl_handle_cmd(msg_buf_t *mbuf)
{
	int aux_len=0;
	char *colon_ptr, cmd[256];

#if USE_DL
	cmd_handler_func_t cmd_func;
	if(!g_dl_lib_handle) {
		g_dl_lib_handle=dlopen(NULL, RTLD_LAZY);
		if(!g_dl_lib_handle) {
			ERROR("Could not load library!!\n");
			return;
		}
	}
#endif

	colon_ptr = strchr((char*)mbuf->buf, ':');
	if(colon_ptr) {
		*colon_ptr++=0;
		aux_len = strlen(colon_ptr);
		strncpy(cmd, (char*)mbuf->buf, sizeof(cmd)-1);
		memmove(mbuf->buf, colon_ptr, aux_len);
	} else {
		strncpy(cmd, (char*)mbuf->buf, sizeof(cmd)-1);
	}
	mbuf->buf[aux_len] = 0;

#if USE_DL
	cmd_func = (cmd_handler_func_t)dlsym(g_dl_lib_handle, cmd);
	if(!cmd_func) {
		ERROR("Could not load cmd: <%s> %s\n", cmd, dlerror());
		mbuf->len = sprintf((char*)mbuf->buf, "SL_INVALID_CMD(%s)", mbuf->buf);
		return;
	}
	mbuf->len = cmd_func(mbuf->src_id, (char*)mbuf->buf, COMMLINE_MAX_BUF);
#else
	if(0) { } 
	PLAY_CMD(mbuf, cmd_rpl_stats)
	PLAY_CMD(mbuf, cmd_def_route)
	PLAY_CMD(mbuf, cmd_route_table)
	PLAY_CMD(mbuf, cmd_rtsize)
	PLAY_CMD(mbuf, cmd_node_osname)
	PLAY_CMD(mbuf, cmd_ipv6_stats)
	PLAY_CMD(mbuf, cmd_nd6_stats)
	PLAY_CMD(mbuf, cmd_icmp_stats)
	PLAY_CMD(mbuf, cmd_udp_stats)
	PLAY_CMD(mbuf, cmd_tcp_stats)
	PLAY_CMD(mbuf, cmd_config_info)
	else {
		mbuf->len = sprintf((char*)mbuf->buf, "SL_INVALID_CMD(%s)", mbuf->buf);
	}   
#endif
}

