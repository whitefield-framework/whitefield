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
 * @brief       Whitefield Manager
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#ifndef _MANAGER_H_
#define _MANAGER_H_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>

class Manager {
private:
    int startManager(wf::Config &cfg);

public:
    Manager(){};
    Manager(wf::Config &cfg);
};

#endif //_MANAGER_H_
