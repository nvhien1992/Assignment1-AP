#include "file_handler.h"
#include "data_structures.h"

int main(int argc, char **argv) {
	char *file = "assignment1.input.txt";
	data_input_t din;

	parser(file, &din);

	return 0;
}
