#ifndef _FORKER_COMMON_H_
#define _FORKER_COMMON_H_

#include <sys/un.h>

#define MAX_CHILD_PROCESS 2000
typedef struct _child_info_ {
    pid_t              pid;
    int                master;
    int                uds_fd;
    struct sockaddr_un peer;
    socklen_t          peerlen;
} child_psinfo_t;

// pty_handler.c exported functions
int start_pty_thread(void);
int pty_add_fd(int nodeid, int fd, int ismaster);
int uds_open(int nodeid);

// monitor.c exported functions
int start_monitor_thread(void);

#endif // _FORKER_COMMON_H_
