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

#ifndef _COMMLINE_H_
#define _COMMLINE_H_

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS 0
#define FAILURE -1

#define CL_SUCCESS 0
#define CL_FAILURE -1

#define USE_UNIX_SOCKETS

#define COMMLINE_MAX_BUF 2048

#define CL_CREATEQ (1 << 0) //Used by airline
#define CL_ATTACHQ (1 << 1) //Used by stackline

int  cl_init(const long my_mtype, const uint8_t flags);
int  cl_bind(const long my_mtype);
void cl_cleanup(void);

//msg_buf_t::flags defined
#define MBUF_IS_ACK         (1 << 0) //Mbuf is an ACK
#define MBUF_IS_CMD         (1 << 1) //Mbuf is a cmd
#define MBUF_DO_NOT_RESPOND (1 << 2) //Cmd does not need a response
//#define	MBUF_OUTPUT_JSON	(1<<2)

#pragma pack(push, 1)
typedef struct _msg_buf_ {
#ifdef USE_UNIX_SOCKETS
    int mtype;
#else
    long mtype;
#endif
    uint16_t src_id;
    uint16_t dst_id;
    uint8_t  flags;
    union {
        struct {
            //Note that you can have one or both or none of the indicators.
            //Value is 0 if not present. NS3 provides LQI only. Castalia provides RSSI.
            uint8_t lqi;  //Link Quality Indicator
            int8_t  rssi; //Rcvd Signal Strength Indicator
        } sig;
        struct {
            uint8_t retries;
            uint8_t status;
        } ack;
    } info;
    uint16_t len, max_len; // length of the buf only
    uint8_t  buf[1];
} msg_buf_t;
#pragma pack(pop)

#define DEFINE_MBUF_SZ(MBUF, SZ)                   \
    uint8_t    MBUF##_buf[sizeof(msg_buf_t) + SZ]; \
    msg_buf_t *MBUF = (msg_buf_t *)MBUF##_buf;     \
    memset(MBUF##_buf, 0, sizeof(MBUF##_buf));     \
    MBUF->max_len = SZ;

#define DEFINE_MBUF(MBUF) DEFINE_MBUF_SZ(MBUF, COMMLINE_MAX_BUF)

#define MAX_CMD_RSP_SZ 4096

#define CL_FLAG_NOWAIT (1 << 1)
int cl_recvfrom_q(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags);
int cl_sendto_q(const long mtype, msg_buf_t *mbuf, uint16_t len);
int cl_get_descriptor(const long mtype);

enum {
    STACKLINE = 1,
    AIRLINE,
    FORKER,
    MONITOR,
    MAX_CL_LINE
};

//In case of Openthread stackline, the packet already contains the machdr
//formed. Thus Airline needs to be informed by setting
//mbuf->dst_id=DSTID_MACHDR_PRESENT.
#define CL_DSTID_MACHDR_PRESENT 0xfffe

#define CL_MGR_ID 0xffff

#define MTYPE(LINE, ID) (((LINE) << 16) | (ID))
#define GET_LINE(MT)    (MT >> 16)

#define ms2hh (3600000)
#define ms2mm (60000)
#define ms2ss (1000)

#ifndef ERROR
#if BASIC_TS_PRN
#define PRN(STR, ...)                                 \
    {                                                 \
        struct timeval tv;                            \
        gettimeofday(&tv, NULL);                      \
        printf("%s %5ld:%-4ld[%s:%d] ", STR,          \
               tv.tv_sec % 100000, tv.tv_usec / 1000, \
               __func__, __LINE__);                   \
        printf(__VA_ARGS__);                          \
    }
#else

#define GET_HH_MM_SS_MS(S, E)                                                           \
    ms = (((E)->tv_sec - (S)->tv_sec) * 1000) + (((E)->tv_usec - (S)->tv_usec) / 1000); \
    hh = ms / ms2hh;                                                                    \
    ms = ms % ms2hh;                                                                    \
    mm = ms / ms2mm;                                                                    \
    ms = ms % ms2mm;                                                                    \
    ss = ms / ms2ss;                                                                    \
    ms = ms % ms2ss;

#define PRN(STR, ...)                                  \
    {                                                  \
        uint32_t              hh, mm, ss, ms;          \
        static struct timeval begin_tv;                \
        struct timeval        tv;                      \
                                                       \
        gettimeofday(&tv, NULL);                       \
        if (!begin_tv.tv_sec)                          \
            begin_tv = tv;                             \
        GET_HH_MM_SS_MS(&begin_tv, &tv);               \
        printf("%s %02u:%02u:%02u.%03u [%s:%d] ", STR, \
               hh, mm, ss, ms, __func__, __LINE__);    \
        printf(__VA_ARGS__);                           \
    }
#endif
#define ERROR(...)             \
    PRN("ERROR", __VA_ARGS__); \
    fflush(NULL)
#define INFO(...)              \
    PRN("INFO ", __VA_ARGS__); \
    fflush(NULL)
#define WARN(...) PRN("WARN " __VA_ARGS__)
#endif //ERROR

/* MAC DataConfirmation status */
enum {
    WF_STATUS_ACK_OK,
    WF_STATUS_NO_ACK,
    WF_STATUS_ERR,
    WF_STATUS_FATAL,
};

#define memcpy_s(DST, DSTLEN, SRC, SRCLEN) \
    memcpy(DST, SRC, DSTLEN < SRCLEN ? DSTLEN : SRCLEN);

#define IN_RANGE(VAL, MIN_N, MAX_N) ((VAL) >= (MIN_N) && (VAL) < (MAX_N))

#define HANDLE_CMD(MBUF, CMD)                                                            \
    else if (!strncasecmp((char *)(MBUF)->buf, #CMD, sizeof(#CMD) - 1))                  \
    {                                                                                    \
        int   aux_len   = 0;                                                             \
        char *colon_ptr = strchr((char *)(MBUF)->buf, ':');                              \
        if (colon_ptr) {                                                                 \
            *colon_ptr++ = 0;                                                            \
            aux_len      = strlen(colon_ptr);                                            \
            memmove((MBUF)->buf, colon_ptr, aux_len);                                    \
        }                                                                                \
        (MBUF)->buf[aux_len] = 0;                                                        \
        (MBUF)->len          = CMD(mbuf->src_id, (char *)(MBUF)->buf, COMMLINE_MAX_BUF); \
    }

#define PRINT_HEX(BUF, LEN, ...)              \
    {                                         \
        int i;                                \
        printf(__VA_ARGS__);                  \
        for (i = 0; i < LEN; i++) {           \
            if (i && !(i % 16))               \
                printf("\n");                 \
            else if (i && !(i % 8))           \
                printf("\t");                 \
            printf("%02x ", (uint8_t)BUF[i]); \
        }                                     \
        printf("\n");                         \
    }

#define CLOSE(FD)  \
    if (FD >= 0) { \
        close(FD); \
        FD = -1;   \
    }

// Stackline Helpers
#include "cl_stackline_helpers.h"

#ifdef __cplusplus
}
#endif

#endif //_COMMLINE_H_
