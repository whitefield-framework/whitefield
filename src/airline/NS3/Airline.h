#ifndef	_AIRLINE_H_
#define	_AIRLINE_H_

#include <stdint.h>
#include <ns3/application.h>
#include <ns3/event-id.h>
#include <ns3/ptr.h>
#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/lr-wpan-module.h>

namespace ns3
{
	class Airline : public Application
	{
		public:
			static void DataIndication (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataIndicationParams params, Ptr<Packet> p);
			static void DataConfirm (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataConfirmParams params);
			static TypeId GetTypeId();
			Airline() {
			};
			virtual ~Airline() {
			};
		private:
			void SendSamplePacket(void);
			/**
			 * \brief Start the application.
			 */
			virtual void StartApplication ();

			/**
			 * \brief Stop the application.
			 */
			virtual void StopApplication ();
			uint32_t m_xyz;	//delete in the future...
			void tx(const uint8_t *pBuf, const size_t buflen);
	};

	class AirlineHelper
	{
		public:
			AirlineHelper() {
				m_factory.SetTypeId(Airline::GetTypeId());
			};

			/**
			 * \brief Install the application in Nodes.
			 * \param c list of Nodes
			 * \return application container
			 */
			ApplicationContainer Install (NodeContainer c)
			{
				ApplicationContainer apps;
				for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
				{
					Ptr<Node> node = *i;
					Ptr<Airline> client = m_factory.Create<Airline> ();
					node->AddApplication (client);
					apps.Add (client);
				}
				return apps;
			}
		private:
			/**
			 * \brief An object factory.
			 */
			ObjectFactory m_factory;
	};
}

#endif //	_AIRLINE_H_
