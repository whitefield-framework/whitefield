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

#define _WFOT_PTY_HANDLER_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define ERROR   printf
#define INFO    printf
#define CLOSE(FD)   \
if(FD>=0) {\
    close(FD);\
    FD=-1;\
}

int g_uds_fd=-1, g_uds_childfd=-1;

extern void pty_send_cmd(char *buf, int buflen);
void *pty_handler_thread(void*arg)
{
    char buf[1024];
    ssize_t n;

    while(1) {
        g_uds_childfd = -1;
        INFO("waiting on accept\r\n");
        g_uds_childfd = accept(g_uds_fd, NULL, NULL);
        INFO("accepted fd=%d\r\n", g_uds_childfd);
        while(1) {
            n = recv(g_uds_childfd, buf, sizeof(buf), 0);
            if(n > 0) {
                pty_send_cmd(buf, n);
            } else if(n == 0) {
                INFO("breaking n:%ld\n", n);
                break;
            }
        }
    }
}

int uds_send(char *buf, int buflen)
{
    if(g_uds_childfd <= 0) {
        return 0;
    }
    return send(g_uds_childfd, buf, buflen, 0);
}

int uds_get_path(int nodeid, char *path, int maxlen)
{
    char *uds=getenv("UDS");
    if(uds) {
        return snprintf(path, maxlen, "%s", uds);
    }
    return snprintf(path, maxlen, "%04x.uds", nodeid);
}

int uds_open(int nodeid)
{
    struct sockaddr_un addr;

    g_uds_fd = socket(AF_UNIX, SOCK_STREAM, 0); 
    if(g_uds_fd == -1) {
        ERROR("UDS socket failed %m\n");
        return -1;
    }   
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    uds_get_path(nodeid, addr.sun_path, sizeof(addr.sun_path));
    unlink(addr.sun_path);

    INFO("binding to unix domain sock:[%s]\r\n", addr.sun_path);
    if ((bind(g_uds_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) ||
        (listen(g_uds_fd, 5) == -1))    {
        CLOSE(g_uds_fd);
        ERROR("UDS bind/listen error %m");
        return -1;
    }
    return g_uds_fd; 
}

int start_pty_thread(int nodeid)
{
    pthread_t tid;

    if(uds_open(nodeid)<0) {
        ERROR("uds_open failed\n");
        return -1;
    }

    if(pthread_create(&tid, NULL, pty_handler_thread, NULL)) {
        CLOSE(g_uds_fd);
        ERROR("failure creating pty handler thread %m\n");
        return -1;
    }
	pthread_detach(tid);
    return 0;
}

