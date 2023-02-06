/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021-2022 NXP
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/queue.h>
#include <getopt.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ftw.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_malloc.h>
#include <rte_string_fns.h>

#include <cmdline_rdline.h>
#include <cmdline_parse.h>
#include <cmdline_socket.h>
#include <cmdline_parse_string.h>
#include <cmdline_parse_num.h>
#include <cmdline.h>

#include "nmm_lib.h"

#define APP_VERSION	"1.1"

static struct cmdline *cl;
static int scan_in_progress = 0;
FILE *band_scan_file;
uint16_t band_scan_crt_earfcn;
pthread_cond_t band_scan_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t band_scan_lock = PTHREAD_MUTEX_INITIALIZER;

static void print_version(void)
{
	printf("nmm-test: version %s (Built %s %s)\n", APP_VERSION, __DATE__, __TIME__);
}

static void hexdump(char *buf, uint32_t len, bool show_hdr)
{
	uint32_t i;

	for (i = 0; i < len; i++) {
		if (i % 16 == 0 && show_hdr)
			printf("\n%08x: ", i);
		printf("%02x ", buf[i]);
	}
	printf("\n");
}

static const char *fapi_type_to_text(uint32_t type)
{
	switch(type) {
	case CELL_SEARCH_REQUEST:
		return "Cell search request";
	case CELL_SEARCH_RESPONSE:
		return "Cell search response";
	case CELL_SEARCH_INDICATION:
		return "Cell search indication";
	case NMM_STOP_REQUEST:
		return "NMM stop request";
	case NMM_STOP_RESPONSE:
		return "NMM stop response";
	case CELL_FOLLOW_REQUEST:
		return "Cell follow request";
	case CELL_FOLLOW_RESPONSE:
		return "Cell follow response";
	case CELL_FOLLOW_INDICATION:
		return "Cell follow indication";
	case BROADCAST_DETECT_REQUEST:
		return "Broadcast detect request";
	case BROADCAST_DETECT_RESPONSE:
		return "Broadcast detect response";
	case BROADCAST_DETECT_INDICATION:
		return "Broadcast detect indication";
	case SYSTEM_INFORMATION_SCHEDULE_REQUEST:
		return "System information schedule request";
	case SYSTEM_INFORMATION_SCHEDULE_RESPONSE:
		return "System information schedule response";
	case SYSTEM_INFORMATION_SCHEDULE_INDICATION:
		return "System information schedule indication";
	case SYSTEM_INFORMATION_REQUEST:
		return "System information request";
	case SYSTEM_INFORMATION_RESPONSE:
		return "System information response";
	case SYSTEM_INFORMATION_INDICATION:
		return "System information indication";
	case SSB_SCAN_RESPONSE:
	    return "SSB scan response";
    case CELL_FOLLOW_MIB_RESPONSE:
	    return "cell follow mib response";
	default:
		return "N/A";
	}
}

static const char *dbg_type_to_text(uint32_t type)
{
	switch(type) {
	case NMM_DBG_RX_CAPTURE_REQUEST:
		return "Rx capture request";
	case NMM_DBG_RX_CAPTURE_RESULT:
		return "Rx capture result";
	case NMM_DBG_TEST_MODE_ENABLE_REQUEST:
		return "Test mode enable request";
	case NMM_DBG_TEST_MODE_ENABLE_RESULT:
		return "Test mode enable result";
	case NMM_DBG_TEST_MODE_DISABLE_REQUEST:
		return "Test mode disable request";
	case NMM_DBG_TEST_MODE_DISABLE_RESULT:
		return "Test mode disable result";
	case NMM_DBG_DECIMATOR_CAPTURE_REQUEST:
		return "Low rate capture request";
	case NMM_DBG_DECIMATOR_CAPTURE_RESULT:
		return "Low rate capture result";
	case NMM_DBG_CMD_FILL_MEM_CUSTOM_REQUEST:
		return "cmd fill mem custom request";
	case NMM_DBG_CMD_FILL_MEM_CUSTOM_RESULT:
		return "cmd fill mem custom result";
	case NMM_DBG_FORCE_RESET_RESULT:
		return "Reset result";
	default:
		return "N/A";
	}
}

static const char *fapi_error_to_text(uint32_t error_code)
{
	switch(error_code) {
	case MSG_OK:
		return "OK";
	case MSG_INVALID_STATE:
		return "Invalid state";
	case MSG_INVALID_CONFIG:
		return "Invalid config";
	case MSG_RAT_NOT_SUPPORTED:
		return "RAT not supported";
	case MSG_NMM_STOP_OK:
		return "Stop ok";
	case MSG_NMM_STOP_IGNORED:
		return "Stop ignored";
	case MSG_NMM_STOP_INVALID_STATE:
		return "Stop - Invalid state";
	case MSG_PROCEDURE_COMPLETE:
		return "Procedure complete";
	case MSG_PROCEDURE_STOPPED:
		return "Procedure stopped";
	case MSG_PARTIAL_RESULTS:
		return "Partial results";
	case MSG_TIMEOUT:
		return "Timeout";
	case MSG_CELL_NOT_FOUND:
		return "Cell not found";
	case MSG_LOST_SYNC:
		return "Lost sync";
	default:
		return "N/A";
	}
}

static int request_cell_search(lte_cell_search_request_t *cs_params)
{
	char request[64] = {0};
	fapi_request_t *fapi_req;
	lte_cell_search_request_t *cs_req;
	int err;

	fapi_req = (fapi_request_t *)request;
	fapi_req->message_type = CELL_SEARCH_REQUEST;
	fapi_req->rat_type = LTE_RAT;

	cs_req = (lte_cell_search_request_t *)&fapi_req->message_body;
	memcpy(cs_req, cs_params, cs_params->length);
	cs_req->tag = LTE_CELL_SEARCH_REQUEST_TAG;

	err = nmm_fapi_send_request(fapi_req);
	if (err)
		printf("nmm_send_fapi_request failed (err = %d)\n", err);

	return err;
}

static int enable_test_mode(char *filename)
{
	char request[64] = {0};
	nmm_dbg_request_t *dbg_req;
	nmm_test_mode_enable_request_t *tme_req;

	dbg_req = (nmm_dbg_request_t *)request;
	dbg_req->type = NMM_DBG_TEST_MODE_ENABLE_REQUEST;

	tme_req = (nmm_test_mode_enable_request_t *)&dbg_req->body;
	rte_strscpy(tme_req->filename, filename, NMM_FILENAME_SIZE);

	return nmm_dbg_send_request(dbg_req);
}

static int disable_test_mode(void)
{
	nmm_dbg_request_t dbg_req;

	dbg_req.type = NMM_DBG_TEST_MODE_DISABLE_REQUEST;

	return nmm_dbg_send_request(&dbg_req);
}

/* Command line interface */


#define QUIT_HELP_TXT		\
	"exit application\n"
#define HELP_HELP_TXT		\
	"print help text\n"
#define	ENABLE_TM_HELP_TXT	\
	"enables test mode and loads test vector\n"								\
	"Usage: enable_test_mode file <test_vector_file>\n"							\
	"	file: Name of file containing the test vector to be used\n"
#define DISABLE_TM_HELP_TXT	\
	"disables test mode\n"
#define CELL_SEARCH_HELP_TXT	\
	"perform cell search\n"											\
	"Usage: cell_search earfcn <earfcn>\n"									\
	"	earfcn: Specify the EARFCN for which cell search is requested\n"
#define CELL_FOLLOW_HELP_TXT	\
	"perform cell follow\n"											\
	"Usage: cell_follow gscn <gscn> \n"			\
	"   gscn: Specify the GSCN for which cell follow is requested\n"				
#define BCAST_DETECT_HELP_TXT	\
	"perform broadcast detect\n"										\
	"Usage: broadcast_detect earfcn <earfcn> pci <cell_id>\n"						\
	"	earfcn: Specify the EARFCN for which broadcast detect measurement is requested\n"		\
	"	pci: Specify the ID of the cell\n"
#define SYS_INFO_SCHED_HELP_TXT	\
	"perform system infrmation schedule\n"										\
	"Usage: system_info_sched earfcn <earfcn> pci <cell_id>\n"						\
	"	earfcn: Specify the EARFCN where system scheduling is requested\n"		\
	"	pci: Specify the ID of the cell\n"
#define SYS_INFO_HELP_TXT	\
	"perform system infrmation\n"										\
	"Usage: system_info earfcn <earfcn> pci <cell_id> window <si_win_len> sib <sib_id_list> period <si_period_list> index <si_index_list>]\n"						\
	"	earfcn: Specify the EARFCN where system scheduling is requested\n"		\
	"	pci: Specify the ID of the cell\n"						\
	"	window: SI window length in ms (valid values: 1,2,5,10,15,20,40\n"		\
	"	sib_id: list of requested SIB (valid values: 2-32; format x,y,z or {x,y,z}\n"	\
	"	period: list of SI periodicity in msec for requested SIBs"			\
	"		(valid values: 80, 160, 320, 640, 1280, 2560, 5120)\n"			\
	"	index: list of SI indexes for requested SIBs\n"					\
	"If multiple SIBs are specified in the same command, list size for sib, period and index must match.\n"
#define RX_CAPTURE_HELP_TXT	\
	"dump Rx data from RF to file\n"									\
	"Usage: rx_capture earfcn <earfcn> file <out_file> size <capture_size>\n"				\
	"	earfcn: EARFCN for which data capture is requested\n"						\
	"	file: Name of file in which to dump Rx data\n"							\
	"	size: Size in bytes of the Rx capture\n"
#define FILL_MEM_CUSTOM_HELP_TXT	\
	" sending custom data from Host to DDR Location\n"									\
	"Usage: cmd_fill_mem_custom file <out_file>\n"				\
	"	file: Name of file you want to dump to DDR\n"				
#define DEC_CAPTURE_HELP_TXT	\
	"dump decimated data @1.92MHz to file\n"									\
	"Usage: low_rate_capture earfcn <earfcn> file <out_file> size <capture_size>\n"				\
	"	earfcn: EARFCN for which data capture is requested\n"						\
	"	file: Name of file in which to dump low rate data\n"							\
	"	size: Size in bytes of the low rate capture\n"
#define RX_BAND_SCAN_TXT	\
	"scan specified band and store information on the cells found in a csv file\n"				\
	"Usage: band_scan band <band_id> file <csv_file>\n"							\
	"	band_id: 3 or 13\n"										\
	"	file: Name of csv file in which to store results\n"
#define RX_RANGE_SCAN_TXT	\
	"scan specified EARFCN range and store information on the cells found in a csv file\n"			\
	"Usage: range_scan start <earfcn_start> end <earfcn_end> file <csv_file>\n"				\
	"	earfcn_start..earfcn_end: range of EARFCNs to search\n"						\
	"	file: Name of csv file in which to store results\n"
#define CELL_SCAN_HELP_TXT   \
    "CELL_SCAN is a dummy command without any inputs\n"       \
	"Sending a command from host to controller and checking whether it is reaching or not\n"
# define BAND_SCAN_N_77_HELP_TXT  \
     "N_77 is frequency band designated for 5G standard and they are defined with prefix n\n"            \
	 "Usage:Just type command on host console and see output at controller or freertos console\n"
#define CELL_FOLLOW_MIB_HELP_TXT  \
    "perform cell follow mib\n"											\
	"Usage: cell_follow gscn <gscn> \n"			\
	"   gscn: Specify the GSCN for which cell follow mib is requested\n"		
#define CELL_FOLLOW_STOP_HELP_TXT  \
    "stops cell_follow command\n"       \
	"Usage:Just type command on host console and see output at controller or freertos console\n"
#define STOP_HELP_TXT		\
	"send NMM STOP request\n"
#define RESET_HELP_TXT		\
	"Force a reset of the NLM card\n"
#define SHOW_LOG_HELP_TXT		\
	"Dump NLM card log. Log wraps around after 4KB and is reset by each read.\n"

#define CELL_FOLLOW_SIB_1_HELP_TXT  \
    "perform cell follow sib 1\n"											\
	"Usage: cell_follow_sib_1 gscn <gscn> \n"			\
	"   gscn: Specify the GSCN for which cell follow sib 1 is requested\n"	
/* Quit */
struct cmd_quit_result {
	cmdline_fixed_string_t quit;
};

cmdline_parse_token_string_t cmd_quit_quit =
	TOKEN_STRING_INITIALIZER(struct cmd_quit_result, quit, "quit");

static void cmd_quit_parsed(__attribute__((unused)) void *parsed_result,
			    struct cmdline *cl,
			    __attribute__((unused)) void *data)
{
	cmdline_quit(cl);
}

cmdline_parse_inst_t cmd_quit = {
	.f = cmd_quit_parsed,
	.data = NULL,
	.help_str = QUIT_HELP_TXT,
	.tokens = {
		(void *)&cmd_quit_quit,
		NULL,
	},
};

/* Help */
struct cmd_help_result {
	cmdline_fixed_string_t help;
};

cmdline_parse_token_string_t cmd_help_help =
	TOKEN_STRING_INITIALIZER(struct cmd_help_result, help, "help");

static void cmd_help_parsed(__attribute__((unused)) void *parsed_result,
			    struct cmdline *cl,
			    __attribute__((unused)) void *data)
{
	cmdline_printf(cl, "\n"
	       "Available commands:\n"
	       "cell_search: " CELL_SEARCH_HELP_TXT
	       "cell_follow: " CELL_FOLLOW_HELP_TXT
	       "broadcast_detect: " BCAST_DETECT_HELP_TXT
	       "system_info_sched:" SYS_INFO_SCHED_HELP_TXT
	       "system_info:" SYS_INFO_HELP_TXT
	       "rx_capture: " RX_CAPTURE_HELP_TXT
		   "fill_mem_custom: " FILL_MEM_CUSTOM_HELP_TXT
	       "low_rate_capture: " DEC_CAPTURE_HELP_TXT
	       "enable_test_mode: " ENABLE_TM_HELP_TXT
	       "disable_test_mode: " DISABLE_TM_HELP_TXT
	       "stop: " STOP_HELP_TXT
	       "reset_modem: " RESET_HELP_TXT
	       "show_modem_log: " SHOW_LOG_HELP_TXT
		   "cell_scan: " CELL_SCAN_HELP_TXT
		   "band_scan_n_77: " BAND_SCAN_N_77_HELP_TXT
		   "cell_follow_mib: " CELL_FOLLOW_MIB_HELP_TXT
		   "cell_follow_stop:" CELL_FOLLOW_STOP_HELP_TXT
		   "cell_follow_sib_1:" CELL_FOLLOW_SIB_1_HELP_TXT
	       "help: " HELP_HELP_TXT
	       "quit: " QUIT_HELP_TXT
          "\n");
}

cmdline_parse_inst_t cmd_help = {
	.f = cmd_help_parsed,
	.data = NULL,
	.help_str = HELP_HELP_TXT,
	.tokens = {
		(void *)&cmd_help_help,
		NULL,
	},
};

/* Stop */
struct cmd_stop_result {
	cmdline_fixed_string_t stop;
};

cmdline_parse_token_string_t cmd_stop_stop =
	TOKEN_STRING_INITIALIZER(struct cmd_stop_result, stop, "stop");

static void cmd_stop_parsed(__attribute__((unused)) void *parsed_result,
			    struct cmdline *cl,
			    __attribute__((unused)) void *data)
{
	char request[64] = {0};
	fapi_request_t *fapi_req;
	int ret;

	fapi_req = (fapi_request_t *)request;
	fapi_req->message_type = NMM_STOP_REQUEST;

	ret = nmm_fapi_send_request(fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error sending NMM_STOP request\n");
}

cmdline_parse_inst_t cmd_stop = {
	.f = cmd_stop_parsed,
	.data = NULL,
	.help_str = STOP_HELP_TXT,
	.tokens = {
		(void *)&cmd_stop_stop,
		NULL,
	},
};

/* Reset */
struct cmd_reset_result {
	cmdline_fixed_string_t reset;
};

cmdline_parse_token_string_t cmd_reset_reset =
	TOKEN_STRING_INITIALIZER(struct cmd_reset_result, reset, "reset_modem");

static void cmd_reset_parsed(__attribute__((unused)) void *parsed_result,
			     struct cmdline *cl,
			     __attribute__((unused)) void *data)
{
	nmm_dbg_request_t dbg_req;
	int ret;

        dbg_req.type = NMM_DBG_FORCE_RESET_REQUEST;
	ret = nmm_dbg_send_request(&dbg_req);
	if (ret < 0)
		cmdline_printf(cl, "Error resetting NLM card\n");
}

cmdline_parse_inst_t cmd_reset = {
	.f = cmd_reset_parsed,
	.data = NULL,
	.help_str = RESET_HELP_TXT,
	.tokens = {
		(void *)&cmd_reset_reset,
		NULL,
	},
};

/* Show NLM card log */
struct cmd_show_log_result {
	cmdline_fixed_string_t show_log;
};

cmdline_parse_token_string_t cmd_show_show =
	TOKEN_STRING_INITIALIZER(struct cmd_show_log_result, show_log, "show_modem_log");

const char search_root[] = "/sys/devices/platform/soc/";
const char parent_dir[] = "la9310sysfs";
const char log_file[] = "target_log";
#define LOG_FILE_SIZE		4096

static int fn(const char *fpath,
	      __attribute__((unused)) const struct stat *sb,
	      __attribute__((unused)) int flag, struct FTW *ftwbuf)
{
	char buffer[LOG_FILE_SIZE] = {0};
	int fd, ret;

	if (strncmp(fpath + ftwbuf->base, log_file, strlen(log_file)))
		return 0;

	/* Make sure we didn't hit another entry with the same name */
	if (!strstr(fpath, parent_dir))
		return 0;

	/* Found our sysfs entry */
	fd = open(fpath, O_RDONLY);
	if (fd < 0) {
		cmdline_printf(cl, "Failed to open %s\n", log_file);
		goto out;
	}
	ret = read(fd, buffer, LOG_FILE_SIZE);
	if (ret < 0) {
		cmdline_printf(cl, "Failed to read from %s\n", log_file);
		goto out_close;
	}

	cmdline_printf(cl, "%.*s\n", ret, buffer);

out_close:
	close(fd);
out:
	return 1;
}

static void cmd_show_log_parsed(__attribute__((unused)) void *parsed_result,
				struct cmdline *cl,
				__attribute__((unused)) void *data)
{
	int ret;

	/* Find sysfs entry */
	ret = nftw(search_root, fn, 32, FTW_PHYS);
	if (ret < 0) {
		cmdline_printf(cl, "Unable to find sysfs entry %s\n", log_file);
		return;
	}
}

cmdline_parse_inst_t cmd_show_log = {
	.f = cmd_show_log_parsed,
	.data = NULL,
	.help_str = SHOW_LOG_HELP_TXT,
	.tokens = {
		(void *)&cmd_show_show,
		NULL,
	},
};

/* Enable test mode */
struct cmd_enable_tm_result {
	cmdline_fixed_string_t enable;
	cmdline_fixed_string_t file;
	cmdline_fixed_string_t test_vector;
};

cmdline_parse_token_string_t cmd_enable_tm =
	TOKEN_STRING_INITIALIZER(struct cmd_enable_tm_result, enable,
				 "enable_test_mode");
cmdline_parse_token_string_t cmd_enable_file =
	TOKEN_STRING_INITIALIZER(struct cmd_enable_tm_result, file, "file");
cmdline_parse_token_string_t cmd_test_vector =
	TOKEN_STRING_INITIALIZER(struct cmd_enable_tm_result, test_vector, NULL);

static void cmd_enable_tm_parsed(void *parsed_result,
				 struct cmdline *cl,
				 __attribute__((unused)) void *data)
{
	struct cmd_enable_tm_result *res = parsed_result;
	int ret;

	ret = enable_test_mode(res->test_vector);
	if (ret < 0)
		cmdline_printf(cl, "Error enabling test mode\n");
}

cmdline_parse_inst_t cmd_enable_test_mode = {
	.f = cmd_enable_tm_parsed,
	.data = NULL,
	.help_str = ENABLE_TM_HELP_TXT,
	.tokens = {
		(void *)&cmd_enable_tm,
		(void *)&cmd_enable_file,
		(void *)&cmd_test_vector,
		NULL,
	},
};

/* Disable test mode */
struct cmd_disable_tm_result {
	cmdline_fixed_string_t disable;
};

cmdline_parse_token_string_t cmd_disable_tm =
	TOKEN_STRING_INITIALIZER(struct cmd_disable_tm_result, disable,
				 "disable_test_mode");

static void cmd_disable_tm_parsed(__attribute__((unused)) void *parsed_result,
				  struct cmdline *cl,
				  __attribute__((unused)) void *data)
{
	int ret;

	ret = disable_test_mode();
	if (ret < 0)
		cmdline_printf(cl, "Error disabling test mode\n");
}

cmdline_parse_inst_t cmd_disable_test_mode = {
	.f = cmd_disable_tm_parsed,
	.data = NULL,
	.help_str = DISABLE_TM_HELP_TXT,
	.tokens = {
		(void *)&cmd_disable_tm,
		NULL,
	},
};

/* BAND_SCAN_N77*/
struct cmd_band_scan_n_77_result {
	cmdline_fixed_string_t n77bs;
};
cmdline_parse_token_string_t cmd_n77bs_n77bs =
	TOKEN_STRING_INITIALIZER(struct cmd_band_scan_n_77_result, n77bs,
				 "band_scan_n_77");

static void cmd_band_scan_n_77_parsed(__attribute__((unused)) void *parsed_result,
                  struct cmdline *cl,
				   __attribute__((unused)) void *data)																																	
{
	int ret;
	char request[64] = {0};
	fapi_request_ssb_t *fapi_req;

    
	fapi_req = (fapi_request_ssb_t *)request;
	fapi_req->message_type = BAND_SCAN_N77_REQUEST;
	
	ret = nmm_fapi_send_request((fapi_request_t*)fapi_req);
	if (ret)
		cmdline_printf(cl, "Error in band scan  n77 request\n");
}


cmdline_parse_inst_t cmd_band_scan_n_77 = {
	.f = cmd_band_scan_n_77_parsed,
	.data = NULL,
	.help_str = BAND_SCAN_N_77_HELP_TXT,
	.tokens = {
		(void *)&cmd_n77bs_n77bs,
		NULL,
	},
};


/* SSB_SCAN */
struct cmd_ssb_scan_result {
	cmdline_fixed_string_t ss;
};
cmdline_parse_token_string_t cmd_ss_ss =
	TOKEN_STRING_INITIALIZER(struct cmd_ssb_scan_result, ss,
				 "cell_scan");

static void cmd_ssb_scan_parsed(__attribute__((unused)) void *parsed_result,
                  struct cmdline *cl,
				   __attribute__((unused)) void *data)																																	
{
	int ret;
	char request[64] = {0};
	fapi_request_ssb_t *fapi_req;

    
	fapi_req = (fapi_request_ssb_t *)request;
	fapi_req->message_type = SSB_SCAN_REQUEST;
	
	ret = nmm_fapi_send_request((fapi_request_t*)fapi_req);
	if (ret)
		cmdline_printf(cl, "Error in ssb scan request\n");
}

cmdline_parse_inst_t cmd_ssb_scan = {
	.f = cmd_ssb_scan_parsed,
	.data = NULL,
	.help_str = CELL_SCAN_HELP_TXT,
	.tokens = {
		(void *)&cmd_ss_ss,
		NULL,
	},
};


/* Cell search */
struct cmd_cell_search_result {
	cmdline_fixed_string_t cs;
	uint16_t earfcn;
};

cmdline_parse_token_string_t cmd_cs_cs =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_search_result, cs,
				 "cell_search");
cmdline_parse_token_string_t cmd_cs_earfcn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_search_result, cs, "earfcn");
cmdline_parse_token_num_t cmd_cs_earfcn =
	TOKEN_NUM_INITIALIZER(struct cmd_cell_search_result, earfcn, UINT16);

static void cmd_cell_search_parsed(void *parsed_result,
				   struct cmdline *cl,
				   __attribute__((unused)) void *data)
{
	struct cmd_cell_search_result *res = parsed_result;
	lte_cell_search_request_t cs_req = {
		.length = 13,
		.earfcn = 0,
		.bandwidth = 0,
		.exhaustive_search = 1,
		.timeout = 0,
		.num_pci = 0,
	};
	int ret;

	cs_req.earfcn = res->earfcn;
	ret = request_cell_search(&cs_req);
	if (ret < 0)
		cmdline_printf(cl, "Error disabling test mode\n");
}

cmdline_parse_inst_t cmd_cell_search = {
	.f = cmd_cell_search_parsed,
	.data = NULL,
	.help_str = CELL_SEARCH_HELP_TXT,
	.tokens = {
		(void *)&cmd_cs_cs,
		(void *)&cmd_cs_earfcn_str,
		(void *)&cmd_cs_earfcn,
		NULL,
	},
};

/*cell_follow_stop*/
struct cmd_cell_follow_stop_result {
	cmdline_fixed_string_t cfs;
};
cmdline_parse_token_string_t cmd_cfs_cfs =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_stop_result, cfs,
				 "cell_follow_stop");

static void cmd_cell_follow_stop_parsed(__attribute__((unused)) void *parsed_result,
                  struct cmdline *cl,
				   __attribute__((unused)) void *data)																																	
{
	int ret;
	
	char request[64] = {0};
	fapi_request_ssb_t *fapi_req;
    
	fapi_req = (fapi_request_ssb_t *)request;
	fapi_req->message_type = CELL_FOLLOW_STOP_REQUEST;
	
	ret = nmm_fapi_send_request((fapi_request_t*)fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error disabling test mode\n");
}

cmdline_parse_inst_t cmd_cell_follow_stop = {
	.f = cmd_cell_follow_stop_parsed,
	.data = NULL,
	.help_str = CELL_FOLLOW_STOP_HELP_TXT,
	.tokens = {
		(void *)&cmd_cfs_cfs,
		NULL,
	},
};
//CELL_FOLLOW_SIB_1
struct cmd_cell_follow_sib_1_result {
	cmdline_fixed_string_t cfs1;
	cmdline_fixed_string_t gscn_str;
	uint16_t gscn;
};

cmdline_parse_token_string_t cmd_cfs1 =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_sib_1_result, cfs1, "cell_follow_sib_1");
cmdline_parse_token_string_t cmd_cfs1_gscn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_sib_1_result, gscn_str, "gscn");
cmdline_parse_token_num_t cmd_cfs1_gscn =
	TOKEN_NUM_INITIALIZER(struct cmd_cell_follow_sib_1_result, gscn, UINT16);

static void cmd_cell_follow_sib_1_parsed(void *parsed_result,
				   struct cmdline *cl,
				   __attribute__((unused)) void *data)
{
	struct cmd_cell_follow_sib_1_result *res = parsed_result;
	char request[64] = {0};
	fapi_request_nr_t *fapi_req;
	nr_cell_follow_request_t *cfs1_req;
	int ret;

	fapi_req = (fapi_request_nr_t *)request;
	fapi_req->message_type = CELL_FOLLOW_SIB_1_REQUEST;
	cfs1_req = (nr_cell_follow_request_t *)&fapi_req->message_body;
	cfs1_req->gscn = res->gscn;
	ret = nmm_fapi_send_request((fapi_request_t*) fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in cell follow sib 1 request\n");
}

cmdline_parse_inst_t cmd_cell_follow_sib_1 = {
	.f = cmd_cell_follow_sib_1_parsed,
	.data = NULL,
	.help_str = CELL_FOLLOW_SIB_1_HELP_TXT,
	.tokens = {
		(void *)&cmd_cfs1,
		(void *)&cmd_cfs1_gscn_str,
		(void *)&cmd_cfs1_gscn,
		NULL,
	},
};
/*cell_follow_mib*/
struct cmd_cell_follow_mib_result {
	cmdline_fixed_string_t cfb;
	cmdline_fixed_string_t gscn_str;
	uint16_t gscn;
};

cmdline_parse_token_string_t cmd_cfb_cfb =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_mib_result, cfb, "cell_follow_mib");
cmdline_parse_token_string_t cmd_cfb_gscn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_mib_result, gscn_str, "gscn");
cmdline_parse_token_num_t cmd_cfb_gscn =
	TOKEN_NUM_INITIALIZER(struct cmd_cell_follow_mib_result, gscn, UINT16);

static void cmd_cell_follow_mib_parsed(void *parsed_result,
				   struct cmdline *cl,
				   __attribute__((unused)) void *data)
{
	struct cmd_cell_follow_mib_result *res = parsed_result;
	char request[64] = {0};
	fapi_request_nr_t *fapi_req;
	nr_cell_follow_request_t *cfb_req;
	int ret;

	fapi_req = (fapi_request_nr_t *)request;
	fapi_req->message_type = CELL_FOLLOW_MIB_REQUEST;
	cfb_req = (nr_cell_follow_request_t *)&fapi_req->message_body;
	cfb_req->gscn = res->gscn;
	ret = nmm_fapi_send_request((fapi_request_t*) fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in cell follow request\n");
}

cmdline_parse_inst_t cmd_cell_follow_mib = {
	.f = cmd_cell_follow_mib_parsed,
	.data = NULL,
	.help_str = CELL_FOLLOW_MIB_HELP_TXT,
	.tokens = {
		(void *)&cmd_cfb_cfb,
		(void *)&cmd_cfb_gscn_str,
		(void *)&cmd_cfb_gscn,
		NULL,
	},
};

/* Cell follow */
struct cmd_cell_follow_result {
	cmdline_fixed_string_t cf;
	cmdline_fixed_string_t gscn_str;
	uint16_t gscn;
};

cmdline_parse_token_string_t cmd_cf_cf =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_result, cf, "cell_follow");
cmdline_parse_token_string_t cmd_cf_gscn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_cell_follow_result, gscn_str, "gscn");
cmdline_parse_token_num_t cmd_cf_gscn =
	TOKEN_NUM_INITIALIZER(struct cmd_cell_follow_result, gscn, UINT16);

static void cmd_cell_follow_parsed(void *parsed_result,
				   struct cmdline *cl,
				   __attribute__((unused)) void *data)
{
	struct cmd_cell_follow_result *res = parsed_result;
	char request[64] = {0};
	fapi_request_nr_t *fapi_req;
	nr_cell_follow_request_t *cf_req;
	int ret;

	fapi_req = (fapi_request_nr_t *)request;
	fapi_req->message_type = CELL_FOLLOW_REQUEST;
	cf_req = (nr_cell_follow_request_t *)&fapi_req->message_body;
	cf_req->gscn = res->gscn;
	ret = nmm_fapi_send_request((fapi_request_t*)fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in cell follow request\n");
}

cmdline_parse_inst_t cmd_cell_follow = {
	.f = cmd_cell_follow_parsed,
	.data = NULL,
	.help_str = CELL_FOLLOW_HELP_TXT,
	.tokens = {
		(void *)&cmd_cf_cf,
		(void *)&cmd_cf_gscn_str,
		(void *)&cmd_cf_gscn,
		NULL,
	},
};

/* Broadcast detect */
struct cmd_bcast_detect_result {
	cmdline_fixed_string_t bd;
	cmdline_fixed_string_t earfcn_str;
	uint16_t earfcn;
	cmdline_fixed_string_t pci_str;
	uint16_t pci;
};

cmdline_parse_token_string_t cmd_bd_bd =
	TOKEN_STRING_INITIALIZER(struct cmd_bcast_detect_result, bd, "broadcast_detect");
cmdline_parse_token_string_t cmd_bd_earfcn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_bcast_detect_result, earfcn_str, "earfcn");
cmdline_parse_token_num_t cmd_bd_earfcn =
	TOKEN_NUM_INITIALIZER(struct cmd_bcast_detect_result, earfcn, UINT16);
cmdline_parse_token_string_t cmd_bd_pci_str =
	TOKEN_STRING_INITIALIZER(struct cmd_bcast_detect_result, pci_str, "pci");
cmdline_parse_token_num_t cmd_bd_pci =
	TOKEN_NUM_INITIALIZER(struct cmd_bcast_detect_result, pci, UINT16);

static void cmd_bcast_detect_parsed(void *parsed_result,
				    struct cmdline *cl,
				    __attribute__((unused)) void *data)
{
	struct cmd_bcast_detect_result *res = parsed_result;
	char request[64] = {0};
	fapi_request_t *fapi_req;
	lte_bcast_detect_request_t *bd_req;
	int ret;

	fapi_req = (fapi_request_t *)request;
	fapi_req->message_type = BROADCAST_DETECT_REQUEST;
	fapi_req->rat_type = LTE_RAT;

	bd_req = (lte_bcast_detect_request_t *)&fapi_req->message_body;
	bd_req->tag = LTE_BROADCAST_DETECT_REQUEST_TAG;
	bd_req->length = sizeof(lte_bcast_detect_request_t);
	bd_req->earfcn = res->earfcn;
	bd_req->pci = res->pci;

	ret = nmm_fapi_send_request(fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in broadcast detect request\n");
}


cmdline_parse_inst_t cmd_bcast_detect = {
	.f = cmd_bcast_detect_parsed,
	.data = NULL,
	.help_str = BCAST_DETECT_HELP_TXT,
	.tokens = {
		(void *)&cmd_bd_bd,
		(void *)&cmd_bd_earfcn_str,
		(void *)&cmd_bd_earfcn,
		(void *)&cmd_bd_pci_str,
		(void *)&cmd_bd_pci,
		NULL,
	},
};

/* System Information Schedule */
struct cmd_sys_info_sched_result {
	cmdline_fixed_string_t sis;
	cmdline_fixed_string_t earfcn_str;
	uint16_t earfcn;
	cmdline_fixed_string_t pci_str;
	uint16_t pci;
};

cmdline_parse_token_string_t cmd_sis_sis =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_sched_result, sis, "system_info_sched");
cmdline_parse_token_string_t cmd_sis_earfcn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_sched_result, earfcn_str, "earfcn");
cmdline_parse_token_num_t cmd_sis_earfcn =
	TOKEN_NUM_INITIALIZER(struct cmd_sys_info_sched_result, earfcn, UINT16);
cmdline_parse_token_string_t cmd_sis_pci_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_sched_result, pci_str, "pci");
cmdline_parse_token_num_t cmd_sis_pci =
	TOKEN_NUM_INITIALIZER(struct cmd_sys_info_sched_result, pci, UINT16);

static void cmd_sys_info_sched_parsed(void *parsed_result,
				    struct cmdline *cl,
				    __attribute__((unused)) void *data)
{
	struct cmd_sys_info_sched_result *res = parsed_result;
	char request[64] = {0};
	fapi_request_t *fapi_req;
	lte_sys_info_sched_request_t *sis_req;
	int ret;

	fapi_req = (fapi_request_t *)request;
	fapi_req->message_type = SYSTEM_INFORMATION_SCHEDULE_REQUEST;
	fapi_req->rat_type = LTE_RAT;

	sis_req = (lte_sys_info_sched_request_t *)&fapi_req->message_body;
	sis_req->tag = LTE_SYSTEM_INFORMATION_SCHEDULE_REQUEST_TAG;
	sis_req->length = sizeof(lte_sys_info_sched_request_t);
	sis_req->earfcn = res->earfcn;
	sis_req->pci = res->pci;
	/* TODO: fill or fields, or assume auto-fill from MIB
		uint16_t dl_bw;
		uint8_t  phich_cfg;
		uint8_t  num_tx;
		uint8_t  retry_count;
		uint32_t timeout;
	*/

	ret = nmm_fapi_send_request(fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in broadcast detect request\n");
}


cmdline_parse_inst_t cmd_sys_info_sched = {
	.f = cmd_sys_info_sched_parsed,
	.data = NULL,
	.help_str = SYS_INFO_SCHED_HELP_TXT,
	.tokens = {
		(void *)&cmd_sis_sis,
		(void *)&cmd_sis_earfcn_str,
		(void *)&cmd_sis_earfcn,
		(void *)&cmd_sis_pci_str,
		(void *)&cmd_sis_pci,
		NULL,
	},
};

/* System Information */
struct cmd_sys_info_result {
	cmdline_fixed_string_t si;
	cmdline_fixed_string_t earfcn_str;
	uint16_t earfcn;
	cmdline_fixed_string_t pci_str;
	uint16_t pci;
	cmdline_fixed_string_t window_str;
	uint8_t window;
	cmdline_fixed_string_t sib_str;
	cmdline_fixed_string_t sib_list;
	cmdline_fixed_string_t period_str;
	cmdline_fixed_string_t period_list;
	cmdline_fixed_string_t index_str;
	cmdline_fixed_string_t index_list;
};

cmdline_parse_token_string_t cmd_si_si =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, si, "system_info");
cmdline_parse_token_string_t cmd_si_earfcn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, earfcn_str, "earfcn");
cmdline_parse_token_num_t cmd_si_earfcn =
	TOKEN_NUM_INITIALIZER(struct cmd_sys_info_result, earfcn, UINT16);
cmdline_parse_token_string_t cmd_si_pci_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, pci_str, "pci");
cmdline_parse_token_num_t cmd_si_pci =
	TOKEN_NUM_INITIALIZER(struct cmd_sys_info_result, pci, UINT16);
cmdline_parse_token_string_t cmd_si_window_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, window_str, "window");
cmdline_parse_token_num_t cmd_si_window =
	TOKEN_NUM_INITIALIZER(struct cmd_sys_info_result, window, UINT8);
cmdline_parse_token_string_t cmd_si_sib_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, sib_str, "sib");
cmdline_parse_token_string_t cmd_si_sib_list =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, sib_list, NULL);
cmdline_parse_token_string_t cmd_si_period_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, period_str, "period");
cmdline_parse_token_string_t cmd_si_period_list =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, period_list, NULL);
cmdline_parse_token_string_t cmd_si_index_str =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, index_str, "index");
cmdline_parse_token_string_t cmd_si_index_list =
	TOKEN_STRING_INITIALIZER(struct cmd_sys_info_result, index_list, NULL);

static int parse_list(char *str, uint32_t *array)
{
	char *token;
	const char sep[] = ",{}";
	uint32_t val;
	int count = 0;

	token = strtok(str, sep);

	while (token != NULL) {
		val = strtoul(token, NULL, 10);
		if ((errno != 0 && val == 0) || (errno == ERANGE)) {
			printf("unable to parse list (invalid token %s)\n", token);
			return -1;
		}
		array[count++] = val;
		token = strtok(NULL, sep);
	}

	return count;
}

static const uint16_t si_periodicity_ms[] = {0, 80, 160, 320, 640, 1280, 2560, 5120};
static uint8_t convert_si_periodicity(uint16_t period_ms)
{
	int i;
	int array_size = sizeof(si_periodicity_ms) / sizeof(si_periodicity_ms[0]);

	for (i = 0; i < array_size; i++)
		if (period_ms == si_periodicity_ms[i])
			return i;
	return 0;
}

static void cmd_sys_info_parsed(void *parsed_result,
				struct cmdline *cl,
				__attribute__((unused)) void *data)
{
	struct cmd_sys_info_result *res = parsed_result;
	char request[128] = {0};
	fapi_request_t *fapi_req;
	lte_sys_info_request_t *si_req;
	char *sib_list_str = res->sib_list;
	char *period_list_str = res->period_list;
	char *index_list_str = res->index_list;
	uint32_t sib[MAX_SI], period[MAX_SI], index[MAX_SI];
	uint8_t period_code;
	int i, ret, num_si;

	fapi_req = (fapi_request_t *)request;
	fapi_req->message_type = SYSTEM_INFORMATION_REQUEST;
	fapi_req->rat_type = LTE_RAT;

	si_req = (lte_sys_info_request_t *)&fapi_req->message_body;
	si_req->tag = LTE_SYSTEM_INFORMATION_REQUEST_TAG;
	si_req->length = sizeof(lte_sys_info_request_t);
	si_req->earfcn = res->earfcn;
	si_req->pci = res->pci;
	si_req->si_window_len = res->window;

	ret = parse_list(sib_list_str, sib);
	if (ret < 0) {
		cmdline_printf(cl, "Error parsing SIB list\n");
		return;
	}
	num_si = ret;

	ret = parse_list(period_list_str, period);
	if (ret < 0 || ret != num_si) {
		cmdline_printf(cl, "Error parsing SI periodicity list\n");
		return;
	}

	ret = parse_list(index_list_str, index);
	if (ret < 0 || ret != num_si) {
		cmdline_printf(cl, "Error parsing SI index list\n");
		return;
	}

	si_req->num_si_periodicity = 0;
	for (i = 0; i < num_si; i++) {
		if (sib[i] < 2 || sib[i] >= MAX_SI) {
			cmdline_printf(cl, "Wrong SIB requested (%d)\n", sib[i]);
			return;
		}

		/*
		 * We don't validate periodicity and index values here,
		 * just make sure we don't truncate them when writing to
		 * destination type (uint8_t)
		 */
		period_code = convert_si_periodicity(period[i]);
		if (period_code == 0) {
			cmdline_printf(cl, "Invalid SI periodicity value(%d)\n", period[i]);
			return;
		}
		if (index[i] > UCHAR_MAX) {
			cmdline_printf(cl, "SI index too large (%d)\n", index[i]);
			return;
		}
		si_req->si_periodicity[sib[i] - 2] = period_code;
		si_req->si_index[sib[i] - 2] = index[i];

		if (si_req->num_si_periodicity < sib[i] - 1)
			si_req->num_si_periodicity = sib[i] - 1;
	}

	ret = nmm_fapi_send_request(fapi_req);
	if (ret < 0)
		cmdline_printf(cl, "Error in system information request\n");
}


cmdline_parse_inst_t cmd_sys_info = {
	.f = cmd_sys_info_parsed,
	.data = NULL,
	.help_str = SYS_INFO_HELP_TXT,
	.tokens = {
		(void *)&cmd_si_si,
		(void *)&cmd_sis_earfcn_str,
		(void *)&cmd_sis_earfcn,
		(void *)&cmd_sis_pci_str,
		(void *)&cmd_sis_pci,
		(void *)&cmd_si_window_str,
		(void *)&cmd_si_window,
		(void *)&cmd_si_sib_str,
		(void *)&cmd_si_sib_list,
		(void *)&cmd_si_period_str,
		(void *)&cmd_si_period_list,
		(void *)&cmd_si_index_str,
		(void *)&cmd_si_index_list,
		NULL,
	},
};

/* Rx capture */
struct cmd_rx_capture_result {
	cmdline_fixed_string_t rx_capture;
	cmdline_fixed_string_t rx_earfcn_str;
	uint16_t rx_earfcn;
	cmdline_fixed_string_t rx_file_str;
	cmdline_fixed_string_t rx_out_file;
	cmdline_fixed_string_t rx_size_str;
	uint32_t rx_out_size;
};

cmdline_parse_token_string_t cmd_rx_capture_cmd =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_capture,
				 "rx_capture");
cmdline_parse_token_string_t cmd_rx_earfcn_str =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_earfcn_str, "earfcn");
cmdline_parse_token_num_t cmd_rx_capture_earfcn =
	TOKEN_NUM_INITIALIZER(struct cmd_rx_capture_result, rx_earfcn, UINT16);
cmdline_parse_token_string_t cmd_rx_file_str =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_file_str, "file");
cmdline_parse_token_string_t cmd_rx_out_file =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_out_file, NULL);
cmdline_parse_token_string_t cmd_rx_size_str =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_size_str, "size");
cmdline_parse_token_num_t cmd_rx_out_size =
	TOKEN_NUM_INITIALIZER(struct cmd_rx_capture_result, rx_out_size, UINT32);

static void cmd_rx_capture_parsed(void *parsed_result,
				  struct cmdline *cl,
				  __attribute__((unused)) void *data)
{
	struct cmd_rx_capture_result *res = parsed_result;
	char request[64] = {0};
	nmm_dbg_request_t *dbg_req;
	nmm_rx_capture_request_t *rxc_req;
	int ret;

	dbg_req = (nmm_dbg_request_t *)request;
	dbg_req->type = NMM_DBG_RX_CAPTURE_REQUEST;

	rxc_req = (nmm_rx_capture_request_t *)&dbg_req->body;
	rxc_req->earfcn = res->rx_earfcn;
	rxc_req->length = res->rx_out_size;
	rte_strscpy(rxc_req->filename, res->rx_out_file, NMM_FILENAME_SIZE);
	ret = nmm_dbg_send_request(dbg_req);
	if (ret < 0)
		cmdline_printf(cl, "Error while doing Rx capture\n");
}

cmdline_parse_inst_t cmd_rx_capture = {
	.f = cmd_rx_capture_parsed,
	.data = NULL,
	.help_str = RX_CAPTURE_HELP_TXT,
	.tokens = {
		(void *)&cmd_rx_capture_cmd,
		(void *)&cmd_rx_earfcn_str,
		(void *)&cmd_rx_capture_earfcn,
		(void *)&cmd_rx_file_str,
		(void *)&cmd_rx_out_file,
		(void *)&cmd_rx_size_str,
		(void *)&cmd_rx_out_size,
		NULL,
	},
};

/* Fill Mem Custom */

struct cmd_fill_mem_custom_result {
	cmdline_fixed_string_t fill_mem_custom;
	cmdline_fixed_string_t fill_mem_custom_file_str;
	cmdline_fixed_string_t fill_mem_custom_file;	
};

cmdline_parse_token_string_t cmd_fill_mem_custom_cmd =
	TOKEN_STRING_INITIALIZER(struct cmd_fill_mem_custom_result, fill_mem_custom,
				 "fill_mem_custom");
cmdline_parse_token_string_t cmd_fill_mem_custom_file_str =
	TOKEN_STRING_INITIALIZER(struct cmd_fill_mem_custom_result, fill_mem_custom_file_str, "file");
cmdline_parse_token_string_t cmd_fill_mem_custom_file =
	TOKEN_STRING_INITIALIZER(struct cmd_fill_mem_custom_result, fill_mem_custom_file, NULL);

static void cmd_fill_mem_custom_parsed(void *parsed_result,
				  struct cmdline *cl,
				  __attribute__((unused)) void *data)
{	
	struct cmd_fill_mem_custom_result *res = parsed_result;
	char request[64] = {0};
	nmm_dbg_request_t *dbg_req;
	nmm_fill_mem_custom_request_t *fill_mem_custom_req; //added new structure
	int ret;

	dbg_req = (nmm_dbg_request_t *)request;
	dbg_req->type = NMM_DBG_CMD_FILL_MEM_CUSTOM_REQUEST;  
	fill_mem_custom_req = (nmm_fill_mem_custom_request_t *)&dbg_req->body;
	
	rte_strscpy(fill_mem_custom_req->filename, res->fill_mem_custom_file, NMM_FILENAME_SIZE);

	ret = nmm_dbg_send_request(dbg_req);
	if (ret < 0)
		cmdline_printf(cl, "Error while doing cmd_fill_mem_custom\n");
}

cmdline_parse_inst_t cmd_fill_mem_custom = {
	.f = cmd_fill_mem_custom_parsed,
	.data = NULL,
	.help_str = FILL_MEM_CUSTOM_HELP_TXT,
	.tokens = {
		(void *)&cmd_fill_mem_custom_cmd,
		(void *)&cmd_fill_mem_custom_file_str,
		(void *)&cmd_fill_mem_custom_file,
		NULL,
	},
};

/* Low rate capturing */
cmdline_parse_token_string_t cmd_dec_capture_cmd =
	TOKEN_STRING_INITIALIZER(struct cmd_rx_capture_result, rx_capture,
				 "low_rate_capture");

static void cmd_dec_capture_parsed(void *parsed_result,
				  struct cmdline *cl,
				  __attribute__((unused)) void *data)
{
	struct cmd_rx_capture_result *res = parsed_result;
	char request[64] = {0};
	nmm_dbg_request_t *dbg_req;
	nmm_rx_capture_request_t *rxc_req;
	int ret;

	dbg_req = (nmm_dbg_request_t *)request;
	dbg_req->type = NMM_DBG_DECIMATOR_CAPTURE_REQUEST;

	rxc_req = (nmm_rx_capture_request_t *)&dbg_req->body;
	rxc_req->earfcn = res->rx_earfcn;
	rxc_req->length = res->rx_out_size;
	rte_strscpy(rxc_req->filename, res->rx_out_file, NMM_FILENAME_SIZE);
	ret = nmm_dbg_send_request(dbg_req);
	if (ret < 0)
		cmdline_printf(cl, "Error while doing Rx capture\n");
}

cmdline_parse_inst_t cmd_dec_capture = {
	.f = cmd_dec_capture_parsed,
	.data = NULL,
	.help_str = DEC_CAPTURE_HELP_TXT,
	.tokens = {
		(void *)&cmd_dec_capture_cmd,
		(void *)&cmd_rx_earfcn_str,
		(void *)&cmd_rx_capture_earfcn,
		(void *)&cmd_rx_file_str,
		(void *)&cmd_rx_out_file,
		(void *)&cmd_rx_size_str,
		(void *)&cmd_rx_out_size,
		NULL,
	},
};

/* Band scan */
struct cmd_band_scan_result {
	cmdline_fixed_string_t band_scan;
	cmdline_fixed_string_t band_str;
	uint8_t band;
	cmdline_fixed_string_t csv_file_str;
	cmdline_fixed_string_t csv_file;
};

cmdline_parse_token_string_t cmd_bs_cmd =
	TOKEN_STRING_INITIALIZER(struct cmd_band_scan_result, band_scan, "band_scan");
cmdline_parse_token_string_t cmd_bs_band_str =
	TOKEN_STRING_INITIALIZER(struct cmd_band_scan_result, band_str, "band");
cmdline_parse_token_num_t cmd_bs_band =
	TOKEN_NUM_INITIALIZER(struct cmd_band_scan_result, band, UINT8);
cmdline_parse_token_string_t cmd_bs_csv_file_str =
	TOKEN_STRING_INITIALIZER(struct cmd_band_scan_result, csv_file_str, "file");
cmdline_parse_token_string_t cmd_bs_csv_file =
	TOKEN_STRING_INITIALIZER(struct cmd_band_scan_result, csv_file, NULL);


struct band_info {
	uint8_t band_id;
	uint16_t earfcn_low;
	uint16_t earfcn_high;
};

#define MAX_BANDS		2
static struct band_info supported_bands[MAX_BANDS] =
{
        {
        .band_id = 3,
        .earfcn_low = 1200,
        .earfcn_high = 1949,
        },
        {
        .band_id = 13,
        .earfcn_low = 5180,
        .earfcn_high = 5279,
        },
};

static struct band_info *get_band_info(uint8_t band_id)
{
	int i;

	for (i = 0; i < MAX_BANDS; i++) {
		if (supported_bands[i].band_id == band_id)
			return &supported_bands[i];
	}
	return NULL;
}

static void scan_earfcn_range(struct cmdline *cl,
			      uint16_t earfcn_low, uint16_t earfcn_high,
			      char *csv_file)
{
	lte_cell_search_request_t cs_req = {
		.length = 13,
		.earfcn = 0,
		.bandwidth = 0,
		.exhaustive_search = 1,
		.timeout = 0,
		.num_pci = 0,
	};
	int i, ret;

	scan_in_progress = 1;
	band_scan_file = fopen(csv_file, "w+");
	if (!band_scan_file) {
		cmdline_printf(cl, "Error opening file %s\n", csv_file);
		return;
	}
	ret = fprintf(band_scan_file, "EARFCN,PCI,RSRP,RSRQ,FREQ_OFFSET\n");
	if (ret < 0) {
		cmdline_printf(cl, "Error writing to file %s\n", csv_file);
		return;
	}

	for (i = earfcn_low; i <= earfcn_high; i++)
	{
		pthread_mutex_lock(&band_scan_lock);

		/* Send cell search request for current earfcn */
		band_scan_crt_earfcn = i;
		cs_req.earfcn = band_scan_crt_earfcn;
		ret = request_cell_search(&cs_req);
		if (ret < 0) {
			cmdline_printf(cl, "Error in cell search request\n");
			pthread_mutex_unlock(&band_scan_lock);
			break;
		}

		/* Wait for indication */
		pthread_cond_wait(&band_scan_cond, &band_scan_lock);
		pthread_mutex_unlock(&band_scan_lock);
	}

	scan_in_progress = 0;
	fclose(band_scan_file);
	cmdline_printf(cl, "Scan complete\n");
}

static void cmd_band_scan_parsed(void *parsed_result,
				      struct cmdline *cl,
				      __attribute__((unused)) void *data)
{
	struct cmd_band_scan_result *res = parsed_result;
	struct band_info *binfo;
	char csv_file[NMM_FILENAME_SIZE];

	binfo = get_band_info(res->band);
	if (!binfo) {
		cmdline_printf(cl, "Band %d not supported\n", res->band);
		return;
	}

	rte_strscpy(csv_file, res->csv_file, NMM_FILENAME_SIZE);
	scan_earfcn_range(cl, binfo->earfcn_low, binfo->earfcn_high, csv_file);
}

cmdline_parse_inst_t cmd_band_scan = {
	.f = cmd_band_scan_parsed,
	.data = NULL,
	.help_str = RX_BAND_SCAN_TXT,
	.tokens = {
		(void *)&cmd_bs_cmd,
		(void *)&cmd_bs_band_str,
		(void *)&cmd_bs_band,
		(void *)&cmd_bs_csv_file_str,
		(void *)&cmd_bs_csv_file,
		NULL,
	},
};

struct cmd_range_scan_result {
	cmdline_fixed_string_t range_scan;
	cmdline_fixed_string_t start_str;
	uint16_t earfcn_start;
	cmdline_fixed_string_t end_str;
	uint16_t earfcn_end;
	cmdline_fixed_string_t csv_file_str;
	cmdline_fixed_string_t csv_file;
};

cmdline_parse_token_string_t cmd_rs_cmd =
	TOKEN_STRING_INITIALIZER(struct cmd_range_scan_result, range_scan, "range_scan");
cmdline_parse_token_string_t cmd_rs_start_str =
	TOKEN_STRING_INITIALIZER(struct cmd_range_scan_result, start_str, "start");
cmdline_parse_token_num_t cmd_rs_earfcn_start =
	TOKEN_NUM_INITIALIZER(struct cmd_range_scan_result, earfcn_start, UINT16);
cmdline_parse_token_string_t cmd_rs_end_str =
	TOKEN_STRING_INITIALIZER(struct cmd_range_scan_result, end_str, "end");
cmdline_parse_token_num_t cmd_rs_earfcn_end =
	TOKEN_NUM_INITIALIZER(struct cmd_range_scan_result, earfcn_end, UINT16);
cmdline_parse_token_string_t cmd_rs_csv_file_str =
	TOKEN_STRING_INITIALIZER(struct cmd_range_scan_result, csv_file_str, "file");
cmdline_parse_token_string_t cmd_rs_csv_file =
	TOKEN_STRING_INITIALIZER(struct cmd_range_scan_result, csv_file, NULL);

static void cmd_range_scan_parsed(void *parsed_result,
				       struct cmdline *cl,
				       __attribute__((unused)) void *data)
{
	struct cmd_range_scan_result *res = parsed_result;
	char csv_file[NMM_FILENAME_SIZE];

	rte_strscpy(csv_file, res->csv_file, NMM_FILENAME_SIZE);
	scan_earfcn_range(cl, res->earfcn_start, res->earfcn_end, csv_file);
}

cmdline_parse_inst_t cmd_range_scan = {
	.f = cmd_range_scan_parsed,
	.data = NULL,
	.help_str = RX_RANGE_SCAN_TXT,
	.tokens = {
		(void *)&cmd_rs_cmd,
		(void *)&cmd_rs_start_str,
		(void *)&cmd_rs_earfcn_start,
		(void *)&cmd_rs_end_str,
		(void *)&cmd_rs_earfcn_end,
		(void *)&cmd_rs_csv_file_str,
		(void *)&cmd_rs_csv_file,
		NULL,
	},
};

/* Supported commands */
cmdline_parse_ctx_t nmm_test_ctx[] = {
	(cmdline_parse_inst_t *)&cmd_help,
	(cmdline_parse_inst_t *)&cmd_quit,
	(cmdline_parse_inst_t *)&cmd_stop,
	(cmdline_parse_inst_t *)&cmd_reset,
	(cmdline_parse_inst_t *)&cmd_enable_test_mode,
	(cmdline_parse_inst_t *)&cmd_disable_test_mode,
	(cmdline_parse_inst_t *)&cmd_cell_search,
	(cmdline_parse_inst_t *)&cmd_cell_follow,
	(cmdline_parse_inst_t *)&cmd_bcast_detect,
	(cmdline_parse_inst_t *)&cmd_sys_info_sched,
	(cmdline_parse_inst_t *)&cmd_sys_info,
	(cmdline_parse_inst_t *)&cmd_rx_capture,
	(cmdline_parse_inst_t *)&cmd_dec_capture,
	(cmdline_parse_inst_t *)&cmd_band_scan,
	(cmdline_parse_inst_t *)&cmd_range_scan,
	(cmdline_parse_inst_t *)&cmd_show_log,
	(cmdline_parse_inst_t *)&cmd_fill_mem_custom,
	(cmdline_parse_inst_t *)&cmd_ssb_scan,
	(cmdline_parse_inst_t *)&cmd_band_scan_n_77,
	(cmdline_parse_inst_t *)&cmd_cell_follow_mib,
	(cmdline_parse_inst_t *)&cmd_cell_follow_sib_1,
	(cmdline_parse_inst_t *)&cmd_cell_follow_stop,

	NULL,
};

static void fapi_response_cli_cb(fapi_response_t *rsp)
{
	if (scan_in_progress && rsp->error_code == MSG_OK)
		return;
	cmdline_printf(cl, "Received fapi response: type = %s, status code = %s\n",
		       fapi_type_to_text(rsp->message_type),
		       fapi_error_to_text(rsp->error_code));
}

static void process_cs_ind(fapi_indication_t *ind)
{
	lte_cell_search_indication_t *cs_ind;
	lte_cell_info_t *cell_info;
	int i;

	cs_ind = (lte_cell_search_indication_t *)&ind->message_body;

	/* If this is part of a band scan operation, write cell info in the
	 * band_scan file
	 */
	if (scan_in_progress) {
		pthread_mutex_lock(&band_scan_lock);

		for (i = 0; i < cs_ind->num_cells; i++) {
			cell_info = &cs_ind->cell_info[i];
			fprintf(band_scan_file, "%d,%d,%d,%d,%d\n",
				band_scan_crt_earfcn,
				cell_info->pci, -cell_info->rsrp,
				-cell_info->rsrq, cell_info->freq_offset);
		}

		pthread_cond_signal(&band_scan_cond);
		pthread_mutex_unlock(&band_scan_lock);
		return;
	}

	cmdline_printf(cl, "num cells = %d\n", cs_ind->num_cells);
	for (i = 0; i < cs_ind->num_cells; i++) {
		cell_info = &cs_ind->cell_info[i];
		cmdline_printf(cl, "cell #%d:\n"
			       "pci = %d\n"
			       "rsrp = %d dBm\n"
			       "rsrq = %d dB\n"
			       "freq offset = %d PPB\n",
			       i, cell_info->pci, -cell_info->rsrp,
			       -cell_info->rsrq, cell_info->freq_offset);
	}
}

static void process_bd_ind(fapi_indication_t *ind)
{
	lte_bcast_detect_indication_t *bd_ind;
	char phich_duration_name[2][10] = {"normal", "extended"};
	char phich_res_name[4][4] = {"1/6", "1/2", "1", "2"};
	char bw_MHz[6][4] = {"1.4", "3", "5", "10", "15", "20"};
	uint8_t *mib;
	uint8_t bw, duration, resource, sfn;
	uint16_t reserved;

	bd_ind = (lte_bcast_detect_indication_t *)&ind->message_body;

	cmdline_printf(cl, "MIB indication:\n");
	cmdline_printf(cl, "Num TX = %d\n", bd_ind->num_tx);
	cmdline_printf(cl, "MIB length = %d\n", bd_ind->mib_len);
	cmdline_printf(cl, "SFN offset = %d\n", bd_ind->sfn_offset);

	/* Print & decode MIB array */
	/* TODO: less hardcoding */
	mib = (uint8_t *)bd_ind->mib;
	printf("MIB array (raw): \n");
	hexdump((char*)mib, (uint32_t)bd_ind->mib_len, false);
	bw = (mib[0] & 0xE0) >> 5;
	duration = (mib[0] & 0x10) >> 4;
	resource = (mib[0] & 0x0C) >> 2;
	sfn = ((mib[0] & 0x03) << 6) | ((mib[1] & 0xFC) >> 2);
	reserved = (((uint16_t)(mib[1] & 0x3)) << 8) | mib[2];

	printf("BW = %sMHz (MIB[0:2] = %d)\n", bw_MHz[bw], bw);
	printf("PHICH duration = %s (MIB[3] = %d)\n", phich_duration_name[duration], duration);
	printf("PHICH resource = %s (MIB[4:5] = %d)\n", phich_res_name[resource], resource);
	printf("SFN = %d (MIB[6:13] = 0x%02x)\n", sfn, sfn);
	printf("Reserved = %d (MIB[14-23] = 0x%02x)\n", reserved, reserved);
}

static void process_si_ind(fapi_indication_t *ind)
{
	lte_sys_info_indication_t *si_ind;
	char *sib;

	si_ind = (lte_sys_info_indication_t *)&ind->message_body;

	sib = (char *)si_ind->sib;
	printf("SIB%d array (raw) of %d bytes: \n", si_ind->sib_type, si_ind->sib_len);
	hexdump(sib, (uint32_t)si_ind->sib_len, false);
}

static void fapi_indication_cli_cb(fapi_indication_t *ind)
{

	/*
	 * If band_scan, we don't want to flood the console.
	 * No prints unless an error occurs.
	 */
	if (!(scan_in_progress && ind->error_code == MSG_PROCEDURE_COMPLETE))
		cmdline_printf(cl, "Received fapi indication: type = %s err code = %s\n",
		       fapi_type_to_text(ind->message_type),
		       fapi_error_to_text(ind->error_code));

	if (ind->error_code != MSG_PROCEDURE_COMPLETE &&
	    ind->error_code != MSG_PARTIAL_RESULTS)
		return;

	switch (ind->message_type) {
	case CELL_SEARCH_INDICATION:
		process_cs_ind(ind);
		break;
	case BROADCAST_DETECT_INDICATION:
		process_bd_ind(ind);
		break;
	case SYSTEM_INFORMATION_SCHEDULE_INDICATION:
	case SYSTEM_INFORMATION_INDICATION:
		process_si_ind(ind);
		break;
	default:
		cmdline_printf(cl, "Unknown indication type!\n");
		break;
	}
}

static void dbg_response_cli_cb(nmm_dbg_response_t *res)
{
	printf("Received dbg command result: type = %s, err code = %s\n",
	       dbg_type_to_text(res->message_type),
	       fapi_error_to_text(res->error_code));
}

static struct nmm_cb cli_callbacks = {
	.dbg_receive_response = dbg_response_cli_cb,
	.fapi_receive_response = fapi_response_cli_cb,
	.fapi_receive_indication = fapi_indication_cli_cb,
};

static void parse_args(__attribute__((unused)) int argc,
		       __attribute__((unused)) char **argv)
{
	int i, opt;
	struct option lgopt[] = {{"version", 0, 0, 0}, {NULL, 0, 0, 0}};

	opterr = 0;
	while ((opt = getopt_long(argc, argv, "", lgopt, &i)) != EOF) {
		switch (opt) {
		case 0:
			if (!strcmp(lgopt[i].name, "version")) {
				print_version();
				exit(0);
			}
			break;
		default:
			break;
		}
	}
}

int main(int argc, char **argv)
{
	int ret;

	parse_args(argc, argv);

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	ret = nmm_init(&cli_callbacks);
	if (ret < 0) {
		printf("nmm_init failed\n");
		return ret;
	}

	cl = cmdline_stdin_new(nmm_test_ctx, "nmm> ");
	if (!cl) {
		printf("Cannot create command line interaface\n");
		return -1;
	}
	cmdline_interact(cl);
	cmdline_stdin_exit(cl);

	nmm_free();

	return 0;
}
