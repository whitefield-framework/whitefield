#ifndef	_COMMLINE_H_
#define	_COMMLINE_H_

#define	CL_SUCCESS	0
#define	CL_FAILURE	-1

#define	COMMLINE_MAX_BUF	2048

#define	CL_CREATEQ	(1<<0)	//Used by airline
#define	CL_ATTACHQ	(1<<1)	//Used by stackline
int cl_init(const uint8_t flags);
void cl_cleanup(void);
int cl_recvfrom_q(const uint16_t srcid, uint8_t *buf, uint16_t *buflen);

typedef struct _cl_mgr_info_
{
	uint16_t sndr_id;
}cl_mgr_info_t;

#define	CL_MANAGER_ID	0x7fff

#ifndef	ERROR
#define	ERROR printf
#define	INFO printf
#define	WARN printf
#endif

#endif	//_COMMLINE_H_
