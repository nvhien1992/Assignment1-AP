#ifndef __ERR_H
#define __ERR_H

#include <stdio.h>

typedef enum {
	SUCCESS = 0,
	CMD_FAILED,
	PARAM_INVALID,
	FILE_NOT_FOUND,
} errno_t;

#endif //__ERR_H
