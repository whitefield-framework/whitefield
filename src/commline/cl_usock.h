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
 * @brief       Abstract Unix domain sockets for commline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _CL_USOCK_H_
#define _CL_USOCK_H_

int  usock_init(const long my_mtype, const uint8_t flags);
void usock_cleanup(void);
int  usock_recvfrom(const long mtype, msg_buf_t *mbuf, uint16_t len, uint16_t flags);
int  usock_sendto(const long mtype, msg_buf_t *mbuf, uint16_t len);
int  usock_get_descriptor(const long mtype);

#define CL_INIT           usock_init
#define CL_CLEANUP        usock_cleanup
#define CL_RECVFROM       usock_recvfrom
#define CL_SENDTO         usock_sendto
#define CL_GET_DESCRIPTOR usock_get_descriptor

#endif //_CL_MSGQ_H_
