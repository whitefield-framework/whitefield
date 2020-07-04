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
 * @brief       OAM PTY handler thread in forker
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _PTY_HANDLER_C_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "commline/commline.h"
#include "utils/forker_common.h"

int                   g_pty_epollfd = -1;
extern child_psinfo_t g_child_info[];

#define MASTER_FD g_child_info[nodeid].master
#define UDS_FD    g_child_info[nodeid].uds_fd
#define PEER      g_child_info[nodeid].peer
#define PEERLEN   g_child_info[nodeid].peerlen
int handle_pty_event(int nodeid, int ismaster)
{
    char buf[1024];
    int  n;

    if (ismaster) {
        n = read(MASTER_FD, buf, sizeof(buf));
        INFO("master read n=%d, peerset=%d\n", n, PEERLEN);
        INFO("master data:\n%.*s\n", n, buf);
        if (n > 0 && PEERLEN) {
            n = sendto(UDS_FD, buf, n, 0, (struct sockaddr *)&PEER, PEERLEN);
        }
    } else {
        PEERLEN = sizeof(PEER);
        n       = recvfrom(UDS_FD, buf, sizeof(buf), 0, (struct sockaddr *)&PEER, &PEERLEN);
        INFO("uds read n=%d, peerset=%d\n", n, PEERLEN);
        if (n > 0) {
            n = write(MASTER_FD, buf, n);
        } else {
            PEERLEN = 0;
        }
    }
    if (n <= 0) {
        ERROR("[%m] nodeid:%d, write failed ismaster:%d n=%d\n", nodeid, ismaster, n);
        return FAILURE;
    }
    return SUCCESS;
}

#define MAXEVENTS 32
void *pty_handler_thread(void *arg)
{
    int                n, i, nodeid, ismaster;
    struct epoll_event events[MAXEVENTS];

    memset(events, 0, sizeof(events));
    for (;;) {
        n = epoll_wait(g_pty_epollfd, events, MAXEVENTS, -1);
        if (n <= 0) {
            ERROR("epoll_wait return n=%d\n", n);
            usleep(10000);
            continue;
        }
        for (i = 0; i < n; i++) {
            nodeid   = events[i].data.u32 & 0x7fffffff;
            ismaster = events[i].data.u32 & 0x80000000;
            handle_pty_event(nodeid, ismaster);
        }
    }
}

int pty_add_fd(int nodeid, int fd, int ismaster)
{
    struct epoll_event ev;

    ev.events = EPOLLIN;

    //We need this first bit to differentiate between master fd versus uds_fd
    ev.data.u32 = ((uint32_t)nodeid | (ismaster ? 0x80000000 : 0));
    if (epoll_ctl(g_pty_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        ERROR("epoll_ctl failed nodeid:%d, fd:%d %m\n", nodeid, fd);
        return FAILURE;
    }
    return SUCCESS;
}

int start_pty_thread(void)
{
    pthread_t tid;

    g_pty_epollfd = epoll_create1(0);
    if (g_pty_epollfd < 0) {
        ERROR("failed creating epollfd %m\n");
        return FAILURE;
    }

    if (pthread_create(&tid, NULL, pty_handler_thread, NULL)) {
        ERROR("failure creating pty handler thread %m\n");
        CLOSE(g_pty_epollfd);
        return FAILURE;
    }
    pthread_detach(tid);
    return SUCCESS;
}

int uds_get_path(int nodeid, char *path, int maxlen)
{
    char *logpath      = getenv("LOGPATH");
    char *monitor_port = getenv("MONITOR_PORT");

    return snprintf(path, maxlen, "%s/%s_%04x.unixsock",
                    logpath ? logpath : "/tmp",
                    monitor_port, nodeid);
}

int uds_open(int nodeid)
{
    struct sockaddr_un addr;
    int                fd;

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd == -1) {
        ERROR("UDS socket failed %m\n");
        return FAILURE;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    uds_get_path(nodeid, addr.sun_path, sizeof(addr.sun_path));
    unlink(addr.sun_path);

    INFO("binding to unix domain sock:[%s]\n", addr.sun_path);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        CLOSE(fd);
        ERROR("UDS bind error %m");
        return FAILURE;
    }
    return fd;
}
