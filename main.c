#include "file_handler.h"
#include "data_structures.h"
#include "optimization_model.h"

int main(int argc, char **argv) {
	char *file = "assignment1.input.txt";
	data_input_t din;

	parser(file, &din);

	gradient_descent(&din.trn_params.start_point, din.trn_params.num_iters, &din.data_samples, NULL);

	return 0;
}
