#ifndef __OPTIMIZATION_MODEL_H
#define __OPTIMIZATION_MODEL_H

#include "data_structures.h"
#include "err.h"

errno_t gradient_descent(start_point_t *sp, const int num_iters, const data_samples_t *d_trn, learning_output_t *lrn_oput);

#endif //__OPTIMIZATION_MODEL_H
