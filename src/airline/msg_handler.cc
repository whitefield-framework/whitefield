#define	_MSG_HANDLER_CC_

#include "common.h"
#include "msg_handler.h"
extern "C" {
#include "commline/commline.h"
}

void commline_thread(void)
{
	uint8_t buf[sizeof(msg_buf_t) + COMMLINE_MAX_BUF];
	msg_buf_t *mbuf=(msg_buf_t*)buf;
	int slptime=1;

	INFO << "Commline Thread created\n";
	while(1)
	{
		usleep(slptime);

		if(CL_SUCCESS!=cl_recvfrom_q(MTYPE(AIRLINE,0), mbuf, sizeof(buf))) {
			break;
		}
		slptime=mbuf->len?1:1000;
		if(mbuf->len) {
			INFO << "rcvd packet len:" << mbuf->len
				 << " src:" << mbuf->src_id 
				 << " dst:" << mbuf->dst_id 
				 << endl;
		}
#if 0
		{
			NodeContainer const & n = NodeContainer::GetGlobal (); 
			Ptr<Application> nodeApp = n.Get(2)->GetApplication(0);
			if(nodeApp) {
				Ptr<Airline> aline = DynamicCast<Airline> (nodeApp);
				aline->rxPacketFromStackline(buf, len);
			}
		}
#endif
	}
	INFO << "stopping commline_thread\n";
}

