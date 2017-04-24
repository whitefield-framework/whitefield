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

	uint16_t Airline::addr2id(const Mac16Address addr)
	{
		uint16_t id=0;
		uint8_t str[2], *ptr=(uint8_t *)&id;
		addr.CopyTo(str);
		ptr[0] = str[1];
		ptr[1] = str[0];
		return id;
	};

	//tx: usually called when packet is rcvd from node's stackline and to be sent on air interface
	void Airline::tx(msg_buf_t *mbuf)
	{
		if(mbuf->flags & MBUF_IS_CMD) {
			INFO << "<TODO> Handle Node specific [" << (char*)mbuf->buf << "]\n";
			mbuf->len = sprintf((char*)mbuf->buf, "AL NODE RESPONSE");
			cl_sendto_q(MTYPE(MONITOR, CL_MGR_ID), mbuf, mbuf->len+sizeof(msg_buf_t));
			return;
		}
		wf::Macstats::set(AL_TX, mbuf);
		if(pktq.size() > m_macpktqlen) {
			ERROR << (int)m_macpktqlen << " pktq size exceeded!!\n";
			return;
		}

		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		Ptr<Packet> p0 = Create<Packet> (mbuf->buf, (uint32_t)mbuf->len);
		McpsDataRequestParams params;
		params.m_srcAddrMode = SHORT_ADDR;
		params.m_dstAddrMode = SHORT_ADDR;
		params.m_dstPanId = CFG_PANID;
		params.m_dstAddr = id2addr(mbuf->dst_id);
		params.m_msduHandle = 0;
		params.m_txOptions = TX_OPTION_NONE;
		if(mbuf->dst_id != 0xffff) {
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
		dev->GetMac()->SetMacMaxFrameRetries(stoi(CFG("macMaxRetry")));
		dev->GetMac()->SetMcpsDataConfirmCallback(MakeBoundCallback(&Airline::DataConfirm, this, dev));
		dev->GetMac()->SetMcpsDataIndicationCallback(MakeBoundCallback (&Airline::DataIndication, this, dev));
		SPAWN_STACKLINE(GetNode()->GetId());
	};

	void Airline::StopApplication()
	{
		INFO << "Airline application stopped\n";
	};

	void Airline::SendPacketToStackline(McpsDataIndicationParams & params, Ptr<Packet> p)
	{
		DEFINE_MBUF(mbuf);
		uint16_t node_id=GetNode()->GetId();

		mbuf->src_id = addr2id(params.m_srcAddr);
		mbuf->dst_id = addr2id(params.m_dstAddr);
		mbuf->sig.lqi = params.m_mpduLinkQuality;
		mbuf->len = p->CopyData(mbuf->buf, COMMLINE_MAX_BUF);
		wf::Macstats::set(AL_RX, mbuf);
		cl_sendto_q(MTYPE(STACKLINE, node_id), mbuf, sizeof(msg_buf_t) + mbuf->len);
	};

	void Airline::DataIndication (Airline *airline, Ptr<LrWpanNetDevice> dev, McpsDataIndicationParams params, Ptr<Packet> p)
	{
		/*INFO << "RX DATA node:" << airline->GetNode()->GetId()
			 << " LQI:" << (int)params.m_mpduLinkQuality
			 << " src:" << params.m_srcAddr
			 << " dst:" << params.m_dstAddr
			 << endl; 
			 */
		airline->SendPacketToStackline(params, p);
	};

	uint8_t Airline::wf_ack_status(LrWpanMcpsDataConfirmStatus status)
	{
		switch(status) {
			case IEEE_802_15_4_SUCCESS:
				return WF_STATUS_ACK_OK;
			case IEEE_802_15_4_NO_ACK:
				return WF_STATUS_NO_ACK;
			case IEEE_802_15_4_TRANSACTION_OVERFLOW:
			case IEEE_802_15_4_TRANSACTION_EXPIRED:
			case IEEE_802_15_4_CHANNEL_ACCESS_FAILURE:
				return WF_STATUS_ERR;	//can retry later
			case IEEE_802_15_4_INVALID_GTS:
			case IEEE_802_15_4_COUNTER_ERROR:
			case IEEE_802_15_4_FRAME_TOO_LONG:
			case IEEE_802_15_4_UNAVAILABLE_KEY:
			case IEEE_802_15_4_UNSUPPORTED_SECURITY:
			case IEEE_802_15_4_INVALID_PARAMETER:
			default:
				return WF_STATUS_FATAL;
		}
	};

	//Send the Ack status with retry count to stackline
	void Airline::SendAckToStackline(McpsDataConfirmParams & params)
	{
		if(pktq.empty()) {
			ERROR << "How can the pktq be empty on dataconfirm ind?? Investigate.\n";
			return;
		}
		McpsDataRequestParams drparams = pktq.front();
		if(drparams.m_txOptions == TX_OPTION_ACK) {
			DEFINE_MBUF(mbuf);

			mbuf->src_id = GetNode()->GetId();
			mbuf->dst_id = addr2id(drparams.m_dstAddr);
			mbuf->ack.status = wf_ack_status(params.m_status);
			if(mbuf->ack.status == WF_STATUS_ACK_OK) {
				mbuf->ack.retries = params.m_retries+1;
			}
			mbuf->flags |= MBUF_IS_ACK;
			mbuf->len = 1;
			wf::Macstats::set(AL_RX, mbuf);
			cl_sendto_q(MTYPE(STACKLINE, mbuf->src_id), mbuf, sizeof(msg_buf_t));
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
		airline->SendAckToStackline(params);
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

