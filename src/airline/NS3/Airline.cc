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
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/lr-wpan-module.h"
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

	//convert 2byte short addr to Mac16Address object
	Mac16Address Airline::id2addr(const uint16_t id)
	{
		Mac16Address mac;
		uint8_t idstr[2], *ptr=(uint8_t*)&id;
		idstr[1] = ptr[0];
		idstr[0] = ptr[1];
		mac.CopyFrom(idstr);
		return mac;
	};

	uint16_t Airline::addr2id(const Mac16Address addr)
	{
		uint16_t id=0;
		uint8_t str[2], *ptr=(uint8_t *)&id;
		addr.CopyTo(str);
		ptr[1] = str[0];
		ptr[0] = str[1];
		return id;
	};

	//tx: usually called when packet is rcvd from node's stackline and to be sent on air interface
	void Airline::tx(msg_buf_t *mbuf)
	{
        int numNodes = stoi(CFG("numOfNodes"));
		McpsDataRequestParams params;

		if(mbuf->flags & MBUF_IS_CMD) {
			ERROR << "MBUF CMD not handled in Airline... No need!" << endl;
			return;
		}
		wf::Macstats::set_stats(AL_TX, mbuf);

		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		Ptr<Packet> p0 = Create<Packet> (mbuf->buf, (uint32_t)mbuf->len);
		params.m_srcAddrMode = SHORT_ADDR;
		params.m_dstAddrMode = SHORT_ADDR;
        params.m_dstPanId    = CFG_PANID;
		params.m_dstAddr     = id2addr(mbuf->dst_id);
		params.m_msduHandle  = 0;
		params.m_txOptions   = TX_OPTION_NONE;
		if(mbuf->dst_id != 0xffff) {
			params.m_txOptions = TX_OPTION_ACK;
		}

        // If the src node is in promiscuous mode then disable L2-ACK 
        if(IN_RANGE(mbuf->src_id, 0, numNodes)) {
            wf::Nodeinfo *ni=NULL;
            ni = WF_config.get_node_info(mbuf->src_id);
            if(ni && ni->getPromisMode()) {
                params.m_txOptions   = TX_OPTION_NONE;
            }
        }
#if AIRLINE_PRN_DATA
        INFO << "TX DATA: "
             << " src_id=" << GetNode()->GetId()
             << " dst_id=" << params.m_dstAddr
             << " pktlen=" << (int)mbuf->len
             << "\n";
        fflush(stdout);
#endif

		Simulator::ScheduleNow (&LrWpanMac::McpsDataRequest, dev->GetMac(), params, p0);
	};

	void Airline::setShortAddress(int shaddr)
	{
		Mac16Address address;
		uint8_t idBuf[2];
		uint16_t id = shaddr >= 0? (uint16_t)shaddr : GetNode()->GetId();
		Ptr<LrWpanNetDevice> device = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();

		idBuf[0] = (id >> 8) & 0xff;
		idBuf[1] = (id >> 0) & 0xff;
		address.CopyFrom (idBuf);
		device->GetMac()->SetShortAddress (address);
	};

	void Airline::setPanID(const uint16_t panid)
	{
		Ptr<LrWpanNetDevice> device = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();

        INFO << "Setting PAN ID:" << panid << "\n";
        fflush(stdout);
		device->GetMac()->SetPanId (panid);
	};

	void Airline::setExtendedAddress(const char *buf)
	{
		Mac64Address address(buf);
		Ptr<LrWpanNetDevice> device = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();

        INFO << "Setting Ext Addr:" << buf << "\n";
		device->GetMac()->SetExtendedAddress (address);
	};

	void Airline::StartApplication()
	{
		//INFO << "Airline application started ID:"<< GetNode()->GetId() << endl;
		Ptr<LrWpanNetDevice> dev = GetNode()->GetDevice(0)->GetObject<LrWpanNetDevice>();
		setShortAddress();
		dev->GetMac()->SetMacMaxFrameRetries(CFG_INT("macMaxRetry", 3));
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
		uint16_t node_id;
		DEFINE_MBUF(mbuf);

		node_id             = GetNode()->GetId();
		mbuf->len           = p->CopyData(mbuf->buf, COMMLINE_MAX_BUF);
		mbuf->src_id        = addr2id(params.m_srcAddr);
		mbuf->dst_id        = addr2id(params.m_dstAddr);
		mbuf->info.sig.lqi  = params.m_mpduLinkQuality;
		wf::Macstats::set_stats(AL_RX, mbuf);
		cl_sendto_q(MTYPE(STACKLINE, node_id), mbuf, sizeof(msg_buf_t) + mbuf->len);
#if AIRLINE_PRN_DATA
        INFO << "RX data"
             << " src_id=" << node_id
             << " dst_id=" << mbuf->dst_id
             << " lqi=" << (int)params.m_mpduLinkQuality
             << " len=" << mbuf->len
             << "\n";
#endif
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
        DEFINE_MBUF(mbuf);
        uint16_t dst_id;

        dst_id = addr2id(params.m_addrShortDstAddr);
        // We recv this callback for all packets. We ignore broadcasts.
        if(dst_id == 0xffff) {
            return;
        }
#if AIRLINE_PRN_DATA
        INFO << "Sending ACK status" 
             << " src=" << GetNode()->GetId()
             << " dst=" << dst_id
             << " status=" << params.m_status
             << " retries=" << (int)params.m_retries
             << " pktSize(inc mac-hdr)=" << params.m_pktSz
             << "\n";
        fflush(stdout);
#endif

        mbuf->src_id = GetNode()->GetId();
        mbuf->dst_id = dst_id;
        mbuf->info.ack.status = wf_ack_status(params.m_status);
        if(mbuf->info.ack.status == WF_STATUS_ACK_OK) {
            mbuf->info.ack.retries = params.m_retries+1;
        }
        mbuf->flags |= MBUF_IS_ACK;
        mbuf->len = 1;
        wf::Macstats::set_stats(AL_RX, mbuf);
        cl_sendto_q(MTYPE(STACKLINE, mbuf->src_id), mbuf, sizeof(msg_buf_t));
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
				m_macpktqlen = 20;
			}
		}
	};
}

