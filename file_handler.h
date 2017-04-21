#ifndef __FILE_HANDLER_H
#define __FILE_HANDLER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include "err.h"
#include "data_structures.h"

errno_t parser(const char *fi_path, data_input_t *din);
errno_t writer(const char *fo_path, const data_output_t *dout);

#endif //__FILE_HANDLER_H
