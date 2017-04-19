#include <math.h>
#include "optimization_model.h"

#define DEBUG_EN 1
#include "dbg.h"

static errno_t forecast_err_mean(const data_samples_t *d_tst, const factors_t *factors, float *fcast_em);

static errno_t forecast_err_mean(const data_samples_t *d_tst, const factors_t *factors, float *fcast_em) {
	float sum_err = 0.0;
	int i;
	for (i = 0; i < d_tst->num_samples; i++) {
		sum_err += pow((factors->a * d_tst->x_data[i] + factors->b - d_tst->t_data[i]), 2);
	}
	*fcast_em = sqrt(sum_err/(float)d_tst->num_samples);
}

errno_t gradient_descent(factors_t *sp, const int num_iters, const float lrn_rate, const data_samples_t *d_trn) {
	if (!sp || !d_trn) {
		return PARAM_INVALID;
	}

	grad_vector_t gv;
	gv.ga = 0.0f;
	gv.gb = 0.0f;

	float module_gv = 0.0;

	int ni, idx;
	for (ni = 0; ni < num_iters; ni++) {
		for (idx = 0; idx < d_trn->num_samples; idx++) {
			gv.ga += (sp->a * d_trn->x_data[idx] + sp->b - d_trn->t_data[idx]) * d_trn->x_data[idx];
			gv.gb += sp->a * d_trn->x_data[idx] + sp->b - d_trn->t_data[idx];
		}
		DEBUG("grad: %.3f, %.3f\n", gv.ga, gv.gb);

		//normalize grad vector
		module_gv = sqrt(pow(gv.ga, 2) + pow(gv.gb, 2));
		gv.ga = (float)gv.ga/(float)module_gv;
		gv.gb = (float)gv.gb/(float)module_gv;
		DEBUG("norm-grad: %f %f %f\n", module_gv, gv.ga, gv.gb);

		sp->a = sp->a - lrn_rate * gv.ga;
		sp->b = sp->b - lrn_rate * gv.gb;
		DEBUG("new startpoint: %f %f\n", sp->a, sp->b);
	}

	return SUCCESS;
}

errno_t validate_model(const data_input_t *din, data_output_t *dout) {
	if (!din || !dout) {
		return PARAM_INVALID;
	}

	dout->num_outputs = din->trn_params.num_folds;

	factors_t sp;
	data_samples_t d_trn, d_tst;
	d_trn.x_data = NULL;
	d_trn.t_data = NULL;
	d_tst.x_data = NULL;
	d_tst.t_data = NULL;

	// num samples of first (k-1) folds
	int samples_of_fold = din->data_samples.num_samples / din->trn_params.num_folds;
	// num samples of last fold
	int samples_of_lfold = din->data_samples.num_samples - (samples_of_fold * (din->trn_params.num_folds - 1));

	int i;
	for (i = 1; i <= din->trn_params.num_folds; i++) {
		d_tst.num_samples = (i==din->trn_params.num_folds) ? samples_of_lfold : samples_of_fold;
		d_trn.num_samples = din->data_samples.num_samples - d_tst.num_samples;
		d_trn.x_data = (float*) realloc(d_trn.x_data, sizeof(float)*d_trn.num_samples);
		d_trn.t_data = (float*) realloc(d_trn.t_data, sizeof(float)*d_trn.num_samples);
		if (i == 1) {
			memcpy((void*)d_trn.x_data, (void*)(din->data_samples.x_data[d_tst.num_samples]), sizeof(float)*d_trn.num_samples);
			memcpy((void*)d_trn.t_data, (void*)(din->data_samples.t_data[d_tst.num_samples]), sizeof(float)*d_trn.num_samples);
		} else if (i == din->trn_params.num_folds) {
			memcpy((void*)d_trn.x_data, (void*)din->data_samples.x_data[0], sizeof(float)*d_trn.num_samples);
			memcpy((void*)d_trn.t_data, (void*)din->data_samples.t_data[0], sizeof(float)*d_trn.num_samples);
		} else {
			memcpy((void*)d_trn.x_data, (void*)din->data_samples.x_data[0], sizeof(float)*(i-1)*samples_of_fold);
			memcpy((void*)d_trn.t_data, (void*)din->data_samples.t_data[0], sizeof(float)*(i-1)*samples_of_fold);
			memcpy((void*)d_trn.x_data[(i-1)*samples_of_fold], (void*)din->data_samples.x_data[i*d_tst.num_samples], sizeof(float)*(din->data_samples.num_samples-i*samples_of_fold));
			memcpy((void*)d_trn.t_data[(i-1)*samples_of_fold], (void*)din->data_samples.t_data[i*d_tst.num_samples], sizeof(float)*(din->data_samples.num_samples-i*samples_of_fold));
		}

		gradient_descent(&sp, din->trn_params.num_iters, din->trn_params.learning_rate, &d_trn);
	}
}
