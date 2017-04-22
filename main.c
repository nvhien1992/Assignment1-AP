#include "common.h"
#include "file_handler.h"
#include "data_structures.h"
#include "optimization_model.h"

int main(int argc, char **argv) {
	char *infile = "assignment1.input.txt";
	char *outfile = "assignment1.output.txt";
	data_input_t din;
	data_output_t dout;

	parser(infile, &din);

	validate_model(&din, &dout);

	writer(outfile, &dout);

	FREE(din.data_samples.x_data);
	FREE(din.data_samples.t_data);
	int i;
	for (i = 0; i < dout.num_outputs; i++) {
		FREE(dout.lrning_oput[i].histogram);
	}
	FREE(dout.lrning_oput);

	return 0;
}
