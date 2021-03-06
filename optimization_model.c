#include <math.h>
#include <stdlib.h>
#include "common.h"
#include "optimization_model.h"

#define DEBUG_EN 0
#include "dbg.h"

static const int NUM_BIN = 10;

static errno_t split_data(const data_input_t *din, int k, data_samples_t *d_trn, data_samples_t *d_tst);

static errno_t forecast_err_mean(const data_samples_t *d_tst, const factors_t *factors, double *fcast_em);

static errno_t histogram(const data_samples_t *d_tst, const factors_t *factors, double *his_data);

static errno_t split_data(const data_input_t *din, int k, data_samples_t *d_trn, data_samples_t *d_tst) {
	int size_in_bytes = 0;
	// num samples of first (k-1) folds
	int samples_of_fold = din->data_samples.num_samples / din->trn_params.num_folds;
	// num samples of last fold
	int samples_of_lfold = din->data_samples.num_samples - (samples_of_fold * (din->trn_params.num_folds - 1));

	d_tst->num_samples = (k==din->trn_params.num_folds) ? samples_of_lfold : samples_of_fold;
	d_trn->num_samples = din->data_samples.num_samples - d_tst->num_samples;

	// only allocate buffer for training data buffer
	d_tst->x_data = &din->data_samples.x_data[(k-1)*samples_of_fold];
	d_tst->t_data = &din->data_samples.t_data[(k-1)*samples_of_fold];

	size_in_bytes = sizeof(double)*d_trn->num_samples;
	d_trn->x_data = (double*) malloc(size_in_bytes);
	if (!d_trn->x_data) {
		return OUT_OF_MEM;
	}
	d_trn->t_data = (double*) malloc(size_in_bytes);
	if (!d_trn->x_data) {
		return OUT_OF_MEM;
	}

	if (k == 1) {
		memcpy(&d_trn->x_data[0], &din->data_samples.x_data[d_tst->num_samples], size_in_bytes);
		memcpy(&d_trn->t_data[0], &din->data_samples.t_data[d_tst->num_samples], size_in_bytes);
	} else if (k == din->trn_params.num_folds) {
		memcpy(&d_trn->x_data[0], &din->data_samples.x_data[0], size_in_bytes);
		memcpy(&d_trn->t_data[0], &din->data_samples.t_data[0], size_in_bytes);
	} else {
		size_in_bytes = sizeof(double)*(k-1)*samples_of_fold;
		memcpy(&d_trn->x_data[0], &din->data_samples.x_data[0], size_in_bytes);
		memcpy(&d_trn->t_data[0], &din->data_samples.t_data[0], size_in_bytes);

		size_in_bytes = sizeof(double)*(din->data_samples.num_samples-k*samples_of_fold);
		memcpy(&d_trn->x_data[(k-1)*samples_of_fold], &din->data_samples.x_data[k*d_tst->num_samples], size_in_bytes);
		memcpy(&d_trn->t_data[(k-1)*samples_of_fold], &din->data_samples.t_data[k*d_tst->num_samples], size_in_bytes);
	}

	return SUCCESS;
}

static errno_t forecast_err_mean(const data_samples_t *d_tst, const factors_t *factors, double *fcast_em) {
	long double sum_err = 0.0;
	int i;
	for (i = 0; i < d_tst->num_samples; i++) {
		sum_err += (factors->a * d_tst->x_data[i] + factors->b - d_tst->t_data[i])*(factors->a * d_tst->x_data[i] + factors->b - d_tst->t_data[i]);
	}
	*fcast_em = sqrt(sum_err/(double)d_tst->num_samples);

	return SUCCESS;
}

static errno_t histogram(const data_samples_t *d_tst, const factors_t *factors, double *his_data) {
	double err[d_tst->num_samples];
	double err_mean = 0.0;
	double deviation = 0.0;

	/* calculate error mean */
	int i;
	for (i = 0; i < d_tst->num_samples; i++) {
		err[i] = factors->a*d_tst->x_data[i] + factors->b - d_tst->t_data[i];
		err_mean += err[i];
	}
	err_mean = err_mean/(double)d_tst->num_samples;

	/* calculate deviation */
	for (i = 0; i < d_tst->num_samples; i++) {
		deviation += pow((err[i] - err_mean), 2);
	}
	deviation = sqrt(deviation/(double)d_tst->num_samples);

	/* sort err values into bins  */
	double Vmin = err_mean - 3*deviation;
	double Vmax = err_mean + 3*deviation;
	double bin_width = (Vmax - Vmin)/(double)NUM_BIN;

	int bin_arr[NUM_BIN];
	double pillar_arr[NUM_BIN+1];

	// Initialize bin array
	for (i = 0; i < NUM_BIN; i++) {
		bin_arr[i] = 0;
	}

	// Inittialize pillar array
	pillar_arr[0] = Vmin;
	pillar_arr[NUM_BIN] = Vmax;
	for (i = 1; i <= NUM_BIN-1; i++) {
		pillar_arr[i] = Vmin + i*bin_width;
	}

	// collect and sort into bin array
	int all_err_in_bin = 0;
	int pA, pB, ptmp;
	for (i = 0; i < d_tst->num_samples; i++) {
		if (err[i] >= Vmin && err[i] <= Vmax) {
			all_err_in_bin++;
			pA = 0;
			pB = NUM_BIN;
			ptmp = 0;
			while ((pB - pA) > 1) {
				ptmp = (pB+pA)/2;
				if (err[i] >= pillar_arr[ptmp]) {
					pA = ptmp;
					if (err[i] == pillar_arr[ptmp]) {
						break;
					}
				} else {
					pB = ptmp;
				}
			}
			bin_arr[pA]++;
		}
	}

	if (!all_err_in_bin) {
		DEBUG("sort err_data failed\n");
		return DATA_FATAL;
	}

	/* calculate histogram */
	for (i = 0; i < NUM_BIN; i++) {
		his_data[i] = (double)bin_arr[i]/(double)all_err_in_bin;
	}

	return SUCCESS;
}

errno_t gradient_descent(factors_t *sp, const int num_iters, const double lrn_rate, const data_samples_t *d_trn) {
	if (!sp || !d_trn) {
		return PARAM_INVALID;
	}

	double ga = 0.0;
	double gb = 0.0;
	double module_gv = 0.0;

	int ni, idx;
	for (ni = 0; ni < num_iters; ni++) {
		ga = 0.0;
		gb = 0.0;
		for (idx = 0; idx < d_trn->num_samples; idx++) {
			ga += (sp->a * d_trn->x_data[idx] + sp->b - d_trn->t_data[idx]) * d_trn->x_data[idx];
			gb += sp->a * d_trn->x_data[idx] + sp->b - d_trn->t_data[idx];
		}
		//DEBUG("grad: %.3f, %.3f\n", gv.ga, gv.gb);

		//normalize grad vector
		module_gv = sqrt(ga*ga + gb*gb);
		ga = (double)ga/module_gv;
		gb = (double)gb/module_gv;
		//DEBUG("norm-grad: %f %f %f\n", module_gv, gv.ga, gv.gb);

		sp->a = sp->a - lrn_rate * ga;
		sp->b = sp->b - lrn_rate * gb;
		//DEBUG("new startpoint: %f %f\n", sp->a, sp->b);
	}

	return SUCCESS;
}

errno_t validate_model(const data_input_t *din, data_output_t *dout) {
	if (!din || !dout) {
		return PARAM_INVALID;
	}

	dout->num_outputs = din->trn_params.num_folds;
	dout->lrning_oput = (learning_output_t*) malloc(sizeof(learning_output_t)*dout->num_outputs);
	if (!dout->lrning_oput) {
		return OUT_OF_MEM;
	}

	errno_t rv;
	double fem = 0.0;
	factors_t sp;
	data_samples_t d_trn, d_tst;
	int i, j;
	for (i = 1; i <= din->trn_params.num_folds; i++) {
		dout->lrning_oput[i-1].num_bin = NUM_BIN;

		// get training data and testing data
		if (rv = split_data(din, i, &d_trn, &d_tst) != SUCCESS) {
			DEBUG("split data failed\n");
			return rv;
		}

		// calculate factors a, b
		// reset startpoint
		sp.a = din->trn_params.start_point.a;
		sp.b = din->trn_params.start_point.b;
		gradient_descent(&sp, din->trn_params.num_iters, din->trn_params.learning_rate, &d_trn);
		dout->lrning_oput[i-1].factors.a = sp.a;
		dout->lrning_oput[i-1].factors.b = sp.b;

		// calculate forecast error mean
		forecast_err_mean(&d_tst, &sp, &fem);
		dout->lrning_oput[i-1].fcast_err_mean = fem;

		// get histogram
		dout->lrning_oput[i-1].histogram = (double*) malloc(sizeof(double)*NUM_BIN);
		if (!dout->lrning_oput[i-1].histogram) {
			FREE(d_trn.x_data);
			FREE(d_trn.t_data);
			return OUT_OF_MEM;
		}
		if (rv = histogram(&d_tst, &sp, dout->lrning_oput[i-1].histogram) != SUCCESS) {
			DEBUG("get histogram failed\n");
			return rv;
		}

		DEBUG("i=%d, %f %f %f ", i, sp.a, sp.b, fem);
		for (j = 0; j < NUM_BIN-1; j++) {
			DEBUG("%f ", dout->lrning_oput[i-1].histogram[j]);
		}
		DEBUG("%f\n", dout->lrning_oput[i-1].histogram[NUM_BIN-1]);

		// free all tmp buffer
		FREE(d_trn.x_data);
		FREE(d_trn.t_data);
	}

	return SUCCESS;
}
