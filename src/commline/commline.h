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

#ifndef	_COMMLINE_H_
#define	_COMMLINE_H_

#include <sys/time.h>

#define	CL_SUCCESS	0
#define	CL_FAILURE	-1

#define	COMMLINE_MAX_BUF	2048

#define	CL_CREATEQ	(1<<0)	//Used by airline
#define	CL_ATTACHQ	(1<<1)	//Used by stackline

int cl_init(const uint8_t flags);
void cl_cleanup(void);

//msg_buf_t::flags defined
#define	MBUF_IS_ACK	(1<<0)
#define	MBUF_IS_CMD	(1<<1)
//#define	MBUF_OUTPUT_JSON	(1<<2)

#pragma pack(push,1)
typedef struct _msg_buf_
{
	long mtype;
	uint16_t src_id;
	uint16_t dst_id;
	uint8_t flags;
	union {
		struct {
			//Note that you can have one or both or none of the indicators.
			//Value is 0 if not present. NS3 provides LQI only. Castalia provides RSSI.
			uint8_t lqi;	//Link Quality Indicator 
			int8_t rssi;	//Rcvd Signal Strength Indicator
		}sig;
		struct {
			uint8_t retries;
			uint8_t status;
		}ack;
	} info;
	uint16_t len; // length of the buf only
	uint8_t buf[1];
}msg_buf_t;
#pragma pack(pop)

#define	DEFINE_MBUF(MBUF)	\
		uint8_t MBUF##_buf[sizeof(msg_buf_t)+COMMLINE_MAX_BUF];\
		msg_buf_t *MBUF = (msg_buf_t *)MBUF##_buf;\
		memset(MBUF##_buf, 0, sizeof(MBUF##_buf));

#define	CL_FLAG_NOWAIT	(1<<1)
int cl_recvfrom_q(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags);
int cl_sendto_q(const long mtype, msg_buf_t *mbuf, uint16_t len);

enum {
	STACKLINE=1,
	AIRLINE,
	FORKER,
	MONITOR,
};

#define	CL_MGR_ID	0xffff

#define	MTYPE(LINE,ID)	(((LINE)<<16)|(ID))

#ifndef	ERROR
#define	PRN(STR, ...)	\
	{\
		struct timeval tv;\
		gettimeofday(&tv, NULL);\
		printf("%s [%ld:%ld] [%s:%d] ", STR, tv.tv_sec, tv.tv_usec, __FUNCTION__, __LINE__);\
		printf(__VA_ARGS__);\
		fflush(stdout);\
	}
#define	ERROR(...) PRN("ERROR", __VA_ARGS__)
#define	INFO(...)  PRN("INFO ", __VA_ARGS__)
#define	WARN(...)  PRN("WARN "__VA_ARGS__)
#endif //ERROR

/* MAC DataConfirmation status */
enum {
	WF_STATUS_ACK_OK,
	WF_STATUS_NO_ACK,
	WF_STATUS_ERR,
	WF_STATUS_FATAL,
};

#define	memcpy_s(DST, DSTLEN, SRC, SRCLEN)	\
	memcpy(DST, SRC, DSTLEN<SRCLEN?DSTLEN:SRCLEN);

#define	IN_RANGE(VAL, MIN_N, MAX_N)	((VAL)>=(MIN_N) && (VAL)<(MAX_N))

#define	HANDLE_CMD(MBUF, CMD)	\
	else if(!strncasecmp((char*)(MBUF)->buf, #CMD, sizeof(#CMD)-1))	\
	{\
		int aux_len=0;\
		char *colon_ptr = strchr((char*)(MBUF)->buf, ':');\
		if(colon_ptr) {\
			*colon_ptr++=0;\
			aux_len = strlen(colon_ptr);\
			memmove((MBUF)->buf, colon_ptr, aux_len);\
		}\
		(MBUF)->buf[aux_len] = 0;\
		(MBUF)->len = CMD(mbuf->src_id, (char*)(MBUF)->buf, COMMLINE_MAX_BUF);\
	}

// Stackline Helpers
#include "cl_stackline_helpers.h"

#endif	//_COMMLINE_H_
