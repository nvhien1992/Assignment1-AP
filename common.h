#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>

#define STR(name)	#name
#define FREE(ptr)	{if(ptr) free(ptr);}

#endif //__COMMON_H
