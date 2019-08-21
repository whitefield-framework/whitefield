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

#ifndef _MAC_STATS_H_
#define _MAC_STATS_H_

#include <sys/time.h>
#include <common.h>
extern "C" {
#include "commline/commline.h"
}

#define MAX_MAC_TX_RETRY_CNT 10
namespace wf {
#define AL_RX 1
#define AL_TX 2
typedef struct _stats_ {
    uint64_t tx_pkts;
    uint64_t rx_pkts, rx_ack_ok[MAX_MAC_TX_RETRY_CNT], tx_fail;
    uint64_t tx_mc_pkts;
    uint64_t rx_mc_pkts;
} stats_t;
class Macstats {
private:
    // on data send : tx_data_pkts++
    // on data rcvd : rx_data_pkts++
    // on ack_ok(retry) rcvd: rx_ack_ok[retry]++
    // on receiving tx_failure: rx_tx_failure++

    // Successful pkts sent: tx_data_pkts - rx_tx_failure
    stats_t               stats;
    void                  set_rx_stats(const msg_buf_t *mbuf);
    void                  set_tx_stats(const msg_buf_t *mbuf);
    static struct timeval tv;
    static uint8_t        is_inited;
    void                  set(int dir, const msg_buf_t *mbuf);
    void                  reset(void);

protected:
    static void add_stats(stats_t &s, const stats_t &b)
    {
        s.tx_pkts += b.tx_pkts;
        s.rx_pkts += b.rx_pkts;
        s.tx_fail += b.tx_fail;
        s.tx_mc_pkts += b.tx_mc_pkts;
        s.rx_mc_pkts += b.rx_mc_pkts;
        for (int i = 0; i < MAX_MAC_TX_RETRY_CNT; i++) {
            s.rx_ack_ok[i] += b.rx_ack_ok[i];
        }
    };
    stats_t &get_stats(void)
    {
        return stats;
    };

public:
    static void set_stats(int dir, const msg_buf_t *mbuf);
    static int  get_summary(uint16_t id, char *buf, int buflen);
    Macstats()
    {
        memset(&stats, 0, sizeof(stats));
    };
    static void clear(void);
};
#ifdef _MAC_STATS_CC_
uint8_t        Macstats::is_inited = 0;
struct timeval Macstats::tv;
#endif
}; // namespace wf

#endif //	_MAC_STATS_H_
