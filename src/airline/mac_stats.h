#ifndef	_MAC_STATS_H_
#define	_MAC_STATS_H_

namespace ns3
{
#define	AL_RX	1
#define	AL_TX	2
	class Macstats
	{
		private:
		public:
			static void set(int dir, const msg_buf_t *mbuf);
	};
};

#endif //	_MAC_STATS_H_
