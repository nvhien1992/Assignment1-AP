#ifndef __FILE_HANDLER_H
#define __FILE_HANDLER_H

#include "data_structures.h"

int parser(const char *fi_path, data_input_t *data_in_s);
int writer(const char *fo_path, const data_output_t *data_out_s);

#endif //__FILE_HANDLER_H
