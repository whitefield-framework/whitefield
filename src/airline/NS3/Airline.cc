#define	_AIRLINE_CC_

#include "common.h"
#include "Airline.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "common.h"

#include <ns3/lr-wpan-module.h>

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
	void Airline::tx(const uint8_t *pBuf, const size_t buflen)
	{
	};
	void Airline::setDeviceAddress(void)
	{
		Mac16Address address;
		uint8_t idBuf[2];
		uint16_t id = GetNode()->GetId();
		Ptr<LrWpanNetDevice> device = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();

		idBuf[0] = (id >> 8) & 0xff;
		idBuf[1] = (id >> 0) & 0xff;
		address.CopyFrom (idBuf);
		device->GetMac ()->SetShortAddress (address);
	};
	void Airline::StartApplication()
	{
		//INFO << "Airline application started ID:"<< GetNode()->GetId() << endl;
		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		setDeviceAddress();
		dev->GetMac()->SetMcpsDataConfirmCallback(MakeBoundCallback(&Airline::DataConfirm, this, dev));
		dev->GetMac()->SetMcpsDataIndicationCallback(MakeBoundCallback (&Airline::DataIndication, this, dev));
		SPAWN_STACKLINE(GetNode()->GetId());
		if(GetNode()->GetId() == 0) {
			SendSamplePacket();
		}
	};
	void Airline::SendSamplePacket()
	{
		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		Ptr<Packet> p0 = Create<Packet> (50);
		McpsDataRequestParams params;
		params.m_srcAddrMode = SHORT_ADDR;
		params.m_dstAddrMode = SHORT_ADDR;
		params.m_dstPanId = CFG_PANID;
		params.m_dstAddr = Mac16Address ("00:02");
		params.m_msduHandle = 0;
		params.m_txOptions = TX_OPTION_ACK;
		Simulator::ScheduleNow (&LrWpanMac::McpsDataRequest, dev->GetMac(), params, p0);
	};
	void Airline::StopApplication()
	{
		INFO << "Airline application stopped\n";
	};
	void Airline::DataIndication (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataIndicationParams params, Ptr<Packet> p)
	{
		INFO << "Wohoooo rcvd DataIndication on node:" << airline->GetNode()->GetId() << endl;
	};
	void Airline::DataConfirm (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataConfirmParams params)
	{
		INFO << "Wohoooo rcvd DataConfirm on node:" << airline->GetNode()->GetId() << endl;
	};
}

