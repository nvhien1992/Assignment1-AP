#include "file_handler.h"

#define DEBUG_EN 0
#include "dbg.h"

const char *prec_pattern = "%.5f";
const int res_width = 10;
const char *width_pattern = "%10s";

static errno_t get_params(const char *file, train_params_t *params);
static errno_t get_data_samples(const char *file, data_samples_t *data);

static errno_t get_params(const char *file, train_params_t *params) {
	FILE *fp;
	const char *pattern = "%d\n%lf\n%lf\n%lf\n%d";

	fp = fopen(file, "r");
	if (!fp) {
		DEBUG("get_params: open file .params.tmp failed\n");
		return OPEN_FILE_FAILED;
	}

	if (fscanf(fp, pattern, &params->num_iters, &params->learning_rate,
		    &params->start_point.a, &params->start_point.b,
		    &params->num_folds) == 0) {
		DEBUG("get_params: parse file failed\n");
		fclose(fp);
		return PARSE_FAILED;
	}
	fclose(fp);
	return SUCCESS;
}

static errno_t get_data_samples(const char *file, data_samples_t *data) {
	FILE *fp;
	int num_samples = 0;

	fp = fopen(file, "r");
	if (!fp) {
		DEBUG("get_data_samples: open file .data.tmp failed\n");
		return OPEN_FILE_FAILED;
	}

	if (fscanf(fp, "%d\n", &num_samples) == 0) {
		return DATA_FATAL;
	}
	data->num_samples = num_samples;

	// allocate mem for x_data and t_data
	data->x_data = (double*) malloc(sizeof(double)*num_samples);
	data->t_data = (double*) malloc(sizeof(double)*num_samples);
	if (!data->x_data || !data->t_data) {
		return OUT_OF_MEM;
	}

	num_samples = 0;
	while (fscanf(fp, "%lf\n", &data->x_data[num_samples]) != 0) {
		num_samples++;
	}
	if (num_samples != data->num_samples) {
		data->num_samples = -1;
		return DATA_FATAL;
	}

	num_samples = 0;
	while (fscanf(fp, "%lf\n", &data->t_data[num_samples]) != 0) {
		num_samples++;
		if (feof(fp)) {
			break;
		}
	}
	if (num_samples != data->num_samples) {
		data->num_samples = -1;
		return DATA_FATAL;
	}

	fclose(fp);
	return SUCCESS;
}

errno_t runcmd_shell(const char *format, ...) {
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

errno_t parser(const char *fi_path, data_input_t *din) {
	errno_t retval = SUCCESS;

	if (!fi_path || !din) {
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

	// get num of data samples
	retval = runcmd_shell("sed -n '12,$p' %s | wc -l > .data.tmp", fi_path);

	// get x_data from input file and save to .data.tmp
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $1}' >> .data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	// seperate x_data and t_data by '-'
	retval = runcmd_shell("echo '-' >> .data.tmp");
	if (retval != SUCCESS) {
		return retval;
	}

	// get t_data from input file and append to .data.tmp
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $2}' >> .data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	get_params(".params.tmp", &din->trn_params);
	DEBUG("learning_rate: %.3f\n", din->trn_params.learning_rate);
	get_data_samples(".data.tmp", &din->data_samples);
	int i = 0;
	for (i = 0; i < 10; i++) {
		DEBUG("%lf %lf\n", din->data_samples.x_data[i], din->data_samples.t_data[i]);
	}

	return runcmd_shell("rm -f .params.tmp .data.tmp");
}

errno_t writer(const char *fo_path, const data_output_t *dout) {
	if (!fo_path || !dout) {
		return PARAM_INVALID;
	}

	FILE *fp;

	fp = fopen(fo_path, "w");
	if (!fp) {
		return OPEN_FILE_FAILED;
	}

	int i, j;
	char str[res_width+1];
	for (i = 0; i < dout->num_outputs; i++) {
		sprintf(str, prec_pattern, dout->lrning_oput[i].factors.a);
		fprintf(fp, width_pattern, str);
		sprintf(str, prec_pattern, dout->lrning_oput[i].factors.b);
		fprintf(fp, width_pattern, str);
		sprintf(str, prec_pattern, dout->lrning_oput[i].fcast_err_mean);
		fprintf(fp, width_pattern, str);
		
		for (j = 0; j < dout->lrning_oput[i].num_bin-1; j++) {
			sprintf(str, prec_pattern, dout->lrning_oput[i].histogram[j]);
			fprintf(fp, width_pattern, str);
		}
		sprintf(str, prec_pattern, dout->lrning_oput[i].histogram[j]);
		fprintf(fp, width_pattern, str);
		fputs("\n", fp);
	}
	fclose(fp);
	return SUCCESS;
}
