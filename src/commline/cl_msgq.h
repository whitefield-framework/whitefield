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

#ifndef _CL_MSGQ_H_
#define _CL_MSGQ_H_

int  msgq_init(const long my_mtype, const uint8_t flags);
void msgq_cleanup(void);
int  msgq_recvfrom(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags);
int  msgq_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len);

#define CL_INIT     msgq_init
#define CL_CLEANUP  msgq_cleanup
#define CL_RECVFROM msgq_recvfrom
#define CL_SENDTO   msgq_sendto

#endif //_CL_MSGQ_H_
