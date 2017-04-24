#ifndef	_MAC_STATS_H_
#define	_MAC_STATS_H_

#include <sys/time.h>

#define	MAX_MAC_TX_RETRY_CNT	12
namespace wf
{
#define	AL_RX	1
#define	AL_TX	2
	class Macstats
	{
		private:
			// on data send : tx_data_pkts++
			// on data rcvd : rx_data_pkts++
			// on ack_ok(retry) rcvd: rx_ack_ok[retry]++
			// on receiving tx_failure: rx_tx_failure++

			// Successful pkts sent: tx_data_pkts - rx_tx_failure
			static uint64_t tx_data_pkts;
			static uint64_t rx_data_pkts, rx_ack_ok[MAX_MAC_TX_RETRY_CNT], rx_tx_failure;
			static uint64_t tx_mc_pkts;
			static uint64_t rx_mc_pkts;
			static void set_rx_stats(const msg_buf_t *mbuf);
			static void set_tx_stats(const msg_buf_t *mbuf);
			static uint8_t is_inited;
			static struct timeval tv;
		public:
			static void set(int dir, const msg_buf_t *mbuf);
			static int get_summary(char *buf, int buflen);
			static void clear(void);
	};
#ifdef	_MAC_STATS_CC_
	uint64_t Macstats::tx_data_pkts=0;
	uint64_t Macstats::rx_data_pkts, Macstats::rx_ack_ok[MAX_MAC_TX_RETRY_CNT], Macstats::rx_tx_failure;
	uint64_t Macstats::tx_mc_pkts=0, Macstats::rx_mc_pkts=0;
	uint8_t Macstats::is_inited=0;
	struct timeval Macstats::tv;
#endif
};

#endif //	_MAC_STATS_H_
