#ifndef	_COMMLINE_H_
#define	_COMMLINE_H_

#define	CL_SUCCESS	0
#define	CL_FAILURE	-1

#define	CL_CREATEQ	(1<<0)	//Used by airline
#define	CL_ATTACHQ	(1<<1)	//Used by stackline
int cl_init(const uint8_t flags);

#endif	//_COMMLINE_H_
