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
 * @brief       Stackline helpers APIs
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _CL_STACKLINE_HELPERS_H_
#define _CL_STACKLINE_HELPERS_H_

#ifdef __cplusplus
extern "C" {
#endif

int      cl_get_id2longaddr(const uint16_t id, uint8_t *addr, const int addrlen);
uint16_t cl_get_longaddr2id(const uint8_t *addr);
void     sl_handle_cmd(msg_buf_t *mbuf);

#ifdef __cplusplus
}
#endif

#endif //_CL_STACKLINE_HELPERS_H_
