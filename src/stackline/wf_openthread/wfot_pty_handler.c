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
#include <poll.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <commline/commline.h>

int g_uds_fd=-1, g_uds_childfd=-1;

extern void pty_send_cmd(char *buf, int buflen);
extern int NODE_ID;
int uds_send(char *buf, int buflen)
{
    if(g_uds_childfd <= 0) return 0;
    return send(g_uds_childfd, buf, buflen?buflen:strlen(buf), 0);
}

int uds_get_path(char *path, int maxlen)
{
    char *uds=getenv("UDSPATH");
    if(uds) {
        return snprintf(path, maxlen, "%s/%04x.uds", uds, NODE_ID-1);
    }
    return snprintf(path, maxlen, "%04x.uds", NODE_ID-1);
}

void uds_close(void)
{
    char udspath[512];
    if(g_uds_fd) {
        uds_get_path(udspath, sizeof(udspath));
        unlink(udspath);
        CLOSE(g_uds_fd);
    }
    INFO("exiting...\r\n");
}

int uds_open(void)
{
    struct sockaddr_un addr;

    g_uds_fd = socket(AF_UNIX, SOCK_STREAM, 0); 
    if(g_uds_fd == -1) {
        ERROR("UDS socket failed %m\n");
        return -1;
    }   
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    uds_get_path(addr.sun_path, sizeof(addr.sun_path));
    unlink(addr.sun_path);

    INFO("binding to unix domain sock:[%s]\r\n", addr.sun_path);
    if ((bind(g_uds_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) ||
        (listen(g_uds_fd, 5) == -1))    {
        CLOSE(g_uds_fd);
        ERROR("UDS bind/listen error %m");
        return -1;
    }
    atexit(uds_close);
    return g_uds_fd; 
}

#define ADD_TO_FDSET(FD)    \
    if(aReadFdSet && (FD >= 0)) {\
        FD_SET(FD, aReadFdSet);\
        if(aErrorFdSet) {\
            FD_SET(FD, aErrorFdSet);\
        }\
        if(aMaxFd && *aMaxFd < FD) {\
            *aMaxFd = FD;\
        }\
    }

extern "C" void __wrap_platformUartUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, fd_set *aErrorFdSet, int *aMaxFd)
{
    extern void __real_platformUartUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, fd_set *aErrorFdSet, int *aMaxFd);
    ADD_TO_FDSET(g_uds_childfd);
    if(g_uds_childfd < 0) {
        ADD_TO_FDSET(g_uds_fd);
    }
    return __real_platformUartUpdateFdSet(aReadFdSet, aWriteFdSet, aErrorFdSet, aMaxFd);
}

void wfHandlePtyEvent(void)
{
    char buf[1024];
    ssize_t rval;
    const int error_flags = POLLERR | POLLNVAL | POLLHUP;
    struct pollfd pollfd[] =
    {
        { g_uds_childfd>=0?g_uds_childfd:g_uds_fd,  POLLIN  | error_flags, 0 },
    };

    errno = 0;
    rval = poll(pollfd, 1, 0);
    if (rval < 0) {
        perror("wfpoll");
        exit(1);
    }

    if (rval > 0) {
        if ((pollfd[0].revents & error_flags) != 0) {
            if(g_uds_childfd >= 0) {
                CLOSE(g_uds_childfd);
            }
        }

        if (pollfd[0].revents & POLLIN) {
            if(g_uds_childfd >= 0) {
                rval = recv(g_uds_childfd, buf, sizeof(buf), 0);
                if (rval <= 0) {
                    INFO("Closing child conn:%d\n", g_uds_childfd);
                    CLOSE(g_uds_childfd);
                } else {
                    pty_send_cmd(buf, rval);
                }
            } else {
                g_uds_childfd = accept(g_uds_fd, NULL, NULL);
                if(g_uds_childfd < 0) {
                    perror("wfHandlePtyEvent accept");
                    exit(1);
                }
            }
        }
    }
}

