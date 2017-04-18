#ifndef __DATA_STRUCTURE_H
#define __DATA_STRUCTURE_H

#include <stdio.h>
#include <string.h>

// data structures for input
typedef struct {
	float a;
	float b;
} start_point_t;

typedef struct {
	int num_iters;
	float learning_rate;
	start_point_t start_point;
	int num_folds;
} train_params_t;

typedef struct {
	int num_samples;
	float *x_data;
	float *t_data;
} data_samples_t;

typedef struct {
	train_params_t trn_params;
	data_samples_t data_samples;
} data_input_t;

//data structures for output
typedef struct {
	float a_factor;
	float b_factor;
	float forecast_err_mean;
	float *chart_data;
} learning_output_t;

typedef struct {
	int num_outputs;
	learning_output_t *lrning_oput;
} data_output_t;

#endif //__DATA_STRUCTURE_H
