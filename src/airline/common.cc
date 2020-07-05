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
 * @brief       Common/utility APIs for Airline
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 *
 * @}
 */

#define _COMMON_CC_

#include <common.h>
#include <Nodeinfo.h>
#include <Config.h>

// trim from left
string& ltrim(string& s, const char* t)
{
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// trim from right
string& rtrim(string& s, const char* t)
{
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// trim from left & right
string& trim(string& s, const char* t)
{
  return ltrim(rtrim(s, t), t);
}

template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

map<string, string, ci_less> splitKV(string & s)
{
    map<string, string, ci_less> m;

    string key, val;
    istringstream iss(s);

    while(getline(getline(iss, key, '=') >> ws, val, ','))
        m[key] = val;
    return m;
}

/* Gets the string for the corr key and removes that map entry */
string getMapCfg(map<string, string, ci_less> & m, string key)
{
    string val;

    if (m.find(key) == m.end()) {
        return val;
    }
    val = m[key];
    m.erase(key);
    CINFO << "param: " << key << "=" << val << "\n";
    return val;
}

void SendAckToStackline(uint16_t src_id, uint16_t dst_id,
        uint8_t status, int retries)
{
    DEFINE_MBUF(mbuf);

    mbuf->src_id = src_id;
    mbuf->dst_id = dst_id;
    mbuf->info.ack.status = status;
    if(mbuf->info.ack.status == WF_STATUS_ACK_OK) {
        mbuf->info.ack.retries = retries;
    }
    mbuf->flags |= MBUF_IS_ACK;
    mbuf->len = 1;
    wf::Macstats::set_stats(AL_RX, mbuf);
    cl_sendto_q(MTYPE(STACKLINE, mbuf->src_id), mbuf, sizeof(msg_buf_t));
}

void SendPacketToStackline(uint16_t id, msg_buf_t *mbuf)
{
    wf::Macstats::set_stats(AL_RX, mbuf);
    cl_sendto_q(MTYPE(STACKLINE, id), mbuf, sizeof(msg_buf_t) + mbuf->len);
#if 0
    CINFO << "RX data"
         << " src_id=" << id << " dst_id=" << mbuf->dst_id
         << " lqi=" << (int)lqi << " rssi=" << (int)rssi
         << " len=" << mbuf->len << "\n";
#endif
}

