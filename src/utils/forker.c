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
 * @brief       Stackline process forker
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _FORKER_C_

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pty.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include "commline/commline.h"
#include "utils/forker_common.h"

child_psinfo_t g_child_info[MAX_CHILD_PROCESS];

void redirect_stdout_to_log(int nodeid)
{
    int  fd;
    char logfile[512];

    if (nodeid >= 0) {
        snprintf(logfile, sizeof(logfile), "%s/node_%04x.log", getenv("LOGPATH") ? getenv("LOGPATH") : "log", nodeid);
    } else {
        snprintf(logfile, sizeof(logfile), "%s/forker.log", getenv("LOGPATH") ? getenv("LOGPATH") : "log");
    }
    fd = open(logfile, O_TRUNC | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd > 0) {
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    }
}

int chk_executable(char *bin)
{
    struct stat st;
    if (stat(bin, &st)) {
        ERROR("Binary:[%s] DOES NOT EXIST!\n", bin);
        return FAILURE;
    }
    return SUCCESS;
}

#define SET_ARG_ENV(BUF)        \
    if (strstr(BUF, "PTY=1")) { \
        pty = 1;                \
    } else {                    \
        if (strchr(BUF, '=')) { \
            envp[e++] = BUF;    \
        } else {                \
            argv[i++] = BUF;    \
        }                       \
    }

int fork_n_exec(uint16_t nodeid, char *buf)
{
    char *argv[20] = { NULL }, *envp[20] = { NULL }, *ptr = NULL;
    int   i = 0, e = 0, pty = 0;

    INFO("fork_n_exec: buf=[%s]\n", buf);
    while ((ptr = strchr(buf, '|'))) {
        *ptr++ = 0;
        SET_ARG_ENV(buf);
        buf = ptr;
    }
    SET_ARG_ENV(buf);

    argv[i] = NULL;
    envp[e] = NULL;

    if (i < 1) {
        ERROR("Insufficient command exec info i=%d, e=%d\n", i, e);
        return FAILURE;
    }

    if (chk_executable(argv[0]))
        return -1;

    if (pty) {
        g_child_info[nodeid].pid = forkpty(&g_child_info[nodeid].master, NULL, NULL, NULL);
    } else {
        g_child_info[nodeid].pid = fork();
    }
    if (g_child_info[nodeid].pid < 0) {
        ERROR("fork failed!!!pty:%d\n", pty);
        return FAILURE;
    }
    if (0 == g_child_info[nodeid].pid) {
        prctl(PR_SET_PDEATHSIG, SIGKILL); //If forker dies then it should send SIGKILL to all kids i.e. stackline processes
        redirect_stdout_to_log(nodeid);
        execvpe(argv[0], argv, envp);
        ERROR("Could not execv [%s]. Check if the cmdname/path is correct.Aborting...\n", argv[0]);
        exit(0);
    }
#if 0
    if(pty) {
        struct termios tios;

        tcgetattr(g_child_info[nodeid].master, &tios);
        tios.c_lflag &= ~(ECHO | ECHONL);
        tcsetattr(g_child_info[nodeid].master, TCSAFLUSH, &tios);
        g_child_info[nodeid].uds_fd = uds_open(nodeid);
        if(g_child_info[nodeid].uds_fd < 0) {
            ERROR("Failed to open unix domain sock\n");
            //Dont bother to cleanup since everything will stop after this failure
            return FAILURE;
        }
        pty_add_fd(nodeid, g_child_info[nodeid].uds_fd, 0);
        pty_add_fd(nodeid, g_child_info[nodeid].master, 1);
    }
#endif
    return SUCCESS;
}

void killall_childprocess(void)
{
    int i;
    for (i = 0; i < MAX_CHILD_PROCESS; i++) {
        if (g_child_info[i].pid <= 0)
            continue;
        kill(g_child_info[i].pid, SIGINT);
    }
}

void wait_on_q(void)
{
    uint8_t    buf[sizeof(msg_buf_t) + COMMLINE_MAX_BUF];
    msg_buf_t *mbuf = (msg_buf_t *)buf;

    while (1) {
        if (cl_recvfrom_q(MTYPE(FORKER, CL_MGR_ID), mbuf, sizeof(buf), 0) < 0) {
            break;
        }
        if (mbuf->len) {
            if ((SUCCESS != fork_n_exec(mbuf->src_id, (char *)mbuf->buf)))
                break;
        }
    }
    killall_childprocess();
    INFO("Quitting forker process\n");
}

int main(void)
{
    redirect_stdout_to_log(-1);
    INFO("Starting forker...\n");
    if (SUCCESS != cl_init(MTYPE(FORKER, CL_MGR_ID), CL_ATTACHQ)) {
        ERROR("forker: failure to cl_init()\n");
        return 1;
    }
    if (SUCCESS != cl_bind(MTYPE(MONITOR, CL_MGR_ID))) {
        ERROR("forker: failure to cl_bind()\n");
        return 1;
    }
    if (SUCCESS != start_pty_thread()) {
        ERROR("start_monitor_thread failed... exiting process!!\n");
        return 1;
    }
    if (SUCCESS != start_monitor_thread()) {
        ERROR("start_monitor_thread failed... exiting process!!\n");
        return 1;
    }
    wait_on_q();
    return 0;
}
