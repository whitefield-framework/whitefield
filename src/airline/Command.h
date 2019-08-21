/*
 * Copyright (C) 2017 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU
 * General Public License v2. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     airline
 * @{
 *
 * @file
 * @brief       OAM/Monitor command handler for Airline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "common.h"
extern "C" {
#include "commline/commline.h"
}

void al_handle_cmd(msg_buf_t *mbuf);

#endif // _COMMAND_H_
