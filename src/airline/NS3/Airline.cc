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
 * @brief       NS3 specific airline module for handling virtual airline nodes.
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_AIRLINE_CC_

#include "common.h"
#include "Airline.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include <Nodeinfo.h>
#include <Config.h>

namespace ns3
{
	NS_LOG_COMPONENT_DEFINE ("AirlineApp");

	NS_OBJECT_ENSURE_REGISTERED (Airline);

	TypeId Airline::GetTypeId ()
	{
		static TypeId tid = TypeId ("ns3::Airline")
			.SetParent<Application>()
			.SetGroupName("Whitefield")
			.AddConstructor<Airline>()
			.AddAttribute ("xyz", 
					"arbitrary attribute to be defined",
					UintegerValue (100),
					MakeUintegerAccessor (&Airline::m_xyz),
					MakeUintegerChecker<uint32_t>())
			;
		return tid;
	};

	void Airline::StartApplication()
	{
		SPAWN_STACKLINE(GetNode()->GetId());
	};

	void Airline::StopApplication()
	{
		CINFO << "Airline application stopped\n";
	};

	Airline::Airline() {
	};
}

