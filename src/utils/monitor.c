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
 * @brief       OAM Monitor thread in forker
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _MONITOR_C_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "commline/commline.h"

int gMonitorFD = -1;
int start_udp_server(int portno)
{
    struct sockaddr_in myaddr; /* our address */
    int                fd;     /* our socket */

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        ERROR("cannot create socket %m\n");
        goto failure;
    }

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family      = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port        = htons(portno);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        ERROR("bind failed portno=%d %m\n", portno);
        goto failure;
    }
    return fd;
failure:
    if (fd > 0) {
        close(fd);
    }
    return FAILURE;
}

int fwd_cmd_on_commline(char *cmd, size_t cmdlen, char *rsp, size_t rsplen)
{
    DEFINE_MBUF_SZ(mbuf, MAX_CMD_RSP_SZ);
    int   line = 0, c = 0, id = CL_MGR_ID;
    char *ptr;

    if (!strncasecmp(cmd, "AL:", sizeof("AL:") - 1)) {
        line = AIRLINE;
    } else if (!strncasecmp(cmd, "SL:", sizeof("SL:") - 1)) {
        line = STACKLINE;
    } else {
        return snprintf(rsp, rsplen, "INVALID_LINE");
    }
    ptr    = strchr(cmd, ':');
    *ptr++ = 2;
    if (isdigit(*ptr)) {
        mbuf->src_id = atoi(ptr);
        cmd          = strchr(ptr, ':');
        if (!cmd) {
            return snprintf(rsp, rsplen, "INVALID_CMD");
        }
        cmd++;
    } else {
        cmd          = ptr;
        mbuf->src_id = CL_MGR_ID;
    }
    cmdlen = strlen(cmd);
    strncpy((char *)mbuf->buf, cmd, mbuf->max_len);
    mbuf->len   = cmdlen;
    mbuf->flags = MBUF_IS_CMD;
    if (STACKLINE == line) {
        id = mbuf->src_id;
    }
    INFO("sending cmd:<%s> <%d>\n", mbuf->buf, mbuf->max_len);
    cl_sendto_q(MTYPE(line, id), mbuf, sizeof(msg_buf_t) + mbuf->len);
    while (c++ < 100) {
        usleep(1000);
        cl_recvfrom_q(MTYPE(MONITOR, CL_MGR_ID), mbuf, sizeof(mbuf_buf), CL_FLAG_NOWAIT);
        if (mbuf->len > 0) {
            memcpy_s(rsp, rsplen, mbuf->buf, mbuf->len);
            rsp[mbuf->len++] = 0;
            return mbuf->len;
        }
    }
    return snprintf(rsp, rsplen, "NO_RSP_FROM_LINE:%d", line);
}

#define END_OF_RSP "END"
void *monitor_thread(void *arg)
{
    struct sockaddr_in remaddr;                /* remote address */
    socklen_t          alen = sizeof(remaddr); /* length of addresses */
    char               cmd[128];               /* receive buffer */
    char               rsp[MAX_CMD_RSP_SZ];    /* response send buffer */
    int                n;                      /* # bytes received */

    while (1) {
        n = recvfrom(gMonitorFD, cmd, sizeof(cmd) - 1, 0, (struct sockaddr *)&remaddr, &alen);
        if (n <= 0) {
            continue;
        }
        cmd[n] = 0;
        n      = fwd_cmd_on_commline(cmd, n, rsp, sizeof(rsp));
        if (n <= 0) {
            n = snprintf(rsp, sizeof(rsp), "CMD_FAILURE");
        }
        sendto(gMonitorFD, rsp, n, 0, (struct sockaddr *)&remaddr, alen);
        sendto(gMonitorFD, END_OF_RSP, sizeof(END_OF_RSP), 0, (struct sockaddr *)&remaddr, alen);
        usleep(1);
    }
    //Unreachable!
    close(gMonitorFD);
    return NULL;
}

int start_monitor_thread(void)
{
    pthread_t tid;
    char *    ptr = getenv("MONITOR_PORT");

    if (!ptr) {
        ERROR("MONITOR_PORT is not defined. (Chk config.inc)\n");
        return FAILURE;
    }

    gMonitorFD = start_udp_server(atoi(ptr));
    if (gMonitorFD < 0) {
        ERROR("Failure starting UDP server\n");
        return FAILURE;
    }
    if (pthread_create(&tid, NULL, monitor_thread, NULL) < 0) {
        ERROR("failure creating monitor thread %m\n");
        CLOSE(gMonitorFD);
        return FAILURE;
    }
    pthread_detach(tid);
    return SUCCESS;
}
