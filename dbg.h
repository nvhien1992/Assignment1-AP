#ifndef __DBG_H
#define __DBG_H

#include <stdio.h>

#if DEBUG_EN
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#if INFO_EN
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

#endif //__DBG_H
