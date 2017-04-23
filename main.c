#include "common.h"
#include "file_handler.h"
#include "data_structures.h"
#include "optimization_model.h"

int main(int argc, char **argv) {
	errno_t rv = SUCCESS;
	char *infile = "assignment1.input.txt";
	char *outfile = "assignment1.output.txt";

	if (argc == 2) {
		infile = argv[1];
	} else if (argc == 3) {
		infile = argv[1];
		outfile = argv[2];
	} else if (argc > 3) {
		printf("Too many arguement\n");
		return 1;
	}

	printf("infile: %s, outfile: %s\n", infile, outfile);

	data_input_t din;
	data_output_t dout;

	if (rv = parser(infile, &din) != SUCCESS) {
		printf("err: %s\n", STR(rv));
		goto out;
	}

	if (rv = validate_model(&din, &dout) != SUCCESS) {
		printf("err: %s\n", STR(rv));
		goto out;
	}

	if (rv = writer(outfile, &dout) != SUCCESS) {
		printf("err: %s\n", STR(rv));
		goto out;
	}

out:
	FREE(din.data_samples.x_data);
	FREE(din.data_samples.t_data);
	int i;
	for (i = 0; i < dout.num_outputs; i++) {
		FREE(dout.lrning_oput[i].histogram);
	}
	FREE(dout.lrning_oput);

	return 0;
}
