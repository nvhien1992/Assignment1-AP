#include <math.h>
#include "optimization_model.h"

#define DEBUG_EN 1
#include "dbg.h"

static void derivative(const start_point_t *sp, const data_samples_t *d_trn, grad_vector_t *gv);

static void derivative(const start_point_t *sp, const data_samples_t *d_trn, grad_vector_t *gv) {
	if (!sp || !d_trn || !gv) {
		return;
	}
	gv->ga = 0.0f;
	gv->gb = 0.0f;

	int i = 0;
	for (i = 0; i < d_trn->num_samples; i++) {
		gv->ga += (sp->a * d_trn->x_data[i] + sp->b - d_trn->t_data[i]) * d_trn->x_data[i];
		gv->gb += sp->a * d_trn->x_data[i] + sp->b - d_trn->t_data[i];
	}
}

errno_t gradient_descent(start_point_t *sp, const int num_iters, const data_samples_t *d_trn, learning_output_t *lrn_oput) {
	if (!sp || !d_trn) {
		return PARAM_INVALID;
	}
	grad_vector_t gv;
	derivative(sp, d_trn, &gv);
	DEBUG("ga: %.3f, gb: %.3f\n", gv.ga, gv.gb);
	//normalize grad vector
	float module_gv = sqrt(pow(gv.ga, 2) + pow(gv.gb, 2));
	DEBUG("%f\n", module_gv);
	gv.ga = (float)gv.ga/(float)module_gv;
	gv.gb = (float)gv.gb/(float)module_gv;
	DEBUG("%f %f\n", gv.ga, gv.gb);
}
