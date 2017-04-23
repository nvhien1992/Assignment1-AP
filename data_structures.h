#ifndef __DATA_STRUCTURE_H
#define __DATA_STRUCTURE_H

#include <stdio.h>
#include <string.h>

// data structures for input
typedef struct {
	double a;
	double b;
} factors_t;

typedef struct {
	int num_iters;
	double learning_rate;
	factors_t start_point;
	int num_folds;
} train_params_t;

typedef struct {
	int num_samples;
	double *x_data;
	double *t_data;
} data_samples_t;

typedef struct {
	train_params_t trn_params;
	data_samples_t data_samples;
} data_input_t;

//data structures for output
typedef struct {
	double ga;
	double gb;
} grad_vector_t;

typedef struct {
	factors_t factors;
	double fcast_err_mean;
	double *histogram;
	int num_bin;
} learning_output_t;

typedef struct {
	int num_outputs;
	learning_output_t *lrning_oput;
} data_output_t;

#endif //__DATA_STRUCTURE_H
