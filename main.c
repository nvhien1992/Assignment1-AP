#include "file_handler.h"
#include "data_structures.h"
#include "optimization_model.h"

int main(int argc, char **argv) {
	char *file = "assignment1.input.txt";
	data_input_t din;
	data_output_t dout;

	parser(file, &din);

	validate_model(&din, &dout);

	writer("assignment1.output.txt", &dout);

	return 0;
}
