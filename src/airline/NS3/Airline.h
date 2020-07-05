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

#ifndef _AIRLINE_H_
#define _AIRLINE_H_

#include <stdint.h>
#include <queue>
#include <ns3/application.h>
#include <ns3/event-id.h>
#include <ns3/ptr.h>
#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>

namespace ns3 {
class Airline : public Application {
public:
    static TypeId GetTypeId();
    Airline();
    virtual ~Airline(){};

private:
    /**
			 * \brief Start the application.
			 */
    virtual void StartApplication();

    /**
			 * \brief Stop the application.
			 */
    virtual void StopApplication();
    uint32_t     m_xyz; //delete in the future...
};

class AirlineHelper {
public:
    AirlineHelper()
    {
        m_factory.SetTypeId(Airline::GetTypeId());
    };

    /**
			 * \brief Install the application in Nodes.
			 * \param c list of Nodes
			 * \return application container
			 */
    ApplicationContainer Install(NodeContainer c)
    {
        ApplicationContainer apps;
        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
            Ptr<Node>    node   = *i;
            Ptr<Airline> client = m_factory.Create<Airline>();
            node->AddApplication(client);
            apps.Add(client);
        }
        return apps;
    }

private:
    /**
			 * \brief An object factory.
			 */
    ObjectFactory m_factory;
};
} // namespace ns3

#endif //	_AIRLINE_H_
