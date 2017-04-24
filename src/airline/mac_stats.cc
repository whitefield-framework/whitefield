#define	_MAC_STATS_CC_

#include "common.h"

extern "C" {
#include "commline/commline.h"
}
#include "mac_stats.h"

namespace wf {
	void Macstats::set_tx_stats(const msg_buf_t *mbuf)
	{
		if(mbuf->flags & MBUF_IS_CMD) {
			return;
		}
		if(mbuf->dst_id == 0xffff) {
			tx_mc_pkts++;
		} else {
			tx_data_pkts++;
		}
	};

	void Macstats::set_rx_stats(const msg_buf_t *mbuf)
	{
		if(mbuf->flags & MBUF_IS_CMD) {
			return;
		}
		if(mbuf->flags & MBUF_IS_ACK) {
			if(mbuf->ack.status == WF_STATUS_ACK_OK) {
				if(IN_RANGE(mbuf->ack.retries, 1, MAX_MAC_TX_RETRY_CNT)) {
					rx_ack_ok[mbuf->ack.retries]++;
				} else {
					ERROR << "Stats failure since ACK_OK with retry:" 
						  << (int)mbuf->ack.retries << endl;
					return;
				}
			} else {
				rx_tx_failure++;
			}
		} else {
			if(mbuf->dst_id) {
				rx_mc_pkts++;
			} else {
				rx_data_pkts++;
			}
		}
	};

	void Macstats::clear(void)
	{
		tx_data_pkts = rx_tx_failure = 0;
		rx_data_pkts = 0;
		tx_mc_pkts = rx_mc_pkts = 0;
		memset(rx_ack_ok, 0, sizeof(rx_ack_ok));
		gettimeofday(&tv, NULL);
	};

	int Macstats::get_summary(char *buf, int buflen)
	{
		int n, mac_retries=stoi(CFG("macMaxRetry"));
		struct timeval etv;
		gettimeofday(&etv, NULL);
		n = snprintf(buf, buflen-1, 
					"Airline MAC stats: duration=%ld\n"
					"MCAST_PKTS: rx=%ld,tx=%ld\n"
					"UCAST_PKTS: rx=%ld,tx=%ld,tx_succ=%ld,tx_fail=%ld",
					etv.tv_sec-tv.tv_sec,
					rx_mc_pkts, tx_mc_pkts,
					rx_data_pkts, tx_data_pkts, tx_data_pkts-rx_tx_failure,rx_tx_failure);
		for(int i=1; i<=mac_retries; i++) {
			n += snprintf(buf+n, buflen-n-1, ",tx_attempt%d=%ld", 
						i, rx_ack_ok[i]);
		}
		return n;
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
			ERROR << "UNKNOW DIR " << dir << endl;
		}
	};
} //namespace ns3
