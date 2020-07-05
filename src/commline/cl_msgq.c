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
 * @brief       SysV message queue for commline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _CL_MSGQ_C_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/* sys v msgq related headers */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <commline.h>
#include <cl_msgq.h>

int gMsgQ_id = -1;

key_t get_msgq_key(void)
{
    char buf[512];
    return ftok(getcwd(buf, sizeof(buf)), 0xab);
}

int msgq_init(const long my_mtype, const uint8_t flags)
{
    key_t key;
    int   msgflag = 0666 | IPC_CREAT;

    key = get_msgq_key();
    if (-1 == key) {
        ERROR("could not get a valid msgq key\n");
        return FAILURE;
    }
    if (flags & CL_CREATEQ) {
        msgflag |= IPC_EXCL;
    }
    gMsgQ_id = msgget(key, msgflag);
    if (-1 == gMsgQ_id) {
        ERROR("could not get msgq key:0x%x\n", key);
        return FAILURE;
    }
    return SUCCESS;
}

void msgq_cleanup(void)
{
    if (msgctl(gMsgQ_id, IPC_RMID, NULL) < 0) {
        return;
    }
    INFO("msgq deleted\n");
}

int msgq_recvfrom(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags)
{
    int ret;

    mbuf->len = 0;

    ret = msgrcv(gMsgQ_id, (void *)mbuf, len, mtype, (flags & CL_FLAG_NOWAIT) ? IPC_NOWAIT : 0);
    if (ret > 0 && ret + 4 < sizeof(msg_buf_t)) //Rahul: +4 is added for bins compiled with -m32. sizeof(long) issue.
    {
        ERROR("some problem ... msgrcv length(%d) not enough sizeof:%zu\n", ret, sizeof(msg_buf_t));
        return FAILURE;
    }
    return ret;
}

int msgq_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len)
{
    mbuf->mtype = mtype;
    if (msgsnd(gMsgQ_id, (void *)mbuf, len, 0) < 0) {
        ERROR("msgsend failed!\n");
        return FAILURE;
    }
    return SUCCESS;
}
