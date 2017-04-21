#ifndef	_COMMLINE_H_
#define	_COMMLINE_H_

#include <sys/time.h>

#define	CL_SUCCESS	0
#define	CL_FAILURE	-1

#define	COMMLINE_MAX_BUF	2048

#define	CL_CREATEQ	(1<<0)	//Used by airline
#define	CL_ATTACHQ	(1<<1)	//Used by stackline

int cl_init(const uint8_t flags);
void cl_cleanup(void);

//msg_buf_t::flags defined
#define	MBUF_IS_ACK	(1<<0)

typedef struct _msg_buf_
{
	long mtype;
	uint16_t src_id;
	uint16_t dst_id;
	uint8_t flags;
	union {
		struct {
			uint8_t lqi;	//Link quality indicator 
			uint8_t rssi;
		}sig;
		struct {
			uint8_t retries;
			uint8_t status;
		}ack;
	};
	uint16_t len; // length of the buf only
	uint8_t buf[1];
}msg_buf_t;

#define	DEFINE_MBUF(MBUF)	\
		uint8_t MBUF##_buf[sizeof(msg_buf_t)+COMMLINE_MAX_BUF];\
		msg_buf_t *MBUF = (msg_buf_t *)MBUF##_buf;\
		memset(MBUF##_buf, 0, sizeof(MBUF##_buf));

int cl_recvfrom_q(const long mtype, msg_buf_t *mbuf, uint16_t len);
int cl_sendto_q(const long mtype, msg_buf_t *mbuf, uint16_t len);

enum {
	STACKLINE=1,
	AIRLINE,
	FORKER,
};

#define	MTYPE(LINE,ID)	(((LINE)<<16)|(ID))

#ifndef	ERROR
#define	PRN(STR, ...)	\
	{\
		struct timeval tv;\
		gettimeofday(&tv, NULL);\
		printf("%s [%ld:%ld] [%s:%d] ", STR, tv.tv_sec, tv.tv_usec, __FUNCTION__, __LINE__);\
		printf(__VA_ARGS__);\
		fflush(stdout);\
	}
#define	ERROR(...) PRN("ERROR", __VA_ARGS__)
#define	INFO(...)  PRN("INFO ", __VA_ARGS__)
#define	WARN(...)  PRN("WARN "__VA_ARGS__)
#endif //ERROR

/* MAC DataConfirmation status */
enum {
	WF_STATUS_ACK_OK,
	WF_STATUS_NO_ACK,
	WF_STATUS_ERR,
	WF_STATUS_FATAL,
};

#endif	//_COMMLINE_H_
