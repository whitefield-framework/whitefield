/*
 * Copyright (C) 2018 Rahul Jadhav <nyrahul@gmail.com>
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
 * @brief       Abstract Unix domain sockets for commline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _CL_USOCK_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#include <commline.h>
#include <cl_usock.h>

int g_usock_fd[MAX_CL_LINE] = { -1 };
int g_def_line              = -1;

socklen_t usock_setabsaddr(const long mtype, struct sockaddr_un *addr)
{
    int   len;
    uid_t uid = getuid();

    addr->sun_family  = AF_UNIX;
    addr->sun_path[0] = 0;
    len               = snprintf(&addr->sun_path[1], sizeof(addr->sun_path) - 1, "/WHITEFIELD_%d_%08lx", uid, mtype);
    return sizeof(sa_family_t) + len + 1;
}

int usock_init(const long my_mtype, const uint8_t flags)
{
    socklen_t          slen;
    struct sockaddr_un addr;
    int                line = GET_LINE(my_mtype);

    if (!IN_RANGE(line, 1, MAX_CL_LINE)) {
        ERROR("my_mtype:%08lx not in range!\n", my_mtype);
        return FAILURE;
    }

    g_usock_fd[line] = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (g_usock_fd[line] < 0) {
        ERROR("socket failure errno=%d\n", errno);
        return FAILURE;
    }

    slen = usock_setabsaddr(my_mtype, &addr);
    INFO("USOCK binding to [%s]\n", &addr.sun_path[1]);

    if (bind(g_usock_fd[line], (struct sockaddr *)&addr, slen)) {
        CLOSE(g_usock_fd[line]);
        ERROR("bind failed errno=%d\n", errno);
        return FAILURE;
    }
    if (g_def_line < 0) {
        g_def_line = line;
    }
    return SUCCESS;
}

void usock_cleanup(void)
{
    int i;
    for (i = 0; i < MAX_CL_LINE; i++) {
        CLOSE(g_usock_fd[i]);
    }
    INFO("closed commline unix sockets\n");
}

int usock_recvfrom(const long my_mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags)
{
    int ret;
    int line = GET_LINE(my_mtype);

    if (!IN_RANGE(line, 1, MAX_CL_LINE)) {
        ERROR("my_mtype:%08lx not in range!\n", my_mtype);
        return FAILURE;
    }

    mbuf->len = 0;

    ret = recvfrom(g_usock_fd[line], (void *)mbuf, len, (flags & CL_FLAG_NOWAIT) ? MSG_DONTWAIT : 0, NULL, 0);
    if (ret > 0 && ret + 4 < sizeof(msg_buf_t)) //Rahul: +4 is added for bins compiled with -m32. sizeof(long) issue.
    {
        ERROR("problem ... recvfrom len(%d) not enough sizeof:%zu\n", ret, sizeof(msg_buf_t));
        return FAILURE;
    }
    return ret;
}

int usock_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len)
{
    struct sockaddr_un addr;
    socklen_t          slen;
    int                ret;

    mbuf->mtype = mtype;

    slen = usock_setabsaddr(mtype, &addr);
    ret  = sendto(g_usock_fd[g_def_line], (void *)mbuf, len, 0, (struct sockaddr *)&addr, slen);
    if (ret != len) {
        ERROR("usock sendto failed! errno=%d\n", errno);
        return FAILURE;
    }
    return SUCCESS;
}

int usock_get_descriptor(const long mtype)
{
    int line = GET_LINE(mtype);

    if (!IN_RANGE(line, 1, MAX_CL_LINE)) {
        ERROR("my_mtype:%08lx not in range!\n", mtype);
        return FAILURE;
    }
    if (g_usock_fd[line] <= 0) {
        ERROR("mtype:%08lx line:%d no fd here\n", mtype, line);
        return FAILURE;
    }

    return g_usock_fd[line];
}
