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
 * @brief       Mac statistics management
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define	_MAC_STATS_CC_

#include "common.h"
#include "Nodeinfo.h"
#include "Config.h"

namespace wf {
	void Macstats::set_tx_stats(const msg_buf_t *mbuf)
	{
		if(mbuf->flags & MBUF_IS_CMD) {
			return;
		}
		if(mbuf->dst_id == 0xffff) {
			stats.tx_mc_pkts++;
		} else {
			stats.tx_pkts++;
		}
	};

	void Macstats::set_rx_stats(const msg_buf_t *mbuf)
	{
		if(mbuf->flags & MBUF_IS_CMD) {
			return;
		}
		if(mbuf->flags & MBUF_IS_ACK) {
			if(mbuf->info.ack.status == WF_STATUS_ACK_OK) {
				if(IN_RANGE(mbuf->info.ack.retries, 1, MAX_MAC_TX_RETRY_CNT)) {
					stats.rx_ack_ok[mbuf->info.ack.retries]++;
				} else {
					CERROR << "Stats failure since ACK_OK with retry:" 
						  << (int)mbuf->info.ack.retries << endl;
					return;
				}
			} else {
				stats.tx_fail++;
			}
		} else {
			if(mbuf->dst_id) {
				stats.rx_mc_pkts++;
			} else {
				stats.rx_pkts++;
			}
		}
	};

	void Macstats::clear(void)
	{
		Nodeinfo *ni=NULL;
		gettimeofday(&tv, NULL);
		for(uint16_t i=0;;i++) {
			ni=WF_config.get_node_info(i);
			if(!ni) break;
			ni->reset();
		}
	};

	int Macstats::get_summary(uint16_t id, char *buf, int buflen)
	{
		stats_t st;
		Nodeinfo *ni=NULL;
		int n=0, mac_retries=stoi(CFG("macMaxRetry"));

		if(id == CL_MGR_ID) {
			struct timeval etv;
			gettimeofday(&etv, NULL);
			memset(&st, 0, sizeof(st));
			for(uint16_t i=0;;i++) {
				ni=WF_config.get_node_info(i);
				if(!ni) break;
				add_stats(st, ni->get_stats());
			}
			n += snprintf(buf+n, buflen-n-1, "Airline MAC stats: duration=%ld\n",
						etv.tv_sec-tv.tv_sec);
		} else {
			ni=WF_config.get_node_info(id);
			if(!ni) {
				CERROR << "Could not retry Nodeinfo for id:" << (int)id << endl;
				return snprintf(buf, buflen, "INTERNAL_ERROR");
			}
			st=ni->get_stats();
		}
		n += snprintf(buf+n, buflen-n-1, 
					"MCAST_PKTS: rx=%ld,tx=%ld\n"
					"UCAST_PKTS: rx=%ld,tx=%ld,tx_succ=%ld,tx_fail=%ld",
					st.rx_mc_pkts, st.tx_mc_pkts,
					st.rx_pkts, st.tx_pkts, st.tx_pkts-st.tx_fail,st.tx_fail);
		for(int i=1; i<=mac_retries; i++) {
			n += snprintf(buf+n, buflen-n-1, ",tx_attempt%d=%ld", 
						i, st.rx_ack_ok[i]);
		}
		return n;
	};

	void Macstats::reset(void)
	{
		memset(&stats, 0, sizeof(stats));
	};

	void Macstats::set(int dir, const msg_buf_t *mbuf)
	{
		if(!is_inited) {
			clear();
			is_inited = 1;
		}
		if(AL_TX == dir) {
			set_tx_stats(mbuf);
		} else if(AL_RX == dir) {
			set_rx_stats(mbuf);
		} else {
			CERROR << "UNKNOW DIR " << dir << endl;
		}
	};

	void Macstats::set_stats(int dir, const msg_buf_t *mbuf) {
		Nodeinfo *ni=WF_config.get_node_info(mbuf->src_id);
		if(!ni) {
			CERROR << "Invalida src_id in set_stats: " << (int)mbuf->src_id << endl;
			return;
		}
		ni->set(dir, mbuf);
	};

} //namespace ns3
