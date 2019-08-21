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
 * @brief       Pcap helpers APIs
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _PCAP_UTIL_H_
#define _PCAP_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

void  pcap_write(void *handle, const uint8_t *buf, int buflen);
void *pcap_init(const char *fname);

#ifdef __cplusplus
}
#endif

#endif //_PCAP_UTIL_H_
