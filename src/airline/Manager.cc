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

#define _MANAGER_CC_

#include <Manager.h>
#include <AirlineManager.h>

Manager::Manager(wf::Config & cfg)
{
	startManager(cfg);
}

int Manager::startManager(wf::Config & cfg)
{
	try {
		AirlineManager airlineManager(cfg);
	} catch (exception & e) {
		CERROR << "Caught exception " << e.what() << endl;
		return FAILURE;
	}
	return SUCCESS;
}

