#ifndef __OPTIMIZATION_MODEL_H
#define __OPTIMIZATION_MODEL_H

#include "data_structures.h"
#include "err.h"

errno_t gradient_descent(factors_t *sp, const int num_iters, const float lrn_rate, const data_samples_t *d_trn);

errno_t validate_model(const data_input_t *din, data_output_t *dout);

#endif //__OPTIMIZATION_MODEL_H
