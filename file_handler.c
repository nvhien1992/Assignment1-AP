#include "file_handler.h"

static errno_t runcmd_shell(const char *format, ...);
static errno_t get_params(const char *file, train_params_t *params);
static errno_t get_data_samples(const char *file, data_samples_t *data);

static errno_t runcmd_shell(const char *format, ...) {
	char cmd_buff[1024];
	va_list args;

	va_start(args, format);
	if (vsprintf(cmd_buff, format, args) < 0) {
		return CMD_FAILED;
	}
	va_end(args);

	if (system(cmd_buff) < 0) {
		return CMD_FAILED;
	}

	return SUCCESS;
}


static errno_t get_params(const char *file, train_params_t *params) {
	FILE *fp;
	const char *pattern = "%d\n%f\n%f\n%f\n%d";

	fp = fopen(file, "r");
	if (!fp) {
		printf("open file .params.tmp failed\n");
		return OPEN_FILE_FAILED;
	}

	if (fscanf(fp, pattern, &params->num_iters, &params->learning_rate,
		    &params->start_point_s.a, &params->start_point_s.b,
		    &params->num_folds) == 0) {
		printf("parse failed\n");
		fclose(fp);
		return PARSE_FAILED;
	}
	fclose(fp);
	return SUCCESS;
}

static errno_t get_data_samples(const char *file, data_samples_t *data) {
	FILE *fp;
	char line[256];
	int num_samples = 0;

	fp = fopen(file, "r");
	if (!fp) {
		printf("open file .data.tmp failed\n");
		return OPEN_FILE_FAILED;
	}

	fgets(line, sizeof(line), fp);
	sscanf(line, "%d\n", &num_samples);
	data->num_samples = num_samples;

	data->x_data = (float*) malloc(sizeof(float)*num_samples);
	data->t_data = (float*) malloc(sizeof(float)*num_samples);

	num_samples = 0;
	while (line[0] != '-') {
		fgets(line, sizeof(line), fp);
		sscanf(line, "%f\n", &data->x_data[num_samples]);
		num_samples++;
	}
	if (num_samples != data->num_samples) {
		data->num_samples = -1;
		return DATA_FATAL;
	}

	num_samples = 0;
	while (fgets(line, sizeof(line), fp)) {
		scanf(line, "%f\n", &data->t_data[num_samples]);
		num_samples++;
	}
	if (num_samples != data->num_samples) {
		data->num_samples = -1;
		return DATA_FATAL;
	}

	fclose(fp);
	return SUCCESS;
}

errno_t parser(const char *fi_path, data_input_t *data_in_s) {
	errno_t retval = SUCCESS;

	if (!fi_path || !data_in_s) {
		return PARAM_INVALID;
	}

	if (access(fi_path, F_OK) != 0) {
		return FILE_NOT_FOUND;
	}

	// get parameters from input file and save to .params.tmp
	retval = runcmd_shell("sed -n '4,8p' %s | awk '{print $2}' > .params.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}
	get_params(".params.tmp", &data_in_s->trn_params);
	printf("%.3f\n", data_in_s->trn_params.learning_rate);

	// get num of samples
	retval = runcmd_shell("sed -n '12,$p' %s | wc -l > .data.tmp", fi_path);

	// get x data from input file and save to .data.tmp
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $1}' >> .data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	// seperate x_data and t_data by '-'
	retval = runcmd_shell("echo '-' >> .data.tmp");
	if (retval != SUCCESS) {
		return retval;
	}

	// get t data from input file and save to another file
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $2}' >> .data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	get_data_samples(".data.tmp", &data_in_s->data_samples);

	int i = 0;
	for (i=0; i<data_in_s->data_samples.num_samples; i++) {
		printf("%f %f\n", data_in_s->data_samples.x_data[i], data_in_s->data_samples.t_data[i]);
	}

	return retval;
}

errno_t writer(const char *fo_path, data_output_t *data_out_s) {

}
