#define	_AIRLINE_CC_

#include "common.h"
#include "Airline.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/lr-wpan-module.h"

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

	//convert 2byte short addr to Mac16Address object
	Mac16Address Airline::id2addr(const uint16_t id)
	{
		Mac16Address mac;
		uint8_t idstr[2], *ptr=(uint8_t*)&id;
		idstr[0] = ptr[0];
		idstr[1] = ptr[1];
		mac.CopyFrom(idstr);
		return mac;
	};

	//tx: usually called when packet is rcvd from node's stackline and to be sent on air interface
	void Airline::tx(const uint16_t dst_id, const uint8_t *pBuf, const size_t buflen)
	{
		if(pktq.size() > m_macpktqlen) {
			ERROR << (int)m_macpktqlen << " pktq size exceeded!!\n";
			return;
		}

		INFO << "sending pkt>> dst:" << dst_id 
			 << " len:" << buflen
			 << endl;
		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		Ptr<Packet> p0 = Create<Packet> (pBuf, (uint32_t)buflen);
		McpsDataRequestParams params;
		params.m_srcAddrMode = SHORT_ADDR;
		params.m_dstAddrMode = SHORT_ADDR;
		params.m_dstPanId = CFG_PANID;
		params.m_dstAddr = id2addr(dst_id);
		params.m_msduHandle = 0;
		params.m_txOptions = TX_OPTION_NONE;
		if(dst_id != 0xffff) {
			params.m_txOptions = TX_OPTION_ACK;
		}
		pktq.push(params);
		Simulator::ScheduleNow (&LrWpanMac::McpsDataRequest, dev->GetMac(), params, p0);
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
	/*	if(GetNode()->GetId() == 0) {
			SendSamplePacket();
		} */
	};

	void Airline::SendSamplePacket()
	{
		uint8_t buf[50]={0};
		tx(0xffff, buf, sizeof(buf));
	};

	void Airline::StopApplication()
	{
		INFO << "Airline application stopped\n";
	};

	void Airline::DataIndication (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataIndicationParams params, Ptr<Packet> p)
	{
		INFO << "RX DATA node:" << airline->GetNode()->GetId()
			 << " LQI:" << (int)params.m_mpduLinkQuality
			 << " src:" << params.m_srcAddr
			 << " dst:" << params.m_dstAddr
			 << endl;
	};

	//Send the Ack status with retry count to stackline
	void Airline::SendAckToStackline(void) 
	{
		if(pktq.empty()) {
			ERROR << "How can the pktq be empty on dataconfirm ind?? Investigate.\n";
			return;
		}
		McpsDataRequestParams params = pktq.front();
		if(params.m_txOptions == TX_OPTION_ACK) {
			//handle ACK
			INFO << "TODO Handle DataConfirm Indication\n";
		}
		pktq.pop();
	};

	//MAC layer Ack handling
	void Airline::DataConfirm (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataConfirmParams params)
	{
	//	INFO << "RX ACK node:" << airline->GetNode()->GetId()
	//		 << " Confirm:" << params.m_status
	//		 << " Retries:" << (int)params.m_retries
	//		 << " msdu:" << (int)params.m_msduHandle
	//		 << endl;
		airline->SendAckToStackline();
	};

	Airline::Airline() {
		if(!m_macpktqlen) {
			string dstr = CFG("macPktQlen");
			if(!dstr.empty()) {
				m_macpktqlen = (uint8_t)stoi(dstr);
			} else {
				m_macpktqlen = 10;
			}
		}
	};
}

