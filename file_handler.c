#include "file_handler.h"

static errno_t runcmd_shell(const char *format, ...);
static errno_t readline_into_buffer(const char *file, void *buff);

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


static errno_t readline_into_buffer(const char *file, void *buff) {
	FILE *fp;

	fp = fopen(file, "r");
	
}


errno_t parser(const char *fi_path, data_input_t *data_in_s) {
	errno_t retval = SUCCESS;

	if (!fi_path || !data_in_s) {
		return PARAM_INVALID;
	}

	if (access(fi_path, F_OK) != 0) {
		return FILE_NOT_FOUND;
	}

	// get parameters from input file and save to another file
	retval = runcmd_shell("sed -n '4,8p' %s | awk '{print $2}' > ./.params.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	// get x data from input file and save to another file
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $1}' > ./.x_data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	// get t data from input file and save to another file
	retval = runcmd_shell("sed -n '12,$p' %s | awk '{print $2}' > ./.t_data.tmp", fi_path);
	if (retval != SUCCESS) {
		return retval;
	}

	

	return retval;
}

errno_t writer(const char *fo_path, data_output_t *data_out_s) {

}
