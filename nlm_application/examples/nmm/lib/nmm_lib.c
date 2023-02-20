/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021-2022 NXP
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <inttypes.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_errno.h>
#include <rte_bbdev.h>
#include <rte_malloc.h>
#include <rte_string_fns.h>
#include <unistd.h>
#include <rte_pmd_bbdev_la93xx.h>

#include "nmm_lib.h"
#include "nmm_host_if.h"
#include "nmm_vspa.h"
#include "nmm_elf.h"
#include "nmm_dl_proc.h"
#include "armral.h"
#include "ps_nr_pdcch.h"
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "mjson.h"
#define PORT 8080
// #include <complex.h>

/* BBDEV */
#define BBDEV_QUEUE_COUNT 2
#define OPS_POOL_SIZE 16
#define OPS_CACHE_SIZE 4

#define NMM_NO_OF_RETRIES (3)
#define NMM_CF_NO_OF_RETRIES (10 * NMM_NO_OF_RETRIES)

#define NMM_CB_RING_MAX_SIZE 16
char buffer[1024] = {0};
char reply[1000];

struct nmm_band_info
{
	enum nmm_rf_band band_id;
	uint16_t earfcn_low;
	uint16_t earfcn_high;
	uint32_t freq_low;
	uint32_t freq_high;
};

#define DEFAULT_UPDATE_INTERVAL 100 /* 100 Frames = 1 second */
#define MAX_FREQ_OFFSET 5500		/* PPB */

#define NMM_DEFAULT_FREQ 751000
#define NMM_DEFAULT_BAND NMM_RF_BAND_13

#define NMM_RF_BANDS 4
static struct nmm_band_info supported_bands[NMM_RF_BANDS] =
	{
		{
			.band_id = NMM_RF_BAND_3,
			.earfcn_low = 1200,
			.earfcn_high = 1949,
			.freq_low = 1805000,
			.freq_high = 1880000,
		},
		{
			.band_id = NMM_RF_BAND_13,
			.earfcn_low = 5180,
			.earfcn_high = 5279,
			.freq_low = 746000,
			.freq_high = 756000,
		},
		{
			.band_id = NMM_RF_BAND_77,
			.earfcn_low = 7711,
			.earfcn_high = 8329,
			.freq_low = 3300000,
			.freq_high = 4200000,
		},
		/* Defaults if earfcn unspecified */
		{
			.band_id = NMM_DEFAULT_BAND,
			.earfcn_low = 0,
			.earfcn_high = 0,
			.freq_low = NMM_DEFAULT_FREQ,
			.freq_high = NMM_DEFAULT_FREQ,
		}};

/* First entry is MIB */
#define MAX_SI_ARRAY_SIZE (MAX_SI + 1)
static const uint16_t si_periodicity_ms[] = {0, 80, 160, 320, 640, 1280, 2560, 5120};

#define SIB1_T 8
#define SIB1_n 0
#define SIB1_w 1
#define SIB1_ID 1
#define SIB1_SFN 5

struct si_params
{
	uint8_t id;
	uint16_t T_ms;
	uint8_t n;
};
struct dl_decode_data
{
	int16_t pcid;
	uint16_t retry_no;
	uint32_t step_no;
	mib_out_T mib_data;
	mib_extra_T mib_extra;
	uint8_t num_si;
	struct si_params si_params[MAX_SI_ARRAY_SIZE];
	uint8_t si_w;
	bool partial_ind;
};

struct nmm_state
{
	rte_atomic16_t initialized; /* nmm_init() can only be called once */
	rte_atomic16_t is_active;	/* NLM card state */
	uint8_t stop_in_progress;
	uint8_t vspa_active;
	uint8_t test_mode; /* int test mode enabled = 1 */
	uint8_t reset_in_progress;
	uint8_t reset_done;
	char capture_file[NMM_FILENAME_SIZE];
	int capture_size;
	uint32_t crt_cmd;
	uint32_t crt_freq;
	enum nmm_rf_band crt_band;
	int32_t crt_freq_off;
	struct dl_decode_data crt_decode;
};

struct fapi_ops
{
	uint16_t rsp_type;
	uint16_t ind_type;
	uint16_t lte_ind_size;
	int (*params_valid)(void *lte_req);
	uint16_t (*get_earfcn)(void *lte_req);
	uint16_t (*get_gscn)(void *nr_req);
	void (*build_cmd)(struct nmm_msg *msg, void *lte_req);
	int (*process_vspa_output)(void *lte_ind, uint8_t *stop);
};

struct dbg_ops
{
	uint16_t rsp_type;
	int (*params_valid)(void *req_params);
	void (*build_cmd)(struct nmm_msg *msg, void *req_params);
	int (*process_vspa_output)(uint8_t *stop);
};

struct rte_mempool *mp[BBDEV_QUEUE_COUNT];
uint16_t raw_buf_size;
uint32_t dev_id = BBDEV_IPC_DEV_ID_0;
struct nmm_cb *user_cbs;
mem_range_t *vspa_mem;
int do_poll = 0;
struct nmm_state state;
struct rte_ring *user_cb_ring;
timer_t cmd_timer;

/* Command timeout: how long to wait for a response from NLM card */
#ifdef NMM_LIB_TIMER_RESET
#define NMM_CMD_TIMEOUT_MSEC 12000
#else
#define NMM_CMD_TIMEOUT_MSEC 0
#endif

/* VSPA memory layout */
#define VSPA_MEM_ALIGN 64
#define MEM_CTRL_MAX_SIZE 0x1000		/* 4KB */
#define OVERLAY_SECTION_MAX_SIZE 0x1E00 /* 7.5KB */
#define VSPA_TABLE_MAX_SIZE 0x10000		/* 64KB */
#define VSPA_DATA_MAX_SIZE 0xE00000		/* 14MB */
#define VSPA_PROC_OUT_MAX_SIZE 0x500000 /* 1MB */

struct nmm_vspa_tbl_desc
{
	enum nmm_vspa_tbl_type id;
	char filename[NMM_NAME_LEN];
};

static struct nmm_vspa_tbl_desc vspa_tbl[] = {
	{VSPA_TBL_PSSDET_REF_TD, "PSSDET_REF_TD.bin"},
	{VSPA_TBL_PSS_REF_XCORR, "PSS_REF_XCORR.bin"},
};

#define VSPA_ELF_NAME "/lib/firmware/apm.eld"

struct nmm_vspa_overlay_sec
{
	enum nmm_vspa_overlay_type id;
	char sec_name[NMM_NAME_LEN];
};

static struct nmm_vspa_overlay_sec vspa_ovl[] = {
	{VSPA_OVL_TOOLS, ".tools"},
	{VSPA_OVL_CELL_SEARCH1, ".cell_search"},
	{VSPA_OVL_CELL_SEARCH2, ".cell_search_csrs"},
	{VSPA_OVL_CELL_FOLLOW1, ".cell_follow"},
	{VSPA_OVL_CELL_FOLLOW2, ".cell_follow_track"},
	{VSPA_OVL_MIB_DECODE, ".mib_extract"},
	{VSPA_OVL_FIRST, ".first"},
	{VSPA_OVL_SECOND, ".second"},
};

/* Logging */
#define nmm_print(fmt, ...)               \
	{                                     \
		printf("libnmm: %s: ", __func__); \
		printf(fmt, ##__VA_ARGS__);       \
	}

#ifdef NMM_LIB_DEBUG
#define nmm_print_dbg nmm_print

static void hexdump(char *buf, uint32_t len)
{
	int i;

	printf("Dump @%p[%d]:", buf, len);
	for (i = 0; i < len; i++)
	{
		if (i % 16 == 0)
			printf("\n%08x: ", i);
		printf("%02x ", buf[i]);
	}
	printf("\n");
}
#else
#define nmm_print_dbg(fmt, ...)
#define hexdump(buf, len)
#endif

static int write_file_data(char *filename, const void *data, uint32_t len)
{
	int fd, err;

	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		nmm_print("Error creating/opening file %s\n", filename);
		return -1;
	}

	err = write(fd, data, len);
	if (err < len)
	{
		nmm_print("Error writing to file %s\n", filename);
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

static int file_size(const char *filename)
{
	struct stat st;
	int err;

	err = stat(filename, &st);
	if (err != 0)
	{
		nmm_print("Error reading %s file size\n", filename);
		return err;
	}

	return st.st_size;
}

static int read_from_file(int fd, void *addr, uint32_t len, uint32_t offset,
						  uint32_t file_len)
{
	int count = 0;
	int ret;

	if (offset + len > file_len)
	{
		nmm_print("Trying to read beyond end of file\n");
		return -EINVAL;
	}

	do
	{
		ret = pread(fd, addr + count, len - count, offset + count);
		if (ret < 0)
			break;
		count += ret;
	} while (count < len);

	if (ret < 0)
		return -1;

	return count;
}

static int read_file_data(const char *filename, void *addr, uint32_t *len)
{
	int fd, err;
	int fsize;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		nmm_print("Error creating/opening file %s\n", filename);
		return -1;
	}

	/* Get file size */
	fsize = file_size(filename);
	if (fsize < 0)
	{
		close(fd);
		return fsize;
	}

	nmm_print_dbg("Reading %d bytes from file %s\n", fsize, filename);

	if (fsize > *len)
	{
		nmm_print("Not enough memory for data in %s (max %d)\n", filename, *len);
		close(fd);
		return -ENOMEM;
	}

	/* Read whole file */
	err = read_from_file(fd, addr, fsize, 0, fsize);
	close(fd);
	if (err < 0)
		return err;

	*len = err;
	return 0;
}

static int setup_raw_queue(struct rte_bbdev_info *info, uint32_t qid, bool dir)
{
	struct rte_bbdev_queue_conf qconf = {0};
	char pool_name[RTE_MEMPOOL_NAMESIZE];
	int ret;

	/* setup device queues */
	qconf.socket = 0;
	qconf.queue_size = info->drv.default_queue_conf.queue_size;
	qconf.priority = 0;
	qconf.deferred_start = 0;
	qconf.op_type = RTE_BBDEV_OP_RAW;

	qconf.raw_queue_conf.direction = dir;
	qconf.raw_queue_conf.conf_enable = 0;

	ret = rte_bbdev_queue_configure(dev_id, qid, &qconf);
	if (ret != 0)
	{
		nmm_print("Failure allocating queue 0 on dev%u\n", dev_id);
		return ret;
	}

	if (dir == RTE_BBDEV_DIR_MODEM_TO_HOST)
	{
		mp[qid] = NULL;
		return 0;
	}

	snprintf(pool_name, sizeof(pool_name), "pool_%u", dev_id);
	mp[qid] = rte_mempool_create(pool_name, OPS_POOL_SIZE,
								 sizeof(struct rte_bbdev_raw_op),
								 OPS_CACHE_SIZE, 0, NULL, NULL,
								 NULL, NULL, info->socket_id, 0);
	if (!mp[qid])
		return -rte_errno;

	return 0;
}

static int init_bbdev(void)
{
	const struct rte_bbdev_op_cap *op_cap;

	struct rte_bbdev_info info;
	int i, ret;

	/* Check if BBDEV device is present */
	if (!rte_bbdev_count())
	{
		nmm_print("No BBDEV device detected\n");
		return -ENODEV;
	}

	ret = rte_bbdev_info_get(0, &info);
	if (ret < 0)
	{
		nmm_print("rte_bbdev_info_get failed, ret: %d\n", ret);
		return ret;
	}

	/* Get maximum buffer size */
	op_cap = info.drv.capabilities;
	for (i = 0; op_cap->type != RTE_BBDEV_OP_NONE; ++i, ++op_cap)
	{
		if (op_cap->type == RTE_BBDEV_OP_RAW &&
			(op_cap->cap.raw.capability_flags &
			 RTE_BBDEV_RAW_CAP_INTERNAL_MEM))
			raw_buf_size =
				op_cap->cap.raw.max_internal_buffer_size;
	}
	nmm_print_dbg("Max buffer size = %u\n", raw_buf_size);

	if (raw_buf_size < NMM_MSG_SIZE)
	{
		nmm_print("BBDEV IPC buffers too small (%u), need %lu bytes\n",
				  raw_buf_size, NMM_MSG_SIZE);
		ret = -EINVAL;
		return ret;
	}

	/* setup device */
	ret = rte_bbdev_setup_queues(dev_id, BBDEV_QUEUE_COUNT, info.socket_id);
	if (ret < 0)
	{
		nmm_print("rte_bbdev_setup_queues failed, ret %d\n", ret);
		return ret;
	}

	/* Host to modem queue */
	ret = setup_raw_queue(&info, BBDEV_IPC_H2M_QUEUE,
						  RTE_BBDEV_DIR_HOST_TO_MODEM);
	if (ret != 0)
	{
		nmm_print("Failure configuring queue %d on dev%u\n",
				  BBDEV_IPC_H2M_QUEUE, dev_id);
		return ret;
	}

	/* Modem to host queue */
	ret = setup_raw_queue(&info, BBDEV_IPC_M2H_QUEUE,
						  RTE_BBDEV_DIR_MODEM_TO_HOST);
	if (ret != 0)
	{
		nmm_print("Failure allocating queue %d on dev%u\n",
				  BBDEV_IPC_M2H_QUEUE, dev_id);
		return ret;
	}

	/* Start bbdev */
	ret = rte_bbdev_start(dev_id);
	if (ret < 0)
	{
		nmm_print("rte_bbdev_start failed (%d)\n", ret);
		return ret;
	}

	return 0;
}

/* TODO Revert this at some point */
/* Use max size for overlay & table regions for easier VSPA integration & testing */
static int add_mem_region_entry(struct nmm_vspa_mem_ctrl *mem_info,
								char *name, uint32_t type, uint32_t id,
								uint32_t size, uint32_t max_size)
{
	struct nmm_vspa_mem_region *reg;

	if (mem_info->end + size > vspa_mem->size)
	{
		nmm_print("Not enough memory left to add region %s\n", name);
		return -ENOMEM;
	}

	reg = &mem_info->region[mem_info->num_regions];
	rte_strscpy(reg->name, name, NMM_NAME_LEN);
	reg->type = type;
	reg->id = id;
	reg->offset = mem_info->end;
	reg->size = size;

	mem_info->num_regions++;
	mem_info->end = RTE_ALIGN(mem_info->end + max_size, VSPA_MEM_ALIGN);

	return 0;
}

static int load_vspa_tables(struct nmm_vspa_mem_ctrl *mem_info)
{
	uint32_t size;
	int i, ret;
	void *crt_addr;

	for (i = 0; i < VSPA_TBL_MAX; i++)
	{
		crt_addr = vspa_mem->host_vaddr + mem_info->end;

		size = VSPA_TABLE_MAX_SIZE;
		ret = read_file_data(vspa_tbl[i].filename, crt_addr, &size);
		if (ret < 0)
			return ret;

		ret = add_mem_region_entry(mem_info, vspa_tbl[i].filename,
								   VSPA_MEM_TBL, vspa_tbl[i].id,
								   size, VSPA_TABLE_MAX_SIZE);
		if (ret < 0)
			return ret;
	}

	return 0;
}

/* Returns index in vspa_ovl table if matching section is found, -1 otherwise */
static int get_overlay_by_name(char *sec_name)
{
	int i;

	for (i = 0; i < VSPA_OVL_MAX; i++)
	{
		if (!strcmp(vspa_ovl[i].sec_name, sec_name))
			return i;
	}

	return -1;
}

static int is_overlay_section(struct section_header *shdr)
{
	return (shdr->sh_type >= SHT_LOPROC && shdr->sh_type <= SHT_HIPROC);
}

static int load_vspa_overlay(struct nmm_vspa_mem_ctrl *mem_info)
{
	struct elf_header ehdr;
	struct section_header *shdr;
	char *strsec;
	void *crt_addr;
	uint32_t align;
	int fd;
	int i, ret = 0;
	int ovl_id;
	int fsize;

	fd = open(VSPA_ELF_NAME, O_RDONLY);
	if (fd < 0)
	{
		nmm_print("Error opening %s\n", VSPA_ELF_NAME);
		return -1;
	}

	fsize = file_size(VSPA_ELF_NAME);
	if (fsize < 0)
	{
		ret = fsize;
		goto out_close;
	}

	/* Read elf file header */
	ret = read_from_file(fd, &ehdr, sizeof(ehdr), 0, fsize);
	if (ret < 0)
	{
		nmm_print("Error reading ELF header from %s\n", VSPA_ELF_NAME);
		goto out_close;
	}

	/* Read the section header table */
	shdr = rte_malloc(NULL, ehdr.fh_shentsize * ehdr.fh_shnum, 0);
	if (!shdr)
	{
		ret = -ENOMEM;
		goto out_close;
	}

	ret = read_from_file(fd, shdr, ehdr.fh_shentsize * ehdr.fh_shnum,
						 ehdr.fh_shoff, fsize);
	if (ret < 0)
	{
		nmm_print("Error reading section headers from %s\n",
				  VSPA_ELF_NAME);
		goto out_free_shdr;
	}

	/* Read the section containing section names */
	strsec = rte_malloc(NULL, shdr[ehdr.fh_shstrndx].sh_size, 0);
	if (!strsec)
	{
		ret = -ENOMEM;
		goto out_free_shdr;
	}

	ret = read_from_file(fd, strsec, shdr[ehdr.fh_shstrndx].sh_size,
						 shdr[ehdr.fh_shstrndx].sh_offset, fsize);
	if (ret < 0)
	{
		nmm_print("Error reading section names from %s\n",
				  VSPA_ELF_NAME);
		goto out_free;
	}

	/*
	 * Find overlay sections, identify them by name, and load them in
	 * the shared memory
	 */
	for (i = 0; i < ehdr.fh_shnum; i++)
	{
		if (!is_overlay_section(&shdr[i]))
			continue;
		ovl_id = get_overlay_by_name(strsec + shdr[i].sh_name);
		if (ovl_id < 0 || ovl_id > VSPA_OVL_MAX)
			continue;

		/* Elf sections have extra alignment requirements */
		align = RTE_MAX(shdr[i].sh_addralign, VSPA_MEM_ALIGN);
		mem_info->end = RTE_ALIGN(mem_info->end, align);
		crt_addr = vspa_mem->host_vaddr + mem_info->end;

		ret = read_from_file(fd, crt_addr, shdr[i].sh_size,
							 shdr[i].sh_offset, fsize);
		if (ret < 0)
		{
			nmm_print("Error reading section %s from %s\n",
					  vspa_ovl[ovl_id].sec_name, VSPA_ELF_NAME);
			goto out_free;
		}

		ret = add_mem_region_entry(mem_info, vspa_ovl[ovl_id].sec_name,
								   VSPA_MEM_OVERLAY, vspa_ovl[ovl_id].id,
								   shdr[i].sh_size, OVERLAY_SECTION_MAX_SIZE);
		if (ret < 0)
			goto out_free;
	}

out_free:
	rte_free(strsec);
out_free_shdr:
	rte_free(shdr);
out_close:
	close(fd);

	return ret;
}

static void dump_vspa_mem_info(struct nmm_vspa_mem_ctrl *mem_info)
{
	struct nmm_vspa_mem_region *reg;
	char name[NMM_NAME_LEN];
	int i;

	nmm_print_dbg("Control info in VSPA-shared memory:\n");
	for (i = 0; i < mem_info->num_regions; i++)
	{
		reg = &mem_info->region[i];
		rte_strscpy(name, reg->name, NMM_NAME_LEN);
		nmm_print_dbg("region[%d]: name = %s "
					  "type = %d id = %d offset = 0x%x size = %d\n",
					  i, name, reg->type, reg->id, reg->offset,
					  reg->size);
	}
}

static int reserve_cmd_regions(struct nmm_vspa_mem_ctrl *mem_info)
{
	int ret;

	ret = add_mem_region_entry(mem_info, "large_data",
							   VSPA_MEM_LARGE_DATA, 0,
							   VSPA_DATA_MAX_SIZE, VSPA_DATA_MAX_SIZE);
	if (ret < 0)
		return ret;

	ret = add_mem_region_entry(mem_info, "processing_out",
							   VSPA_MEM_PROCESSING_OUT, 0,
							   VSPA_PROC_OUT_MAX_SIZE, VSPA_PROC_OUT_MAX_SIZE);
	return ret;
}

static void *get_tx_buf(int qid);
static int send_msg(void *msg, uint32_t len);
static int send_mem_init_msg(void)
{
	struct nmm_msg *msg;

	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
		return -ENOMEM;

	msg->type = NMM_VSPA_MEM_INIT;
	msg->addr = vspa_mem->modem_phys;

	return send_msg(msg, sizeof(*msg));
}

static int init_vspa_mem(void)
{
	struct nmm_vspa_mem_ctrl *mem_info;
	int ret;

	vspa_mem = rte_pmd_la93xx_get_nlm_mem(dev_id);
	if (!vspa_mem)
	{
		nmm_print("Error reading VSPA shared memory\n");
		return -EINVAL;
	}
	nmm_print_dbg("vspa_mem: host virt = %p, modem phys = 0x%x, size = 0x%x\n",
				  vspa_mem->host_vaddr, vspa_mem->modem_phys, vspa_mem->size);

	mem_info = (struct nmm_vspa_mem_ctrl *)vspa_mem->host_vaddr;

	mem_info->num_regions = 0;
	mem_info->end = 0;
	add_mem_region_entry(mem_info, "ctrl", VSPA_MEM_CTRL, 0,
						 sizeof(*mem_info), MEM_CTRL_MAX_SIZE);

	/* VSPA OVERLAY sections */
	ret = load_vspa_overlay(mem_info);
	if (ret < 0)
	{
		nmm_print("Error loading VSPA overlay sections\n");
		return ret;
	}

	/* VSPA internal tables */
	ret = load_vspa_tables(mem_info);
	if (ret < 0)
	{
		nmm_print("Error loading VSPA internal tables\n");
		return ret;
	}

	ret = reserve_cmd_regions(mem_info);
	if (ret < 0)
		return ret;

	/* TODO: in debug mode only */
	dump_vspa_mem_info(mem_info);

	/* Notify VSPA that memory initialization is done */
	ret = send_mem_init_msg();
	if (ret < 0)
		return ret;

	return 0;
}

static uint32_t get_region_offset(enum nmm_vspa_mem_type type, uint32_t index)
{
	struct nmm_vspa_mem_ctrl *mem_info;
	int i;

	mem_info = (struct nmm_vspa_mem_ctrl *)vspa_mem->host_vaddr;

	for (i = 0; i < mem_info->num_regions; i++)
	{
		if (mem_info->region[i].type == type &&
			mem_info->region[i].id == index)
			break;
	}

	if (i == mem_info->num_regions)
	{
		nmm_print("Couldn't find VSPA mem region type %d, index %d\n",
				  type, index);
		return -1;
	}

	return mem_info->region[i].offset;
}

static void reset_modem(void);
static void cmd_timer_thread(union sigval si)
{
	/* If we got here, NLM card is unresponsive. Force a reset... */
	reset_modem();

	/* and clear all active flags */
	state.vspa_active = 0;
	rte_atomic16_clear(&state.is_active);
}

static int create_cmd_timer(void)
{
	struct sigevent se = {0};
	int ret;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &cmd_timer;
	se.sigev_notify_function = cmd_timer_thread;
	se.sigev_notify_attributes = NULL;

	ret = timer_create(CLOCK_REALTIME, &se, &cmd_timer);
	if (ret < 0)
	{
		nmm_print("Error creating cmd timer\n");
		return ret;
	}

	return 0;
}

static void delete_cmd_timer(void)
{
	timer_delete(cmd_timer);
}

static void set_cmd_timer(uint64_t timeout_ms)
{
	struct itimerspec ts = {0};
	int ret;

	ts.it_value.tv_sec = timeout_ms / 1000;
	ts.it_value.tv_nsec = (timeout_ms % 1000) * 1000000;
	/* One shot timer */
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;

	ret = timer_settime(cmd_timer, 0, &ts, 0);
	if (ret < 0)
		nmm_print("Error arming cmd timer\n");

	nmm_print_dbg("timer set to %ldms\n", timeout_ms);
}

static void arm_cmd_timer(void)
{
	set_cmd_timer(NMM_CMD_TIMEOUT_MSEC);
}

static void disarm_cmd_timer(void)
{
	set_cmd_timer(0);
}

#define ENQUEUE_RETRIES 100
static int send_msg(void *msg, uint32_t len)
{
	struct rte_bbdev_raw_op *raw_ops_enq[1] = {NULL};
	uint32_t qid = BBDEV_IPC_H2M_QUEUE;
	int retries = ENQUEUE_RETRIES;
	int ret = 0;

	if (state.reset_in_progress)
		return -EBUSY;

	ret = rte_mempool_get_bulk(mp[qid], (void **)raw_ops_enq, 1);
	if (ret < 0)
	{
		nmm_print("rte_mempool_get_bulk failed (%d)\n", ret);
		return ret;
	}

	raw_ops_enq[0]->input.is_direct_mem = 1;
	raw_ops_enq[0]->input.length = len;
	raw_ops_enq[0]->input.mem = msg;
	raw_ops_enq[0]->output.mem = NULL;
	raw_ops_enq[0]->output.length = 0;

	nmm_print_dbg("preparing to send msg: buf = %p, len = %d\n",
				  raw_ops_enq[0]->input.mem, raw_ops_enq[0]->input.length);
	hexdump(raw_ops_enq[0]->input.mem, len);

	while (retries--)
	{
		ret = rte_bbdev_enqueue_raw_op(dev_id, qid, raw_ops_enq[0]);
		if (ret == 0)
			break;
		if (ret == -EBUSY)
			continue;
	}
	if (ret < 0)
		nmm_print("rte_bbdev_enqueue_raw_op failed (%d)\n", ret);

	rte_mempool_put_bulk(mp[qid], (void **)raw_ops_enq, 1);

	return ret;
}

#define GET_BUF_RETRIES 10
static void *get_tx_buf(int qid)
{
	void *buf;
	uint32_t len;
	int retries = GET_BUF_RETRIES;

	while (retries--)
	{
		buf = rte_bbdev_get_next_internal_buf(dev_id, qid, &len);
		if (buf)
		{
			memset(buf, 0, len);
			return buf;
		}
	}

	nmm_print("Failed to get internal buffer\n");
	return NULL;
}

static int send_nmm_command(struct nmm_msg *msg)
{
	int ret;

	ret = send_msg((char *)msg, NMM_MSG_SIZE);
	if (ret < 0)
		return ret;

	return ret;
}

static int enq_response(uint32_t type, uint32_t err)
{
	fapi_response_t *rsp;

	rsp = rte_malloc(NULL, sizeof(*rsp), 0);
	if (!rsp)
	{
		nmm_print("Error in rte_malloc\n");
		return -ENOMEM;
	}

	rsp->message_type = type;
	rsp->error_code = err;
	rte_ring_enqueue(user_cb_ring, rsp);

	return 0;
}

static int gscn_valid(uint16_t gscn)
{
	int i;

	for (i = 0; i < NMM_RF_BANDS; i++)
		if (gscn >= supported_bands[i].earfcn_low &&
			gscn <= supported_bands[i].earfcn_high)
			return 1;
	return 0;
}

static int earfcn_valid(uint16_t earfcn)
{
	int i;

	for (i = 0; i < NMM_RF_BANDS; i++)
		if (earfcn >= supported_bands[i].earfcn_low &&
			earfcn <= supported_bands[i].earfcn_high)
			return 1;
	return 0;
}

static void translate_earfcn(uint16_t earfcn, enum nmm_rf_band *band_id,
							 uint32_t *freq)
{
	struct nmm_band_info *band;
	int i;

	for (i = 0; i < NMM_RF_BANDS; i++)
	{
		band = &supported_bands[i];
		if (earfcn >= band->earfcn_low && earfcn <= band->earfcn_high)
			break;
	}

	/* We shouldn't be here, valid EARFCN parameter is expected */
	if (i == NMM_RF_BANDS)
	{
		nmm_print("EARFCN (%d) not supported\n", earfcn);
		return;
	}

	*band_id = band->band_id;
	*freq = band->freq_low + (earfcn - band->earfcn_low) * 100;
	if (earfcn == 0)
	{
		nmm_print_dbg("EARFCN 0, using default: band = %d, freq = %ukHz\n",
					  *band_id, *freq);
	}
	else
	{
		nmm_print_dbg("Requested RF cfg: band = %d, freq = %ukHz\n",
					  *band_id, *freq);
	}

	/* If in test mode, do nothing */
	if (state.test_mode)
	{
		nmm_print_dbg("In test mode, will not configure RF\n");
		*band_id = NMM_RF_BAND_NO_CHANGE;
		return;
	}

	/*
	 * If same as the EARFCN for which we currently configured the RF,
	 * do nothing
	 */
	if (*freq == state.crt_freq && *band_id == state.crt_band)
	{
		nmm_print_dbg("Frequency %ukHz already set\n", *freq);
		*band_id = NMM_RF_BAND_NO_CHANGE;
	}
}

/*** Funtion for loading Custom File from host ***/

static int load_fill_mem_custom_file(char *filename, void *crt_addr, uint32_t *size)
{
	int ret;

	ret = read_file_data(filename, crt_addr, size);

	if (ret < 0)
		return ret;

	return 0;
}

/*** DBG commands ***/

/* Common capture functions */
static void capture_build_cmd(struct nmm_msg *msg, void *p, uint32_t type)
{
	nmm_rx_capture_request_t *params = (nmm_rx_capture_request_t *)p;

	msg->type = type;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_LARGE_DATA, 0);
	if (!state.test_mode)
		msg->freq_off = state.crt_freq_off;
	msg->size = params->length;

	translate_earfcn(params->earfcn, &msg->band, &msg->freq_khz);

	/* Store parameters that won't be passed to FreeRTOS */
	rte_strscpy(state.capture_file, params->filename, NMM_FILENAME_SIZE);
	state.capture_size = params->length;

	/* Store new band/freq values */
	state.crt_band = msg->band;
	state.crt_freq = msg->freq_khz;
}

static int capture_params_valid(void *p)
{
	nmm_rx_capture_request_t *params = (nmm_rx_capture_request_t *)p;

	if (!earfcn_valid(params->earfcn))
	{
		nmm_print("Requested invalid EARFCN (%d)\n", params->earfcn);
		return -1;
	}

	if (params->length > VSPA_DATA_MAX_SIZE)
	{
		nmm_print("Requested capture size (%d) is too large (max %d)\n",
				  params->length, VSPA_DATA_MAX_SIZE);
		return -1;
	}

	return 0;
}

static int process_capture_output(uint8_t *stop)
{
	char *dump_addr = NULL;
	uint32_t offset;
	int err;

	/* Write captured data to file */
	offset = get_region_offset(VSPA_MEM_LARGE_DATA, 0);
	dump_addr = vspa_mem->host_vaddr + offset;
	err = write_file_data(state.capture_file, dump_addr, state.capture_size);
	if (err)
		return err;

	/* Make sure we don't accidentally overwrite it later */
	rte_strscpy(state.capture_file, "", NMM_FILENAME_SIZE);
	state.capture_size = 0;

	return 0;
}

/* DBG Commands for Fill Mem Custom */
static void mem_custom_build_cmd(struct nmm_msg *msg, void *p, uint32_t type)
{
	uint32_t size;
	uint32_t *crt_addr;

	nmm_fill_mem_custom_request_t *params = (nmm_fill_mem_custom_request_t *)p;

	msg->type = type;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_LARGE_DATA, 0);
	crt_addr = vspa_mem->host_vaddr + get_region_offset(VSPA_MEM_LARGE_DATA, 0);
	size = VSPA_TABLE_MAX_SIZE;

	load_fill_mem_custom_file(params->filename, (void *)crt_addr, &size);
	msg->size = size;
}

static int fill_mem_custom_params_valid(void *p)
{
	return 0;
}

/* Rx capture */
static void rx_capture_build_cmd(struct nmm_msg *msg, void *p)
{
	capture_build_cmd(msg, p, NMM_RX_CAPTURE);
}

static struct dbg_ops rxc_ops = {
	.rsp_type = NMM_DBG_RX_CAPTURE_RESULT,
	.params_valid = capture_params_valid,
	.build_cmd = rx_capture_build_cmd,
	.process_vspa_output = process_capture_output,
};

/* Fill Mem Custom */
static void fill_mem_custom_build_cmd(struct nmm_msg *msg, void *p)
{
	mem_custom_build_cmd(msg, p, NMM_FILL_MEM_CUSTOM);
}

static struct dbg_ops fill_mem_custom_ops = {
	.rsp_type = NMM_DBG_CMD_FILL_MEM_CUSTOM_RESULT,
	.params_valid = fill_mem_custom_params_valid,
	.build_cmd = fill_mem_custom_build_cmd,
};

/* Decimation capture */
static void dec_capture_build_cmd(struct nmm_msg *msg, void *p)
{
	capture_build_cmd(msg, p, NMM_DEC_CAPTURE);
}

static struct dbg_ops dec_ops = {
	.rsp_type = NMM_DBG_DECIMATOR_CAPTURE_RESULT,
	.params_valid = capture_params_valid,
	.build_cmd = dec_capture_build_cmd,
	.process_vspa_output = process_capture_output,
};

/* Enable test mode */
static int test_mode_enable_params_valid(void *p)
{
	nmm_test_mode_enable_request_t *params;
	struct stat st;

	params = (nmm_test_mode_enable_request_t *)p;

	if (stat(params->filename, &st) != 0)
	{
		nmm_print("Test vector file (%s) does not exist\n",
				  params->filename);
		return -1;
	}
	if (st.st_size > VSPA_DATA_MAX_SIZE)
	{
		nmm_print("Test vector file (%ldB) too large (max %dB)\n",
				  st.st_size, VSPA_DATA_MAX_SIZE);
		return -1;
	}

	return 0;
}

static void test_mode_enable_build_cmd(struct nmm_msg *msg, void *p)
{
	nmm_test_mode_enable_request_t *params;
	uint32_t size, offset;
	int err;

	params = (nmm_test_mode_enable_request_t *)p;

	offset = get_region_offset(VSPA_MEM_LARGE_DATA, 0);
	size = VSPA_DATA_MAX_SIZE;
	err = read_file_data(params->filename,
						 vspa_mem->host_vaddr + offset, &size);
	if (err)
	{
		/*
		 * No reason we should be here, we already checked file exists
		 * and size not too large
		 */
		nmm_print("Error reading from file %s\n", params->filename);
		return;
	}

	msg->type = NMM_INT_TEST_ENABLE;
	msg->addr = vspa_mem->modem_phys + offset;
	msg->size = size;
}

static struct dbg_ops tme_ops = {
	.rsp_type = NMM_DBG_TEST_MODE_ENABLE_RESULT,
	.params_valid = test_mode_enable_params_valid,
	.build_cmd = test_mode_enable_build_cmd,
	.process_vspa_output = NULL,
};

/* Test mode disable */
static void test_mode_disable_build_cmd(struct nmm_msg *msg, void *p)
{
	msg->type = NMM_INT_TEST_DISABLE;
}

static struct dbg_ops tmd_ops = {
	.rsp_type = NMM_DBG_TEST_MODE_DISABLE_RESULT,
	.params_valid = NULL,
	.build_cmd = test_mode_disable_build_cmd,
	.process_vspa_output = NULL,
};

/* Force reset */
static void reset_build_cmd(struct nmm_msg *msg, void *p)
{
	msg->type = NMM_RESET;
}

static struct dbg_ops rst_ops = {
	.rsp_type = NMM_DBG_FORCE_RESET_RESULT,
	.params_valid = NULL,
	.build_cmd = reset_build_cmd,
	.process_vspa_output = NULL,
};

/* DBG commands processing */
static uint32_t dbg_req_to_hif_cmd_type(uint32_t type)
{
	switch (type)
	{
	case NMM_DBG_RX_CAPTURE_REQUEST:
		return NMM_RX_CAPTURE;
	case NMM_DBG_TEST_MODE_ENABLE_REQUEST:
		return NMM_INT_TEST_ENABLE;
	case NMM_DBG_TEST_MODE_DISABLE_REQUEST:
		return NMM_INT_TEST_DISABLE;
	case NMM_DBG_DECIMATOR_CAPTURE_REQUEST:
		return NMM_DEC_CAPTURE;
	case NMM_DBG_CMD_FILL_MEM_CUSTOM_REQUEST:
		return NMM_FILL_MEM_CUSTOM;
	case NMM_DBG_FORCE_RESET_REQUEST:
		return NMM_RESET;
	default:
		return NMM_MAX_OPS;
	}
}

static struct dbg_ops *get_dbg_ops(uint32_t message_type)
{
	switch (message_type)
	{
	case NMM_RX_CAPTURE:
		return &rxc_ops;
	case NMM_DEC_CAPTURE:
		return &dec_ops;
	case NMM_INT_TEST_ENABLE:
		return &tme_ops;
	case NMM_INT_TEST_DISABLE:
		return &tmd_ops;
	case NMM_FILL_MEM_CUSTOM:
		return &fill_mem_custom_ops;
	case NMM_RESET:
		return &rst_ops;
	default:
		return NULL;
	}
}

static int process_dbg_request(nmm_dbg_request_t *req)
{
	struct dbg_ops *ops;
	void *req_params;
	struct nmm_msg *msg;
	int ret;

	ops = get_dbg_ops(dbg_req_to_hif_cmd_type(req->type));
	if (!ops)
	{
		nmm_print("DBG request type %d not supported\n", req->type);
		return -EOPNOTSUPP;
	}

	if (rte_atomic16_test_and_set(&state.is_active) == 0)
	{
		nmm_print("NMM already active!\n");
		return enq_response(ops->rsp_type, MSG_INVALID_STATE);
	}

	req_params = &req->body;
	if (ops->params_valid && ops->params_valid(req_params) != 0)
	{
		ret = enq_response(ops->rsp_type, MSG_INVALID_CONFIG);
		goto out_err;
	}

	if (state.stop_in_progress)
	{
		nmm_print("STOP detected, aborting current command\n");
		ret = enq_response(ops->rsp_type, MSG_PROCEDURE_STOPPED);
		goto out_err;
	}

	/* Send message to FreeRTOS */
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		ret = -ENOMEM;
		goto out_err;
	}

	ops->build_cmd(msg, req_params);
	ret = send_nmm_command(msg);
	if (ret)
		goto out_err;

	state.vspa_active = 1;

	return 0;

out_err:
	rte_atomic16_clear(&state.is_active);
	return ret;
}

int nmm_dbg_send_request(nmm_dbg_request_t *req)
{
	switch (req->type)
	{
	case NMM_DBG_RX_CAPTURE_REQUEST:
	case NMM_DBG_DECIMATOR_CAPTURE_REQUEST:
	case NMM_DBG_TEST_MODE_ENABLE_REQUEST:
	case NMM_DBG_TEST_MODE_DISABLE_REQUEST:
	case NMM_DBG_CMD_FILL_MEM_CUSTOM_REQUEST:
	case NMM_DBG_FORCE_RESET_REQUEST:
		return process_dbg_request(req);
	default:
		nmm_print("DBG request type (%d) not supported\n", req->type);
		return -EOPNOTSUPP;
	}

	return 0;
}

/*** MIB/SIB helpers ***/

static int send_time_update(uint32_t sfn)
{
	struct nmm_msg *msg_out;

	/* Send message to FreeRTOS */
	msg_out = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg_out)
		return -ENOMEM;
	msg_out->type = NMM_TIME_UPDATE;
	msg_out->sfn = sfn;

	return send_nmm_command(msg_out);
}

static int max_decode_retries(void)
{
	if (state.crt_cmd == CELL_FOLLOW_REQUEST)
		return NMM_CF_NO_OF_RETRIES;
	return NMM_NO_OF_RETRIES;
}

static int send_nmm_stop_command(void);
static int request_dl_dump()
{
	struct nmm_msg *msg_out;
	uint8_t index;

	if (state.crt_decode.retry_no >= max_decode_retries())
	{
		nmm_print("DL decoding failed\n");
		send_nmm_stop_command();
		return -ETIMEDOUT;
	}

	/* Send message to FreeRTOS */
	msg_out = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg_out)
		return -ENOMEM;
	msg_out->type = NMM_DL_DUMP;

	index = state.crt_decode.step_no;
	msg_out->si_T = state.crt_decode.si_params[index].T_ms;
	msg_out->si_n = state.crt_decode.si_params[index].n;
	msg_out->si_w = state.crt_decode.si_w;

	send_nmm_command(msg_out);
	state.crt_decode.retry_no++;

	return -EAGAIN;
}

/*
 * Returns: calculated SFN if successful; -EAGAIN if retry requested;
 * -ETIMEDOUT if retry count reached
 */
static int decode_mib(uint8_t *mib_raw)
{
	mib_out_T *mib_data = &state.crt_decode.mib_data;
	mib_extra_T *mib_extra = &state.crt_decode.mib_extra;
	cfixed16_t *dump_addr;
	uint32_t offset;
	uint32_t calculated_sfn;

	offset = get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	dump_addr = vspa_mem->host_vaddr + offset;

	if (mib_process(dump_addr, state.crt_decode.pcid, mib_data, mib_extra, mib_raw))
		return request_dl_dump();

	calculated_sfn = mib_data->sfn * 4 + mib_extra->minor_sfn;

	nmm_print_dbg(" --- MIB dump --- \n");
	nmm_print_dbg("BandWidth = %.2f MHz\n", mib_extra->bw_MHz);
	nmm_print_dbg("Extended = %s, Ng = %s\n", mib_data->extend_phich ? "true" : "false", mib_extra->phich);
	nmm_print_dbg("SFN = %d (%dx4+%d)\n", calculated_sfn, mib_data->sfn, mib_extra->minor_sfn);
	nmm_print_dbg("MIB reserved bits = %d\n", mib_data->reserved);
	nmm_print_dbg("No Tx Antennas = %d\n", mib_extra->num_tx);

	return calculated_sfn;
}

static bool sib1_requested(void)
{
	return (state.crt_decode.si_w == SIB1_w);
}

#define SUBFRM_MAX_OFFSET 2047
#define MAX_FRAME_NUM 1023
static int decode_sib(char *sib, size_t *sib_len)
{
	mib_out_T *mib_data = &state.crt_decode.mib_data;
	mib_extra_T *mib_extra = &state.crt_decode.mib_extra;
	cfixed16_t *dump_addr;
	uint32_t offset;
	uint32_t offset_subframe_start;
	uint32_t frame_num;
	uint16_t si_x;
	uint8_t sf_start;
	uint8_t si_n, si_w, index;

	index = state.crt_decode.step_no;
	offset = get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	dump_addr = vspa_mem->host_vaddr + offset;

	offset_subframe_start = (uint32_t)((uint32_t *)dump_addr)[0];
	if (offset_subframe_start > SUBFRM_MAX_OFFSET)
	{
		nmm_print_dbg("Received invalid subfrm offset (%d)\n",
					  offset_subframe_start);
		return request_dl_dump();
	}
	frame_num = (uint32_t)((uint32_t *)dump_addr)[1];
	if (frame_num > MAX_FRAME_NUM)
	{
		nmm_print_dbg("Received invalid frame number (%d)\n",
					  frame_num);
		return request_dl_dump();
	}

	nmm_print_dbg("Jumped to offset = %d, current frame = %d\n",
				  offset_subframe_start, frame_num);
	dump_addr = &dump_addr[offset_subframe_start];

	si_n = state.crt_decode.si_params[index].n;
	si_w = state.crt_decode.si_w;

	if (sib1_requested())
	{
		sf_start = SIB1_SFN;
	}
	else
	{
		si_x = (si_n - 1) * si_w;
		sf_start = si_x % 10;
	}

	if (sib_process(dump_addr, state.crt_decode.pcid, si_w, sf_start,
					frame_num, mib_data, mib_extra, sib, sib_len))
	{
		nmm_print_dbg("SIB decode failed (try %d)\n", state.crt_decode.retry_no);
		return request_dl_dump();
	}

	return 0;
}

/*** FAPI commands ***/

/* NMM STOP */
static int send_nmm_stop_command(void)
{
	struct nmm_msg *msg;

	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
		return -ENOMEM;
	msg->type = NMM_STOP;

	return send_nmm_command(msg);
}

static int process_stop_request(void)
{
	/* If no active procedure, respond now */
	if (rte_atomic16_read(&state.is_active) == 0)
		return enq_response(NMM_STOP_RESPONSE, MSG_NMM_STOP_IGNORED);

	state.stop_in_progress = 1;

	/* If current command is being run on VSPA, send it a stop request */
	if (state.vspa_active == 1)
		return send_nmm_stop_command();

	/*
	 * Otherwise, current command runs on host. Wait for it to finish
	 * or hit a checkpoint and abort on its own.
	 */
	while (rte_atomic16_read(&state.is_active) > 0)
	{
		usleep(100);
		/*
		 * This may happen if we checked the vspa_active flag *just*
		 * efore the active command thread set it.
		 */
		if (state.vspa_active == 1)
			return send_nmm_stop_command();
	}

	state.stop_in_progress = 0;
	return enq_response(NMM_STOP_RESPONSE, MSG_NMM_STOP_OK);
}

/* Cell search */
static int cell_search_params_valid(void *req)
{
	lte_cell_search_request_t *cs_req = (lte_cell_search_request_t *)req;

	if (cs_req->tag != LTE_CELL_SEARCH_REQUEST_TAG)
	{
		nmm_print("Invalid tag (%d)\n", cs_req->tag);
		return -1;
	}
	if (cs_req->exhaustive_search != 1)
	{
		nmm_print("Only exhaustive search is supported\n");
		return -1;
	}
	if (cs_req->timeout != 0)
	{
		nmm_print("timeout not supported for now\n");
		return -1;
	}
	if (cs_req->num_pci != 0)
	{
		nmm_print("PCI list not supported\n");
		return -1;
	}
	if (!earfcn_valid(cs_req->earfcn))
	{
		nmm_print("Requested EARFCN (%d) not supported\n", cs_req->earfcn);
		return -1;
	}

	return 0;
}

static void build_band_scan_n77_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_BAND_SCAN_N77;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}
static void build_ssb_scan_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_SSB_SCAN;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}
static void build_cell_follow_stop_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_CELL_FOLLOW_STOP;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}

static void build_cell_search_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_CELL_SEARCH;
	if (!state.test_mode)
		msg->freq_off = state.crt_freq_off;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}

static uint16_t cell_search_get_earfcn(void *req)
{
	return ((lte_cell_search_request_t *)req)->earfcn;
}

static int cs_result_vspa_to_fapi(void *ind, uint8_t *stop)
{
	lte_cell_search_indication_t *lte_cs = (lte_cell_search_indication_t *)ind;
	ssdet_out_T *ssdet_out;
	uint32_t offset;
	int32_t freq_off;

	offset = get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	ssdet_out = (ssdet_out_T *)(vspa_mem->host_vaddr + offset);

	lte_cs->tag = LTE_CELL_SEARCH_INDICATION_TAG;
	lte_cs->num_cells = ssdet_out->cell_found ? 1 : 0;
	lte_cs->length = sizeof(lte_cs->tag) + sizeof(lte_cs->length) +
					 sizeof(lte_cs->num_cells) +
					 lte_cs->num_cells * sizeof(lte_cell_info_t);

	if (!ssdet_out->cell_found)
		return 0;

	lte_cs->cell_info[0].pci = (uint16_t)ssdet_out->cellid;
	lte_cs->cell_info[0].rsrp = (uint8_t)roundf(-ssdet_out->rsrp);
	lte_cs->cell_info[0].rsrq = (uint8_t)roundf(-ssdet_out->rsrq);
	lte_cs->cell_info[0].freq_offset = (int16_t)
		roundf((float)ssdet_out->nco_psscfo * FREQ_KHZ_TO_PPB(state.crt_freq));

	/* Store first frequency offset value for future calibrations */
	freq_off = lte_cs->cell_info[0].freq_offset;
	if (!state.test_mode && !state.crt_freq_off)
	{
		if (freq_off < MAX_FREQ_OFFSET && freq_off > -MAX_FREQ_OFFSET)
		{
			nmm_print_dbg("Using freq offset %d PPB for VCXO calibration\n",
						  freq_off);
			state.crt_freq_off = freq_off;
		}
	}

	nmm_print_dbg("ssdet out len = %lu, lte_cs->length = %d\n",
				  sizeof(ssdet_out_T), lte_cs->length);
	nmm_print_dbg("cell found = %d\n", ssdet_out->cell_found);
	nmm_print_dbg("pci = %d %d\n", lte_cs->cell_info[0].pci, ssdet_out->cellid);
	nmm_print_dbg("rsrp = %d %f dBm\n", lte_cs->cell_info[0].rsrp, ssdet_out->rsrp);
	nmm_print_dbg("rsrq = %d %f dB\n", lte_cs->cell_info[0].rsrq, ssdet_out->rsrq);
	nmm_print_dbg("freq_offset = %d PPB %d Hz\n", lte_cs->cell_info[0].freq_offset,
				  ssdet_out->nco_psscfo);

	return 0;
}

static struct fapi_ops n77bs_ops = {

	.rsp_type = BAND_SCAN_N77_RESPONSE,
	.build_cmd = build_band_scan_n77_cmd,
};
static struct fapi_ops ss_ops = {

	.rsp_type = SSB_SCAN_RESPONSE,
	.build_cmd = build_ssb_scan_cmd,
};
static struct fapi_ops cfs_ops = {

	.rsp_type = CELL_FOLLOW_STOP_RESPONSE,
	.build_cmd = build_cell_follow_stop_cmd,
};

static struct fapi_ops cs_ops = {
	.rsp_type = CELL_SEARCH_RESPONSE,
	.ind_type = CELL_SEARCH_INDICATION,
	.lte_ind_size = sizeof(lte_cell_search_indication_t),
	.params_valid = cell_search_params_valid,
	.get_earfcn = cell_search_get_earfcn,
	.build_cmd = build_cell_search_cmd,
	.process_vspa_output = cs_result_vspa_to_fapi,
};

static void setgscn(struct nmm_msg *msg, void *req)
{
	nr_cell_follow_request_t *cf_req = (nr_cell_follow_request_t *)req;
	msg->gscn = ((nr_cell_follow_request_t *)cf_req)->gscn;
}

/* Cell follow */
#define NMM_PCI_MIN 0
#define NMM_PCI_MAX 503
#define NMM_CF_UPDATE_INTERVAL_MIN 1
#define NMM_CF_UPDATE_INTERVAL_MAX 1024
static int cell_follow_params_valid(void *req)
{
	nr_cell_follow_request_t *cf_req = (nr_cell_follow_request_t *)req;
	if (!gscn_valid(cf_req->gscn))
	{
		nmm_print("Invalid GSCN (%d)\n", cf_req->gscn);
		return -1;
	}

	return 0;
}

static void build_cell_follow_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_CELL_FOLLOW;
	setgscn(msg, req);
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}
static void build_cell_follow_mib_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_CELL_FOLLOW_MIB;
	setgscn(msg, req);
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
}
static void build_cell_follow_sib_1_cmd(struct nmm_msg *msg, void *req)
{
	msg->type = NMM_CELL_FOLLOW_SIB_1;
	setgscn(msg, req);
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_LARGE_DATA, 0);
}

static uint16_t cell_follow_get_earfcn(void *req)
{
	return ((nr_cell_follow_request_t *)req)->gscn;
}

static int cf_aid_vspa_with_sfn(void *ind, uint8_t *stop)
{
	uint32_t calculated_sfn;
	struct timespec spec;
	uint32_t time_shift;
	int ret;

	ret = decode_mib(NULL);
	if (ret < 0)
		return ret;

	calculated_sfn = ret;
	clock_gettime(CLOCK_REALTIME, &spec);

	/* Get rough time, count how many 240 ms shifts there were */
	time_shift = spec.tv_sec % 256;											  // 10.24 sec wraps every 256 seconds
	time_shift = (((time_shift * 100) & 0xFFFFFFC00) | calculated_sfn) / 100; // correct time using SFN
	time_shift = (time_shift * 100 / 1024) * 240;							  // We need to shift 240ms for every 10.24sec
	time_shift = time_shift + 360;											  // time offset correction
	time_shift = 1000 - (time_shift % 1000);								  // Only keep the amount that is less than 1 second. The time goes forward

	send_time_update((time_shift << 16) | calculated_sfn);
	nmm_print_dbg("time_shift  = %u sfn = %u ; s = %ld \r\n", time_shift, calculated_sfn, spec.tv_sec);

	/* No further response expected from NLM card */
	disarm_cmd_timer();

	return -EAGAIN;
}

static struct fapi_ops cf_ops = {
	.rsp_type = CELL_FOLLOW_RESPONSE,
	.ind_type = CELL_FOLLOW_INDICATION,
	.params_valid = cell_follow_params_valid,
	.get_earfcn = cell_follow_get_earfcn,
	.build_cmd = build_cell_follow_cmd,
	.process_vspa_output = cf_aid_vspa_with_sfn,
};

static struct fapi_ops cfb_ops = {
	.rsp_type = CELL_FOLLOW_MIB_RESPONSE,
	.params_valid = cell_follow_params_valid,
	.get_earfcn = cell_follow_get_earfcn,
	.build_cmd = build_cell_follow_mib_cmd,
	.process_vspa_output = cf_aid_vspa_with_sfn,
};

static struct fapi_ops cfs1_ops = {
	.rsp_type = CELL_FOLLOW_SIB_1_RESPONSE,
	.params_valid = cell_follow_params_valid,
	.get_earfcn = cell_follow_get_earfcn,
	.build_cmd = build_cell_follow_sib_1_cmd,
	.process_vspa_output = cf_aid_vspa_with_sfn,
};

/* Broadcast detect */
static int bcast_detect_params_valid(void *req)
{
	lte_bcast_detect_request_t *bd_req = (lte_bcast_detect_request_t *)req;

	if (bd_req->tag != LTE_BROADCAST_DETECT_REQUEST_TAG)
	{
		nmm_print("Invalid tag (%d), expected %d\n",
				  bd_req->tag, LTE_BROADCAST_DETECT_REQUEST_TAG);
		return -1;
	}
	if (bd_req->length != sizeof(lte_bcast_detect_request_t))
	{
		nmm_print("Invalid length (%d), expected %lu\n",
				  bd_req->length, sizeof(lte_bcast_detect_request_t));
		return -1;
	}
	if (!earfcn_valid(bd_req->earfcn))
	{
		nmm_print("Invalid EARFCN (%d)\n", bd_req->earfcn);
		return -1;
	}
	if (bd_req->pci < NMM_PCI_MIN || bd_req->pci > NMM_PCI_MAX)
	{
		nmm_print("Invalid PCI (%d), expected range [%d..%d]\n",
				  bd_req->pci, NMM_PCI_MIN, NMM_PCI_MAX);
		return -1;
	}
	if (bd_req->timeout != 0)
	{
		nmm_print("timeout not supported for now\n");
		return -1;
	}

	return 0;
}

static void build_bcast_detect_cmd(struct nmm_msg *msg, void *req)
{
	lte_bcast_detect_request_t *bd_req = (lte_bcast_detect_request_t *)req;

	msg->type = NMM_DO_MIB;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	msg->cell_id = bd_req->pci;
	if (!state.test_mode)
		msg->freq_off = state.crt_freq_off;
	msg->update_int = DEFAULT_UPDATE_INTERVAL;

	memset(&state.crt_decode, 0, sizeof(struct dl_decode_data));
	state.crt_decode.pcid = (int16_t)bd_req->pci;
}

static uint16_t bcast_detect_get_earfcn(void *req)
{
	return ((lte_bcast_detect_request_t *)req)->earfcn;
}

static int bd_result_vspa_to_fapi(void *ind, uint8_t *stop)
{
	lte_bcast_detect_indication_t *bd_ind = (lte_bcast_detect_indication_t *)ind;
	mib_extra_T *mib_extra = &state.crt_decode.mib_extra;
	uint32_t calculated_sfn;
	int ret;

	ret = decode_mib(&bd_ind->mib[0]);
	if (ret < 0)
		return ret;

	calculated_sfn = ret;

	/* Decoding succesful, tell VSPA to stop */
	send_nmm_stop_command();

	/* ...and send indication to user */
	bd_ind->tag = LTE_BROADCAST_DETECT_INDICATION_TAG;
	bd_ind->length = sizeof(bd_ind->tag) + sizeof(bd_ind->length) +
					 sizeof(bd_ind->num_tx) + sizeof(bd_ind->sfn_offset) +
					 sizeof(bd_ind->mib);
	bd_ind->num_tx = mib_extra->num_tx;
	bd_ind->sfn_offset = calculated_sfn;
	bd_ind->mib_len = MIB_LEN;

	return -EINPROGRESS;
}

static struct fapi_ops bd_ops = {
	.rsp_type = BROADCAST_DETECT_RESPONSE,
	.ind_type = BROADCAST_DETECT_INDICATION,
	.lte_ind_size = sizeof(lte_bcast_detect_indication_t),
	.params_valid = bcast_detect_params_valid,
	.get_earfcn = bcast_detect_get_earfcn,
	.build_cmd = build_bcast_detect_cmd,
	.process_vspa_output = bd_result_vspa_to_fapi,
};

/* System information schedule */
static int sys_info_sched_params_valid(void *req)
{
	lte_sys_info_sched_request_t *sis_req = (lte_sys_info_sched_request_t *)req;

	if (sis_req->tag != LTE_SYSTEM_INFORMATION_SCHEDULE_REQUEST_TAG)
	{
		nmm_print("Invalid tag (%d), expected %d\n",
				  sis_req->tag, LTE_SYSTEM_INFORMATION_SCHEDULE_REQUEST_TAG);
		return -1;
	}
	if (sis_req->length != sizeof(lte_sys_info_sched_request_t))
	{
		nmm_print("Invalid length (%d), expected %lu\n",
				  sis_req->length, sizeof(lte_sys_info_sched_request_t));
		return -1;
	}
	if (!earfcn_valid(sis_req->earfcn))
	{
		nmm_print("Invalid EARFCN (%d)\n", sis_req->earfcn);
		return -1;
	}
	if (sis_req->pci < NMM_PCI_MIN || sis_req->pci > NMM_PCI_MAX)
	{
		nmm_print("Invalid PCI (%d), expected range [%d..%d]\n",
				  sis_req->pci, NMM_PCI_MIN, NMM_PCI_MAX);
		return -1;
	}
	if (sis_req->num_tx > 1)
	{
		nmm_print("Invalid num_tx (%d), expected 1\n", sis_req->num_tx);
		return -1;
	}
	/* TODO: dl_bw, phich_cfg, retry_count */
	if (sis_req->timeout != 0)
	{
		nmm_print("timeout not supported for now\n");
		return -1;
	}

	return 0;
}

static void build_sys_info_sched_cmd(struct nmm_msg *msg, void *req)
{
	lte_sys_info_sched_request_t *sis_req = (lte_sys_info_sched_request_t *)req;

	msg->type = NMM_DO_SIB;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	msg->cell_id = sis_req->pci;
	if (!state.test_mode)
		msg->freq_off = state.crt_freq_off;
	msg->update_int = DEFAULT_UPDATE_INTERVAL;

	memset(&state.crt_decode, 0, sizeof(struct dl_decode_data));
	state.crt_decode.pcid = (int16_t)sis_req->pci;
	state.crt_decode.num_si = 2;
	/* First entry is MIB (all zero), second is SIB1 */
	state.crt_decode.si_params[1].T_ms = SIB1_T;
	state.crt_decode.si_params[1].n = SIB1_n;
	state.crt_decode.si_params[1].id = SIB1_ID;
	state.crt_decode.si_w = SIB1_w;
}

static uint16_t sys_info_sched_get_earfcn(void *req)
{
	return ((lte_sys_info_sched_request_t *)req)->earfcn;
}

static int si_result_vspa_to_fapi(void *ind, uint8_t *stop)
{
	lte_sys_info_indication_t *si_ind = (lte_sys_info_indication_t *)ind;
	uint32_t calculated_sfn;
	int ret;

	/* step 0 is MIB dump */
	if (state.crt_decode.step_no == 0)
	{
		ret = decode_mib(NULL);
		if (ret < 0)
			return ret;

		calculated_sfn = ret;
		send_time_update(calculated_sfn);

		/* Request SIB dump */
		state.crt_decode.retry_no = 0;
		state.crt_decode.step_no++;
		return request_dl_dump();
	}

	/* SIB dumps start with step 1 */
	ret = decode_sib((char *)si_ind->sib, (size_t *)&(si_ind->sib_len));
	if (ret < 0)
		return ret;

	state.crt_decode.retry_no = 0;

	/* Decoding successful, fill indication */
	si_ind->tag = LTE_SYSTEM_INFORMATION_INDICATION_TAG;
	si_ind->sib_type = state.crt_decode.si_params[state.crt_decode.step_no].id;
	si_ind->length = sizeof(si_ind->tag) + sizeof(si_ind->length) +
					 sizeof(si_ind->sib_type) + sizeof(si_ind->sib_len) +
					 si_ind->sib_len;

	state.crt_decode.step_no++;
	if (state.crt_decode.step_no == state.crt_decode.num_si)
	{
		/* we're done here, but still need to wait for VSPA to stop */
		send_nmm_stop_command();
		return -EINPROGRESS;
	}

	/* More SIBs to decode */
	state.crt_decode.partial_ind = true;
	return request_dl_dump();
}

static struct fapi_ops sis_ops = {
	.rsp_type = SYSTEM_INFORMATION_SCHEDULE_RESPONSE,
	.ind_type = SYSTEM_INFORMATION_SCHEDULE_INDICATION,
	.lte_ind_size = sizeof(lte_sys_info_indication_t),
	.params_valid = sys_info_sched_params_valid,
	.get_earfcn = sys_info_sched_get_earfcn,
	.build_cmd = build_sys_info_sched_cmd,
	.process_vspa_output = si_result_vspa_to_fapi,
};

/* System information */
#define SI_PERIODICITY_MAX 7
#define SI_INDEX_MAX 32

static int is_si_window_valid(uint8_t value)
{
	uint8_t valid_array[] = {1, 2, 5, 10, 15, 20, 40};
	int array_size = sizeof(valid_array) / sizeof(valid_array[0]);
	int i;

	for (i = 0; i < array_size; i++)
		if (valid_array[i] == value)
			return 1;
	return 0;
}

static int sys_info_params_valid(void *req)
{
	lte_sys_info_request_t *si_req = (lte_sys_info_request_t *)req;
	int i;

	if (si_req->tag != LTE_SYSTEM_INFORMATION_REQUEST_TAG)
	{
		nmm_print("Invalid tag (%d), expected %d\n",
				  si_req->tag, LTE_SYSTEM_INFORMATION_REQUEST_TAG);
		return -1;
	}
	if (si_req->length > sizeof(lte_sys_info_request_t))
	{
		nmm_print("Invalid length (%d), max is %lu\n",
				  si_req->length, sizeof(lte_sys_info_request_t));
		return -1;
	}
	if (!earfcn_valid(si_req->earfcn))
	{
		nmm_print("Invalid EARFCN (%d)\n", si_req->earfcn);
		return -1;
	}
	if (si_req->pci < NMM_PCI_MIN || si_req->pci > NMM_PCI_MAX)
	{
		nmm_print("Invalid PCI (%d), expected range [%d..%d]\n",
				  si_req->pci, NMM_PCI_MIN, NMM_PCI_MAX);
		return -1;
	}
	if (si_req->num_si_periodicity < 1 ||
		si_req->num_si_periodicity > MAX_SI)
	{
		nmm_print("Invalid number of SI periodicity, expected range [%d..%d]\n",
				  1, MAX_SI);
		return -1;
	}
	for (i = 0; i < si_req->num_si_periodicity; i++)
	{
		if (si_req->si_periodicity[i] > SI_PERIODICITY_MAX)
		{
			nmm_print("Invalid SI periodicity (%d) for SIB%d\n",
					  si_req->si_periodicity[i], i + 2);
			return -1;
		}
		if (si_req->si_index[i] > SI_INDEX_MAX)
		{
			nmm_print("Invalid SI index (%d) for SIB%d\n",
					  si_req->si_index[i], i + 2);
			return -1;
		}
	}
	if (!is_si_window_valid(si_req->si_window_len))
	{
		nmm_print("Invalid SI window length (%dms)\n", si_req->si_window_len);
		return -1;
	}
	/* TODO: dl_bw, phich_cfg, retry_count */
	if (si_req->timeout != 0)
	{
		nmm_print("timeout not supported for now\n");
		return -1;
	}

	return 0;
}

static void build_sys_info_cmd(struct nmm_msg *msg, void *req)
{
	lte_sys_info_request_t *si_req = (lte_sys_info_request_t *)req;
	int i, index;

	msg->type = NMM_DO_SIB;
	msg->addr = vspa_mem->modem_phys +
				get_region_offset(VSPA_MEM_PROCESSING_OUT, 0);
	msg->cell_id = si_req->pci;
	if (!state.test_mode)
		msg->freq_off = state.crt_freq_off;
	msg->update_int = DEFAULT_UPDATE_INTERVAL;

	memset(&state.crt_decode, 0, sizeof(struct dl_decode_data));
	state.crt_decode.pcid = (int16_t)si_req->pci;

	/* Fill SIB params; index 0 reserved for MIB */
	index = 1;
	for (i = 0; i < si_req->num_si_periodicity; i++)
	{
		if (si_req->si_periodicity[i] == 0)
			continue;
		state.crt_decode.si_params[index].id = i + 2;
		state.crt_decode.si_params[index].T_ms =
			si_periodicity_ms[si_req->si_periodicity[i]];
		state.crt_decode.si_params[index++].n = si_req->si_index[i];

		nmm_print_dbg("SIB[%d]:_periodicity = %d, SI_index = %d\n",
					  i, si_req->si_periodicity[i], si_req->si_index[i]);
	}
	state.crt_decode.num_si = index;
	state.crt_decode.si_w = si_req->si_window_len;
}

static uint16_t sys_info_get_earfcn(void *req)
{
	return ((lte_sys_info_request_t *)req)->earfcn;
}

static struct fapi_ops si_ops = {
	.rsp_type = SYSTEM_INFORMATION_RESPONSE,
	.ind_type = SYSTEM_INFORMATION_INDICATION,
	.lte_ind_size = sizeof(lte_sys_info_indication_t),
	.params_valid = sys_info_params_valid,
	.get_earfcn = sys_info_get_earfcn,
	.build_cmd = build_sys_info_cmd,
	.process_vspa_output = si_result_vspa_to_fapi,
};

/* FAPI commands processing */
static uint32_t fapi_req_to_hif_cmd_type(uint32_t fapi_req)
{
	switch (fapi_req)
	{
	case CELL_SEARCH_REQUEST:
		return NMM_CELL_SEARCH;
	case CELL_FOLLOW_REQUEST:
		return NMM_CELL_FOLLOW;
	case BROADCAST_DETECT_REQUEST:
		return NMM_DO_MIB;
	case SYSTEM_INFORMATION_SCHEDULE_REQUEST:
		return NMM_DO_SIB;
	case SYSTEM_INFORMATION_REQUEST:
		return NMM_DO_SIB;
	case NMM_STOP_REQUEST:
		return NMM_STOP;
	case SSB_SCAN_REQUEST:
		return NMM_SSB_SCAN;
	case BAND_SCAN_N77_REQUEST:
		return NMM_BAND_SCAN_N77;
	case CELL_FOLLOW_MIB_REQUEST:
		return NMM_CELL_FOLLOW_MIB;
	case CELL_FOLLOW_STOP_REQUEST:
		return NMM_CELL_FOLLOW_STOP;
	default:
		return NMM_MAX_OPS;
	}
}

static struct fapi_ops *get_fapi_ops(uint32_t message_type)
{
	switch (message_type)
	{
	case NMM_CELL_SEARCH:
		return &cs_ops;
	case NMM_CELL_FOLLOW:
		return &cf_ops;
	case NMM_DO_MIB:
		return &bd_ops;
	case NMM_DO_SIB:
		if (sib1_requested())
			return &sis_ops;
		return &si_ops;
	default:
		return NULL;
	}
}

static struct fapi_ops *get_fapi_ops_by_req(uint32_t fapi_req)
{
	switch (fapi_req)
	{
	case CELL_SEARCH_REQUEST:
		return &cs_ops;
	case CELL_FOLLOW_REQUEST:
		return &cf_ops;
	case BROADCAST_DETECT_REQUEST:
		return &bd_ops;
	case SYSTEM_INFORMATION_SCHEDULE_REQUEST:
		return &sis_ops;
	case SYSTEM_INFORMATION_REQUEST:
		return &si_ops;
	case CELL_FOLLOW_MIB_REQUEST:
		return &cfb_ops;
	case CELL_FOLLOW_SIB_1_REQUEST:
		return &cfs1_ops;
	default:
		return NULL;
	}
}

static int process_fapi_request(fapi_request_t *req)
{
	struct fapi_ops *ops;
	void *lte_req;
	struct nmm_msg *msg;
	enum nmm_rf_band band = 0;
	uint32_t freq = 0;
	int ret = 0;

	ops = get_fapi_ops_by_req(req->message_type);
	if (!ops)
	{
		nmm_print("FAPI request of type %d not supported\n",
				  req->message_type);
		return -EOPNOTSUPP;
	}

	if (rte_atomic16_test_and_set(&state.is_active) == 0)
	{
		nmm_print("NMM already active!\n");
		return enq_response(ops->rsp_type, MSG_INVALID_STATE);
	}

	if (req->rat_type != LTE_RAT)
	{
		ret = enq_response(ops->rsp_type, MSG_RAT_NOT_SUPPORTED);
		goto out_err;
	}

	lte_req = &req->message_body;

	/* Param validation */
	if (ops->params_valid(lte_req) != 0)
	{
		ret = enq_response(ops->rsp_type, MSG_INVALID_CONFIG);
		goto out_err;
	}

	ret = enq_response(ops->rsp_type, MSG_OK);
	if (ret)
		goto out_err;

	if (state.stop_in_progress)
	{
		nmm_print("STOP detected, aborting current command\n");
		goto out_err;
	}

	/* Send message to FreeRTOS */
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		ret = -ENOMEM;
		goto out_err;
	}

	/* If command has an EARFCN param, translate it to band/freq pair */
	if (ops->get_earfcn)
	{
		translate_earfcn(ops->get_earfcn(lte_req), &band, &freq);
		msg->band = band;
		msg->freq_khz = freq;
		state.crt_band = band;
		state.crt_freq = freq;
	}

	/* Fill in other command specific fields and send it */
	ops->build_cmd(msg, lte_req);
	ret = send_nmm_command(msg);
	if (ret)
		goto out_err;

	state.vspa_active = 1;

	return 0;

out_err:
	rte_atomic16_clear(&state.is_active);
	return ret;
}

static int process_cell_follow_mib(fapi_request_nr_t *req)
{
	struct fapi_ops *ops;
	struct nmm_msg *msg;
	void *nr_req;
	int ret;
	ops = get_fapi_ops_by_req(req->message_type);
	if (!ops)
	{
		nmm_print("FAPI request of type %d not supported\n",
				  req->message_type);
		return -EOPNOTSUPP;
	}

	if (rte_atomic16_test_and_set(&state.is_active) == 0)
	{
		nmm_print("NMM already active!\n");
		return enq_response(ops->rsp_type, MSG_INVALID_STATE);
	}

	nr_req = &req->message_body;

	/* Param validation */
	if (ops->params_valid(nr_req) != 0)
	{
		ret = enq_response(ops->rsp_type, MSG_INVALID_CONFIG);
		goto out_err;
	}

	ret = enq_response(ops->rsp_type, MSG_OK);
	if (ret)
		goto out_err;

	if (state.stop_in_progress)
	{
		nmm_print("STOP detected, aborting current command\n");
		goto out_err;
	}

	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	ops->build_cmd(msg, nr_req);

	ret = send_nmm_command(msg);
	if (ret)
		return -1;

out_err:
	rte_atomic16_clear(&state.is_active);
	return ret;
}
// ###################################################
static int process_cell_follow_sib_1(fapi_request_nr_t *req)
{
	struct fapi_ops *ops;
	struct nmm_msg *msg;
	void *nr_req;
	int ret;
	ops = get_fapi_ops_by_req(req->message_type);
	if (!ops)
	{
		nmm_print("FAPI request of type %d not supported\n",
				  req->message_type);
		return -EOPNOTSUPP;
	}

	if (rte_atomic16_test_and_set(&state.is_active) == 0)
	{
		nmm_print("NMM already active!\n");
		return enq_response(ops->rsp_type, MSG_INVALID_STATE);
	}

	nr_req = &req->message_body;

	/* Param validation */
	if (ops->params_valid(nr_req) != 0)
	{
		ret = enq_response(ops->rsp_type, MSG_INVALID_CONFIG);
		goto out_err;
	}

	ret = enq_response(ops->rsp_type, MSG_OK);
	if (ret)
		goto out_err;

	if (state.stop_in_progress)
	{
		nmm_print("STOP detected, aborting current command\n");
		goto out_err;
	}

	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	ops->build_cmd(msg, nr_req);

	ret = send_nmm_command(msg);
	if (ret)
		return -1;

out_err:
	rte_atomic16_clear(&state.is_active);
	return ret;
}

static int process_cell_follow(fapi_request_nr_t *req)
{
	struct fapi_ops *ops;
	struct nmm_msg *msg;
	void *nr_req;
	int ret;
	ops = get_fapi_ops_by_req(req->message_type);
	if (!ops)
	{
		nmm_print("FAPI request of type %d not supported\n",
				  req->message_type);
		return -EOPNOTSUPP;
	}

	if (rte_atomic16_test_and_set(&state.is_active) == 0)
	{
		nmm_print("NMM already active!\n");
		return enq_response(ops->rsp_type, MSG_INVALID_STATE);
	}

	nr_req = &req->message_body;

	/* Param validation */
	if (ops->params_valid(nr_req) != 0)
	{
		ret = enq_response(ops->rsp_type, MSG_INVALID_CONFIG);
		goto out_err;
	}

	ret = enq_response(ops->rsp_type, MSG_OK);
	if (ret)
		goto out_err;

	if (state.stop_in_progress)
	{
		nmm_print("STOP detected, aborting current command\n");
		goto out_err;
	}

	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	ops->build_cmd(msg, nr_req);

	ret = send_nmm_command(msg);
	if (ret)
		return -1;

out_err:
	rte_atomic16_clear(&state.is_active);
	return ret;
}
static int process_band_scan_n77()
{
	struct nmm_msg *msg;
	void *nr_req = NULL;
	int ret;
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	n77bs_ops.build_cmd(msg, nr_req);
	ret = send_nmm_command(msg);
	if (ret)
		return -1;

	return 0;
}

static int process_ssb_scan()
{
	struct nmm_msg *msg;
	void *nr_req = NULL;
	int ret;
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	ss_ops.build_cmd(msg, nr_req);
	ret = send_nmm_command(msg);
	if (ret)
		return -1;

	return 0;
}

static int process_cell_follow_stop()
{
	struct nmm_msg *msg;
	void *nr_req = NULL;
	int ret;
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
	{
		nmm_print("Error allocating memory\n");
		return -1;
	}
	cfs_ops.build_cmd(msg, nr_req);
	ret = send_nmm_command(msg);
	if (ret)
		return -1;

	return 0;
}

int nmm_fapi_send_request(fapi_request_t *req)
{
	int err;

	state.crt_cmd = fapi_req_to_hif_cmd_type(req->message_type);
	switch (req->message_type)
	{
	case NMM_STOP_REQUEST:
		/* Stop procedure is treated separately */
		err = process_stop_request();
		break;
	case SSB_SCAN_REQUEST:
		err = process_ssb_scan();
		break;
	case BAND_SCAN_N77_REQUEST:
		err = process_band_scan_n77();
		break;
	case CELL_FOLLOW_REQUEST:
		err = process_cell_follow((fapi_request_nr_t *)req);
		break;
	case CELL_FOLLOW_STOP_REQUEST:
		err = process_cell_follow_stop();
		break;
	case CELL_FOLLOW_MIB_REQUEST:
		err = process_cell_follow_mib((fapi_request_nr_t *)req);
		break;
	case CELL_FOLLOW_SIB_1_REQUEST:
		err = process_cell_follow_sib_1((fapi_request_nr_t *)req);
		break;
	default:
		err = process_fapi_request(req);
		break;
	}

	return err;
}

static void reset_modem(void)
{
	int ret;

	nmm_print("NLM card stuck! Resetting modem\n");

	/* Wait for any in-flight IPC messages to be processed */
	usleep(10000);

	/*
	 * Mark reset procedure as started and wait for various
	 * checkpoints to catch up
	 */
	state.reset_in_progress = 1;
	usleep(10000);

	ret = rte_pmd_la93xx_reset_restore_cfg(dev_id);
	if (ret)
		nmm_print("Modem reset failed!\n");

	nmm_print("Reset done\n");
	state.reset_in_progress = 0;
	state.reset_done = 1;

	/* Re-init shared memory area */
	ret = init_vspa_mem();
	if (ret)
		nmm_print("Failed to initialize shared memory\n");
}

/* Processing of results */
static void process_dbg_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *reply;
	struct dbg_ops *ops;
	uint32_t err_code;
	int ret;

	reply = (struct nmm_msg *)in_buf->mem;
	ops = get_dbg_ops(reply->type);
	if (!ops)
	{
		nmm_print("Received FreeRTOS message of unknown type (%d)\n",
				  reply->type);
		return;
	}

	/* We received a sane response from FreeRTOS, VSPA processing is done */
	state.vspa_active = 0;
	err_code = MSG_PROCEDURE_COMPLETE;

	/* Don't treat timeout status as an error if we issued a reset cmd */
	if (reply->type == NMM_RESET && reply->status == NMM_STATUS_TIMEOUT)
		goto out;

	if (reply->status != NMM_STATUS_OK)
	{
		err_code = MSG_TIMEOUT;
		goto out;
	}

	if (!ops->process_vspa_output)
		goto out;

	if (state.stop_in_progress)
	{
		err_code = MSG_PROCEDURE_STOPPED;
		goto out;
	}

	ret = ops->process_vspa_output(&state.stop_in_progress);
	if (ret)
	{
		err_code = MSG_PROCEDURE_STOPPED;
		goto out;
	}

out:
	enq_response(ops->rsp_type, err_code);

	/* Timeout status indicates VSPA core hang */
	if (reply->status == NMM_STATUS_TIMEOUT)
		reset_modem();

	rte_atomic16_clear(&state.is_active);
}

static void process_mib_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *nmm_info;
	nmm_info = (struct nmm_msg *)in_buf->mem;
	printf("[MIB_INFO:]SFN: %d\n\r", nmm_info->mib_info.systemFrameNumber);
	if (nmm_info->mib_info.subCarrierSpacingCommon == 1)
	{
		printf("[MIB_INFO:]subCarrierSpacingCommon: scs30\n\r");
	}
	else
		printf("[MIB_INFO:]subCarrierSpacingCommon: scs15\n\r");
	printf("[MIB_INFO:]ssb_subcarrierOffset: %d\n\r", nmm_info->mib_info.ssb_subcarrierOffset);
	if (nmm_info->mib_info.dmrs_typeA_Position == 1)
	{
		printf("[MIB_INFO:]dmrs_typeA_Position: pos3\n\r");
	}
	else
		printf("[MIB_INFO:]dmrs_typeA_Position: pos2\n\r");
	printf("[MIB_INFO:]half_frame_bit: %d\n\r", nmm_info->mib_info.half_frame_bit);
	if (nmm_info->mib_info.intraFreqReselection == 1)
	{
		printf("[MIB_INFO:]intraFreqReselection: notAllowed\n\r");
	}
	else
		printf("[MIB_INFO:]intraFreqReselection: allowed\n\r");

	printf("[MIB_INFO:]k_ssb_msb: %d\n\r", nmm_info->mib_info.k_ssb_msb);
	if (nmm_info->mib_info.cellBarred == 1)
	{
		printf("[MIB_INFO:]cellBarred: notBarred\n\r");
	}
	else
		printf("[MIB_INFO:]cellBarred: Barred\n\r");
	printf("[MIB_INFO:]pdcch_configSIB1: %d\n\r", nmm_info->mib_info.pdcch_configSIB1);
	// memcpy(&stats,&nmm_info->mib_info,sizeof(stats));
	stats.systemFrameNumber = nmm_info->mib_info.systemFrameNumber;
	stats.ssb_subcarrierOffset = nmm_info->mib_info.ssb_subcarrierOffset;
	stats.dmrs_typeA_Position = nmm_info->mib_info.dmrs_typeA_Position;
	stats.half_frame_bit = nmm_info->mib_info.half_frame_bit;
	stats.k_ssb_msb = nmm_info->mib_info.k_ssb_msb;
	stats.cellBarred = nmm_info->mib_info.cellBarred;
	stats.subCarrierSpacingCommon = nmm_info->mib_info.subCarrierSpacingCommon;
	stats.pdcch_configSIB1 = nmm_info->mib_info.pdcch_configSIB1;
	stats.intraFreqReselection = nmm_info->mib_info.intraFreqReselection;
}

static void process_get_cell_stats_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *nmm_info;
	nmm_info = (struct nmm_msg *)in_buf->mem;
	/*printf("[SSB_TRACK_UPDATE]: SFN/SF: %d/%d, CellId: %d, RSSI_x10: %d dBm, SNR_x10: %d dB, t_del_min: %d, proc_itr: %d\n\r",
		nmm_info->schedular_params.gnb_sfn,
		nmm_info->schedular_params.gnb_sf,
		nmm_info->cell_acquistion_response.cell_follow_cell_id,
		(int32_t)(10 * nmm_info->cell_acquistion_response.cell_rssi_dBm),
		(int32_t)(10 * nmm_info->cell_acquistion_response.cell_snr_dB),
		//(int32_t)(nmm_info->iir_filter_param.cfo_est * 1000),
		//(int32_t)(ppb_x10),
		//vspa_controller_if.t_delta_minor,
		nmm_info->schedular_params.t_delta_minor,
		nmm_info->schedular_params.process_itr);

	printf("[SSB_TRACK_UPDATE]: n_tx_ssb: %d, pmd_x100000: %d, pfa_x100000: %d, n_tx_mib: %d, n_mib_bler_x100000: %d\n\n\r",
		nmm_info->ssb_stats.n_rx_ssb,
		((nmm_info->ssb_stats.n_missed_detection + nmm_info->ssb_stats.n_false_alarm) * 100000) / nmm_info->ssb_stats.n_rx_ssb,
		(nmm_info->ssb_stats.n_false_alarm * 100000) / nmm_info->ssb_stats.n_rx_ssb,
		nmm_info->ssb_stats.n_rx_mib,
		(nmm_info->ssb_stats.n_mib_crc_fail * 100000) / (nmm_info->ssb_stats.n_rx_mib + 1));*/
	cell_follow_stats.cell_follow_cell_id = nmm_info->cell_acquistion_response.cell_follow_cell_id;
	cell_follow_stats.cell_rssi_dBm = ((int32_t)(10 * nmm_info->cell_acquistion_response.cell_rssi_dBm));
	cell_follow_stats.cell_snr_dB = ((int32_t)(10 * nmm_info->cell_acquistion_response.cell_snr_dB));
	cell_follow_stats.n_rx_ssb = nmm_info->ssb_stats.n_rx_ssb;
	cell_follow_stats.n_missed_detection = ((nmm_info->ssb_stats.n_missed_detection + nmm_info->ssb_stats.n_false_alarm) * 100000) / nmm_info->ssb_stats.n_rx_ssb;
	cell_follow_stats.n_false_alarm = (nmm_info->ssb_stats.n_false_alarm * 100000) / nmm_info->ssb_stats.n_rx_ssb;
	cell_follow_stats.n_rx_mib = nmm_info->ssb_stats.n_rx_mib;
	cell_follow_stats.n_mib_crc_fail = ((nmm_info->ssb_stats.n_mib_crc_fail * 100000) / (nmm_info->ssb_stats.n_rx_mib + 1));
	cell_follow_stats.cell_tracking = nmm_info->cell_acquistion_response.cell_tracking;
	cell_follow_stats.cfo_ppb = ((int32_t)(nmm_info->iir_filter_cfo_param.cfo_est * 1000));
}

static void process_band_stats_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *nmm_info;
	nmm_info = (struct nmm_msg *)in_buf->mem;
	cell_follow_stats.cell_follow_cell_id = nmm_info->cell_acquistion_response.cell_search_cell_id;
	cell_follow_stats.cell_rssi_dBm = (int32_t)(10 * nmm_info->cell_acquistion_response.cell_rssi_dBm);
	cell_follow_stats.cell_snr_dB = (int32_t)(10 * nmm_info->cell_acquistion_response.cell_snr_dB);
}
static void process_stop_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *reply;

	reply = (struct nmm_msg *)in_buf->mem;

	state.vspa_active = 0;
	if (state.crt_cmd != NMM_STOP)
		goto out;

	/* Send response if this was a user-initiated STOP command */
	if (reply->status != NMM_STATUS_OK)
		enq_response(NMM_STOP_RESPONSE, MSG_NMM_STOP_INVALID_STATE);
	else
		enq_response(NMM_STOP_RESPONSE, MSG_NMM_STOP_OK);

	state.stop_in_progress = 0;

out:
	/* Timeout status indicates VSPA core hang */
	if (reply->status == NMM_STATUS_TIMEOUT)
		reset_modem();

	state.crt_cmd = 0;
	rte_atomic16_clear(&state.is_active);
}

static void process_fapi_reply(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *reply;
	fapi_indication_t *fapi_ind;
	struct fapi_ops *ops;
	uint16_t ind_size;
	int ret;

	reply = (struct nmm_msg *)in_buf->mem;
	ops = get_fapi_ops(reply->type);
	if (!ops)
	{
		nmm_print("Received FreeRTOS message of unknown type (%d)\n",
				  reply->type);
		return;
	}

	ind_size = sizeof(fapi_indication_t) + ops->lte_ind_size;
	fapi_ind = (fapi_indication_t *)rte_malloc(NULL, ind_size, 0);
	if (!fapi_ind)
	{
		nmm_print("rte_malloc failed\n");
		return;
	}

	fapi_ind->message_type = ops->ind_type;
	fapi_ind->error_code = MSG_PROCEDURE_COMPLETE;

	if (reply->status == NMM_STATUS_LOST_SYNC)
	{
		nmm_print("Synchronization can't be established or is lost.\n");
		fapi_ind->error_code = MSG_PROCEDURE_STOPPED;
		goto out;
	}

	if (reply->status == NMM_STATUS_ERR || reply->status == NMM_STATUS_TIMEOUT)
	{
		fapi_ind->error_code = MSG_TIMEOUT;
		goto out;
	}

	if (!ops->process_vspa_output)
		goto out;

	if (state.stop_in_progress)
	{
		fapi_ind->error_code = MSG_PROCEDURE_STOPPED;
		goto out;
	}

	ret = ops->process_vspa_output(&fapi_ind->message_body,
								   &state.stop_in_progress);
	if (-EAGAIN == ret)
	{
		if (state.crt_decode.partial_ind)
		{
			fapi_ind->error_code = MSG_PARTIAL_RESULTS;
			state.crt_decode.partial_ind = false;
			goto out_active;
		}
		/* We asked VSPA to do more stuff, no indication yet */
		rte_free(fapi_ind);
		return;
	}
	/* Processing successful, but VSPA still active */
	if (-EINPROGRESS == ret)
		goto out_active;
	/* Error in processing, send error indication to user */
	if (ret)
		fapi_ind->error_code = MSG_PROCEDURE_STOPPED;

out:
	state.vspa_active = 0;
out_active:
	rte_ring_enqueue(user_cb_ring, fapi_ind);

	/* Timeout status indicates VSPA core hang */
	if (reply->status == NMM_STATUS_TIMEOUT)
	{
		reset_modem();
		state.vspa_active = 0;
	}

	state.crt_cmd = 0;
	if (!state.vspa_active)
		rte_atomic16_clear(&state.is_active);
}

static void process_rx(struct rte_bbdev_op_data *in_buf)
{
	struct nmm_msg *msg;

	nmm_print_dbg("Received message address from controller to host: buf = %x, len = %d\n",
				  in_buf->mem, in_buf->length);
	hexdump(in_buf->mem, in_buf->length);

	/*
	 * Ignore empty messages. These may happen on modem reset if
	 * new producer/consumer indexes don't match the old ones
	 */
	if (!in_buf->length)
		return;

	/* If truncated message, show a warning then exit */
	if (in_buf->length != NMM_MSG_SIZE)
	{
		nmm_print("Received truncated message (%uB, expect %luB)\n",
				  in_buf->length, NMM_MSG_SIZE);
		return;
	}

	/* NLM card is alive */
	disarm_cmd_timer();

	msg = (struct nmm_msg *)in_buf->mem;

	switch (msg->type)
	{
	/* This can only be an error from VSPA; fallthrough to FAPI reply */
	case NMM_TIME_UPDATE:
	/* Same as above; successful dumps come with DO_MIB/DO_SIB/CF opcodes */
	case NMM_DL_DUMP:
		msg->type = state.crt_cmd;
	case NMM_CELL_SEARCH:
	case NMM_DO_MIB:
	case NMM_DO_SIB:
		process_fapi_reply(in_buf);
		break;
	case NMM_CELL_FOLLOW:
		break;
	case NMM_CELL_FOLLOW_MIB:
		break;
	case NMM_CELL_FOLLOW_MIB_INFO:
		process_mib_reply(in_buf);
		break;
	case NMM_GET_CELL_STATS_INFO:
		process_get_cell_stats_reply(in_buf);
		break;
	case NMM_GET_BAND_STATS:
		process_band_stats_reply(in_buf);
		break;
	case NMM_CELL_FOLLOW_STOP:
		break;
	case NMM_FILL_MEM_CUSTOM:
		break;
	case NMM_RX_CAPTURE:
	case NMM_DEC_CAPTURE:
	case NMM_INT_TEST_ENABLE:
	case NMM_INT_TEST_DISABLE:
	case NMM_SSB_SCAN:
		break;
	case NMM_BAND_SCAN_N77:
		break;
	case NMM_RESET:
		process_dbg_reply(in_buf);
		break;
	case NMM_STOP:
		process_stop_reply(in_buf);
		break;
	default:
		nmm_print_dbg("Received unknown message type (%d)\n",
					  msg->type);
		/* If unexpected message, rearm timer */
		arm_cmd_timer();
		break;
	}
}

static int nmm_poll_loop(__attribute__((unused)) void *dummy)
{
	struct rte_bbdev_raw_op *raw_ops_deq = NULL;
	int qid = BBDEV_IPC_M2H_QUEUE;
	int ret;

	while (do_poll)
	{
		do
		{
			if (state.reset_in_progress)
				continue;
			if (!do_poll)
				return 0;
			raw_ops_deq = rte_bbdev_dequeue_raw_op(dev_id, qid);
		} while (!raw_ops_deq);

		process_rx(&raw_ops_deq->input);

		/* Don't consume stale dequeue op if NLM card was reset */
		if (state.reset_done)
		{
			state.reset_done = 0;
			continue;
		}
		ret = rte_bbdev_consume_raw_op(dev_id, qid, raw_ops_deq);
		if (ret < 0)
			nmm_print("rte_bbdev_consume_raw_op failed (%d)\n", ret);
	}

	return 0;
}
int16_t inSym[4416] = {30,
					   22,
					   58,
					   -10,
					   56,
					   -10,
					   15,
					   -7,
					   -30,
					   -31,
					   -41,
					   -59,
					   -28,
					   -56,
					   -22,
					   -24,
					   -22,
					   6,
					   -4,
					   31,
					   30,
					   71,
					   34,
					   110,
					   -11,
					   92,
					   -58,
					   -6,
					   -42,
					   -116,
					   33,
					   -133,
					   93,
					   -47,
					   80,
					   45,
					   18,
					   53,
					   -30,
					   0,
					   -39,
					   -20,
					   -40,
					   26,
					   -58,
					   71,
					   -72,
					   37,
					   -44,
					   -56,
					   26,
					   -110,
					   93,
					   -67,
					   110,
					   29,
					   65,
					   89,
					   -13,
					   75,
					   -81,
					   28,
					   -101,
					   2,
					   -62,
					   2,
					   12,
					   -8,
					   73,
					   -45,
					   82,
					   -82,
					   44,
					   -76,
					   -2,
					   -21,
					   -23,
					   40,
					   -24,
					   67,
					   -32,
					   56,
					   -57,
					   36,
					   -70,
					   24,
					   -37,
					   10,
					   35,
					   -21,
					   95,
					   -56,
					   95,
					   -65,
					   41,
					   -33,
					   -18,
					   19,
					   -40,
					   53,
					   -31,
					   49,
					   -23,
					   17,
					   -26,
					   -14,
					   -22,
					   -23,
					   2,
					   -13,
					   26,
					   -4,
					   25,
					   -16,
					   1,
					   -41,
					   -17,
					   -48,
					   -11,
					   -9,
					   5,
					   60,
					   12,
					   109,
					   8,
					   97,
					   7,
					   27,
					   7,
					   -58,
					   -4,
					   -114,
					   -19,
					   -112,
					   -18,
					   -50,
					   4,
					   43,
					   27,
					   104,
					   25,
					   83,
					   3,
					   -6,
					   -13,
					   -79,
					   -13,
					   -66,
					   -11,
					   14,
					   -17,
					   71,
					   -22,
					   48,
					   -16,
					   -14,
					   -6,
					   -40,
					   2,
					   -11,
					   20,
					   25,
					   57,
					   22,
					   82,
					   -8,
					   48,
					   -26,
					   -39,
					   -26,
					   -101,
					   -24,
					   -70,
					   -18,
					   19,
					   8,
					   62,
					   45,
					   9,
					   54,
					   -73,
					   15,
					   -84,
					   -39,
					   -5,
					   -54,
					   86,
					   -20,
					   111,
					   25,
					   68,
					   44,
					   6,
					   41,
					   -40,
					   24,
					   -66,
					   -11,
					   -69,
					   -59,
					   -42,
					   -79,
					   2,
					   -34,
					   33,
					   47,
					   38,
					   84,
					   31,
					   34,
					   28,
					   -48,
					   17,
					   -76,
					   -16,
					   -27,
					   -53,
					   38,
					   -56,
					   61,
					   -17,
					   46,
					   25,
					   30,
					   37,
					   21,
					   28,
					   -7,
					   28,
					   -60,
					   34,
					   -99,
					   12,
					   -82,
					   -39,
					   -14,
					   -72,
					   55,
					   -44,
					   82,
					   18,
					   61,
					   51,
					   19,
					   23,
					   -8,
					   -29,
					   -3,
					   -47,
					   17,
					   -15,
					   12,
					   35,
					   -36,
					   66,
					   -91,
					   59,
					   -89,
					   21,
					   -15,
					   -31,
					   76,
					   -71,
					   103,
					   -72,
					   53,
					   -35,
					   -15,
					   13,
					   -38,
					   41,
					   -15,
					   41,
					   11,
					   28,
					   8,
					   18,
					   -17,
					   11,
					   -39,
					   -0,
					   -42,
					   -22,
					   -25,
					   -46,
					   11,
					   -48,
					   55,
					   -12,
					   78,
					   46,
					   58,
					   76,
					   2,
					   38,
					   -58,
					   -40,
					   -83,
					   -80,
					   -59,
					   -38,
					   -4,
					   36,
					   44,
					   54,
					   48,
					   3,
					   11,
					   -40,
					   -23,
					   -10,
					   -5,
					   54,
					   51,
					   58,
					   75,
					   -14,
					   19,
					   -74,
					   -77,
					   -43,
					   -119,
					   37,
					   -71,
					   57,
					   9,
					   -16,
					   46,
					   -95,
					   40,
					   -78,
					   43,
					   30,
					   67,
					   122,
					   65,
					   114,
					   6,
					   21,
					   -61,
					   -68,
					   -71,
					   -84,
					   -31,
					   -31,
					   -8,
					   30,
					   -35,
					   37,
					   -61,
					   -13,
					   -23,
					   -61,
					   62,
					   -49,
					   116,
					   19,
					   95,
					   74,
					   26,
					   60,
					   -35,
					   1,
					   -67,
					   -35,
					   -73,
					   -21,
					   -54,
					   4,
					   -5,
					   0,
					   51,
					   -16,
					   69,
					   -7,
					   33,
					   21,
					   -28,
					   26,
					   -65,
					   -5,
					   -53,
					   -29,
					   -4,
					   -15,
					   49,
					   11,
					   70,
					   6,
					   45,
					   -22,
					   -7,
					   -27,
					   -39,
					   9,
					   -33,
					   43,
					   -13,
					   37,
					   -16,
					   6,
					   -32,
					   -10,
					   -13,
					   -11,
					   47,
					   -24,
					   84,
					   -43,
					   46,
					   -28,
					   -31,
					   35,
					   -63,
					   89,
					   -28,
					   70,
					   3,
					   -15,
					   -27,
					   -91,
					   -70,
					   -102,
					   -35,
					   -59,
					   73,
					   -3,
					   142,
					   45,
					   94,
					   83,
					   -17,
					   96,
					   -75,
					   68,
					   -46,
					   11,
					   -6,
					   -39,
					   -16,
					   -63,
					   -52,
					   -71,
					   -49,
					   -70,
					   -2,
					   -45,
					   35,
					   7,
					   30,
					   56,
					   11,
					   63,
					   21,
					   38,
					   54,
					   19,
					   68,
					   18,
					   35,
					   6,
					   -31,
					   -36,
					   -91,
					   -71,
					   -106,
					   -49,
					   -63,
					   17,
					   16,
					   65,
					   74,
					   57,
					   69,
					   17,
					   15,
					   -14,
					   -26,
					   -34,
					   -17,
					   -56,
					   16,
					   -67,
					   28,
					   -32,
					   9,
					   43,
					   -10,
					   98,
					   -13,
					   83,
					   -21,
					   20,
					   -41,
					   -32,
					   -43,
					   -49,
					   2,
					   -53,
					   67,
					   -56,
					   85,
					   -34,
					   33,
					   27,
					   -42,
					   87,
					   -71,
					   86,
					   -42,
					   17,
					   1,
					   -57,
					   24,
					   -73,
					   25,
					   -31,
					   15,
					   15,
					   -4,
					   23,
					   -29,
					   3,
					   -32,
					   -10,
					   7,
					   -3,
					   61,
					   13,
					   71,
					   22,
					   14,
					   20,
					   -62,
					   9,
					   -82,
					   -9,
					   -33,
					   -27,
					   29,
					   -30,
					   42,
					   -9,
					   11,
					   23,
					   -11,
					   36,
					   2,
					   11,
					   18,
					   -32,
					   -4,
					   -51,
					   -46,
					   -22,
					   -47,
					   30,
					   17,
					   56,
					   89,
					   30,
					   90,
					   -21,
					   8,
					   -51,
					   -80,
					   -34,
					   -96,
					   16,
					   -39,
					   59,
					   25,
					   65,
					   47,
					   26,
					   32,
					   -35,
					   13,
					   -74,
					   -0,
					   -66,
					   -19,
					   -23,
					   -39,
					   15,
					   -36,
					   23,
					   1,
					   20,
					   48,
					   29,
					   66,
					   40,
					   41,
					   24,
					   -13,
					   -18,
					   -57,
					   -40,
					   -58,
					   -15,
					   -7,
					   22,
					   53,
					   22,
					   56,
					   -13,
					   -16,
					   -27,
					   -97,
					   -2,
					   -92,
					   14,
					   5,
					   -20,
					   96,
					   -62,
					   92,
					   -37,
					   18,
					   51,
					   -23,
					   105,
					   7,
					   56,
					   35,
					   -40,
					   -9,
					   -74,
					   -82,
					   -22,
					   -84,
					   36,
					   -1,
					   35,
					   69,
					   3,
					   48,
					   -4,
					   -27,
					   5,
					   -56,
					   -21,
					   -15,
					   -76,
					   32,
					   -81,
					   38,
					   5,
					   29,
					   108,
					   42,
					   115,
					   44,
					   19,
					   -13,
					   -75,
					   -102,
					   -72,
					   -122,
					   6,
					   -29,
					   60,
					   94,
					   36,
					   125,
					   -25,
					   42,
					   -53,
					   -55,
					   -25,
					   -71,
					   21,
					   -19,
					   34,
					   14,
					   3,
					   -13,
					   -41,
					   -52,
					   -53,
					   -30,
					   -12,
					   57,
					   52,
					   131,
					   84,
					   115,
					   51,
					   11,
					   -21,
					   -102,
					   -69,
					   -141,
					   -56,
					   -91,
					   -0,
					   -7,
					   47,
					   50,
					   49,
					   62,
					   9,
					   50,
					   -38,
					   33,
					   -57,
					   9,
					   -38,
					   -23,
					   3,
					   -42,
					   34,
					   -31,
					   38,
					   4,
					   21,
					   32,
					   2,
					   31,
					   -6,
					   10,
					   -6,
					   -8,
					   -3,
					   -15,
					   1,
					   -15,
					   7,
					   -18,
					   11,
					   -22,
					   8,
					   -19,
					   -6,
					   -0,
					   -30,
					   28,
					   -53,
					   42,
					   -53,
					   22,
					   -13,
					   -16,
					   50,
					   -28,
					   88,
					   6,
					   68,
					   49,
					   13,
					   36,
					   -20,
					   -34,
					   -17,
					   -87,
					   -22,
					   -54,
					   -63,
					   37,
					   -92,
					   87,
					   -38,
					   38,
					   79,
					   -55,
					   142,
					   -98,
					   74,
					   -51,
					   -59,
					   43,
					   -118,
					   114,
					   -59,
					   115,
					   31,
					   41,
					   56,
					   -69,
					   22,
					   -136,
					   -1,
					   -94,
					   7,
					   21,
					   7,
					   86,
					   -17,
					   27,
					   -27,
					   -75,
					   3,
					   -69,
					   31,
					   67,
					   10,
					   180,
					   -41,
					   121,
					   -53,
					   -63,
					   -5,
					   -184,
					   46,
					   -135,
					   49,
					   -3,
					   21,
					   65,
					   6,
					   43,
					   6,
					   14,
					   -13,
					   31,
					   -49,
					   55,
					   -60,
					   32,
					   -27,
					   -21,
					   15,
					   -45,
					   29,
					   -29,
					   30,
					   -8,
					   45,
					   -6,
					   63,
					   -8,
					   43,
					   -2,
					   -15,
					   0,
					   -61,
					   -11,
					   -63,
					   -16,
					   -50,
					   1,
					   -54,
					   26,
					   -53,
					   33,
					   -4,
					   28,
					   79,
					   33,
					   122,
					   39,
					   83,
					   10,
					   10,
					   -57,
					   -24,
					   -102,
					   -8,
					   -71,
					   5,
					   8,
					   -23,
					   55,
					   -69,
					   32,
					   -92,
					   -16,
					   -78,
					   -25,
					   -38,
					   16,
					   19,
					   59,
					   81,
					   64,
					   121,
					   29,
					   113,
					   -20,
					   61,
					   -52,
					   -2,
					   -46,
					   -57,
					   -12,
					   -98,
					   12,
					   -111,
					   -5,
					   -73,
					   -44,
					   8,
					   -49,
					   70,
					   3,
					   63,
					   64,
					   11,
					   72,
					   -15,
					   28,
					   15,
					   -8,
					   52,
					   -4,
					   36,
					   6,
					   -23,
					   -14,
					   -66,
					   -44,
					   -58,
					   -37,
					   -22,
					   3,
					   11,
					   22,
					   35,
					   -4,
					   54,
					   -27,
					   48,
					   0,
					   8,
					   51,
					   -40,
					   57,
					   -49,
					   3,
					   -15,
					   -49,
					   14,
					   -48,
					   4,
					   -10,
					   -23,
					   11,
					   -24,
					   5,
					   6,
					   4,
					   35,
					   25,
					   39,
					   38,
					   20,
					   18,
					   -4,
					   -17,
					   -27,
					   -31,
					   -35,
					   -21,
					   -9,
					   -7,
					   40,
					   -3,
					   62,
					   3,
					   14,
					   13,
					   -67,
					   10,
					   -93,
					   -14,
					   -30,
					   -31,
					   56,
					   -15,
					   69,
					   24,
					   6,
					   43,
					   -50,
					   22,
					   -35,
					   -8,
					   23,
					   -10,
					   50,
					   15,
					   27,
					   30,
					   2,
					   15,
					   7,
					   -21,
					   18,
					   -54,
					   -2,
					   -68,
					   -45,
					   -57,
					   -64,
					   -18,
					   -40,
					   37,
					   -2,
					   76,
					   19,
					   69,
					   25,
					   28,
					   35,
					   -2,
					   39,
					   3,
					   12,
					   17,
					   -32,
					   1,
					   -45,
					   -45,
					   2,
					   -76,
					   64,
					   -58,
					   66,
					   -8,
					   -4,
					   29,
					   -75,
					   38,
					   -72,
					   36,
					   -5,
					   37,
					   45,
					   20,
					   26,
					   -27,
					   -31,
					   -74,
					   -54,
					   -65,
					   -20,
					   10,
					   30,
					   93,
					   45,
					   111,
					   25,
					   53,
					   8,
					   -29,
					   18,
					   -76,
					   40,
					   -77,
					   43,
					   -52,
					   12,
					   -24,
					   -39,
					   -0,
					   -83,
					   14,
					   -94,
					   17,
					   -67,
					   21,
					   -17,
					   38,
					   30,
					   57,
					   57,
					   48,
					   64,
					   2,
					   60,
					   -44,
					   51,
					   -45,
					   35,
					   -6,
					   15,
					   20,
					   -12,
					   4,
					   -49,
					   -30,
					   -91,
					   -40,
					   -108,
					   -27,
					   -66,
					   -22,
					   26,
					   -27,
					   103,
					   -5,
					   99,
					   61,
					   25,
					   121,
					   -42,
					   107,
					   -40,
					   20,
					   7,
					   -65,
					   31,
					   -84,
					   12,
					   -55,
					   -9,
					   -37,
					   -3,
					   -46,
					   3,
					   -38,
					   -28,
					   19,
					   -66,
					   87,
					   -46,
					   95,
					   40,
					   27,
					   108,
					   -50,
					   75,
					   -61,
					   -34,
					   -4,
					   -108,
					   54,
					   -77,
					   49,
					   12,
					   -12,
					   63,
					   -62,
					   47,
					   -56,
					   18,
					   -11,
					   23,
					   20,
					   36,
					   12,
					   -0,
					   -4,
					   -79,
					   7,
					   -120,
					   41,
					   -61,
					   52,
					   54,
					   17,
					   116,
					   -33,
					   70,
					   -49,
					   -21,
					   -21,
					   -51,
					   12,
					   4,
					   11,
					   61,
					   -18,
					   37,
					   -37,
					   -54,
					   -21,
					   -111,
					   18,
					   -69,
					   51,
					   26,
					   56,
					   82,
					   31,
					   57,
					   -15,
					   -4,
					   -53,
					   -38,
					   -54,
					   -25,
					   -12,
					   10,
					   41,
					   43,
					   58,
					   57,
					   22,
					   37,
					   -38,
					   -22,
					   -72,
					   -87,
					   -53,
					   -98,
					   2,
					   -35,
					   49,
					   51,
					   56,
					   80,
					   26,
					   32,
					   -11,
					   -33,
					   -21,
					   -45,
					   1,
					   -2,
					   28,
					   41,
					   27,
					   47,
					   -5,
					   26,
					   -41,
					   11,
					   -51,
					   4,
					   -31,
					   -17,
					   0,
					   -57,
					   23,
					   -80,
					   28,
					   -53,
					   16,
					   14,
					   -4,
					   63,
					   -15,
					   55,
					   -4,
					   10,
					   25,
					   -14,
					   42,
					   7,
					   28,
					   36,
					   -10,
					   27,
					   -38,
					   -19,
					   -35,
					   -51,
					   -9,
					   -34,
					   10,
					   5,
					   2,
					   17,
					   -25,
					   -5,
					   -40,
					   -19,
					   -18,
					   4,
					   36,
					   38,
					   79,
					   35,
					   67,
					   -5,
					   1,
					   -39,
					   -63,
					   -37,
					   -70,
					   -15,
					   -18,
					   -5,
					   38,
					   -3,
					   50,
					   18,
					   22,
					   59,
					   -7,
					   82,
					   -18,
					   53,
					   -22,
					   -20,
					   -29,
					   -91,
					   -31,
					   -111,
					   -15,
					   -67,
					   8,
					   14,
					   23,
					   73,
					   28,
					   67,
					   28,
					   5,
					   26,
					   -48,
					   16,
					   -32,
					   1,
					   37,
					   -8,
					   80,
					   -13,
					   49,
					   -26,
					   -20,
					   -43,
					   -55,
					   -39,
					   -36,
					   -2,
					   -12,
					   37,
					   -23,
					   38,
					   -41,
					   6,
					   -20,
					   -14,
					   41,
					   2,
					   86,
					   21,
					   69,
					   9,
					   -2,
					   -17,
					   -71,
					   -11,
					   -89,
					   17,
					   -43,
					   13,
					   36,
					   -38,
					   89,
					   -66,
					   71,
					   -8,
					   -4,
					   93,
					   -67,
					   117,
					   -59,
					   20,
					   -1,
					   -106,
					   38,
					   -135,
					   26,
					   -53,
					   -2,
					   42,
					   -7,
					   64,
					   -2,
					   31,
					   -23,
					   15,
					   -52,
					   37,
					   -31,
					   51,
					   45,
					   16,
					   97,
					   -50,
					   52,
					   -88,
					   -49,
					   -61,
					   -92,
					   8,
					   -30,
					   61,
					   61,
					   56,
					   79,
					   3,
					   18,
					   -42,
					   -42,
					   -32,
					   -46,
					   17,
					   -22,
					   40,
					   -20,
					   4,
					   -35,
					   -51,
					   -19,
					   -52,
					   37,
					   13,
					   82,
					   75,
					   66,
					   71,
					   2,
					   9,
					   -49,
					   -45,
					   -48,
					   -51,
					   -13,
					   -26,
					   10,
					   -6,
					   3,
					   2,
					   -9,
					   13,
					   4,
					   25,
					   31,
					   19,
					   35,
					   -12,
					   -3,
					   -44,
					   -48,
					   -43,
					   -52,
					   -2,
					   -10,
					   50,
					   28,
					   76,
					   19,
					   55,
					   -14,
					   0,
					   -14,
					   -53,
					   32,
					   -71,
					   74,
					   -49,
					   54,
					   -6,
					   -17,
					   30,
					   -74,
					   46,
					   -73,
					   40,
					   -28,
					   12,
					   17,
					   -27,
					   35,
					   -56,
					   34,
					   -51,
					   22,
					   -13,
					   1,
					   31,
					   -26,
					   54,
					   -44,
					   49,
					   -39,
					   27,
					   -7,
					   2,
					   38,
					   -18,
					   71,
					   -22,
					   67,
					   -10,
					   20,
					   -3,
					   -38,
					   -22,
					   -61,
					   -54,
					   -33,
					   -55,
					   6,
					   -5,
					   12,
					   54,
					   -17,
					   65,
					   -38,
					   28,
					   -25,
					   -3,
					   6,
					   10,
					   23,
					   34,
					   26,
					   21,
					   34,
					   -25,
					   48,
					   -53,
					   48,
					   -37,
					   20,
					   -7,
					   -20,
					   -5,
					   -54,
					   -22,
					   -71,
					   -21,
					   -64,
					   10,
					   -26,
					   43,
					   29,
					   49,
					   56,
					   27,
					   26,
					   -4,
					   -24,
					   -27,
					   -25,
					   -27,
					   35,
					   -1,
					   87,
					   33,
					   59,
					   36,
					   -28,
					   -6,
					   -85,
					   -57,
					   -64,
					   -61,
					   -4,
					   -17,
					   28,
					   24,
					   20,
					   28,
					   -3,
					   17,
					   -21,
					   21,
					   -31,
					   29,
					   -16,
					   10,
					   36,
					   -26,
					   92,
					   -33,
					   82,
					   0,
					   -10,
					   23,
					   -103,
					   -2,
					   -106,
					   -35,
					   -25,
					   -18,
					   48,
					   40,
					   51,
					   59,
					   15,
					   5,
					   1,
					   -62,
					   17,
					   -67,
					   24,
					   -13,
					   8,
					   33,
					   -9,
					   40,
					   -21,
					   35,
					   -44,
					   38,
					   -68,
					   20,
					   -44,
					   -34,
					   41,
					   -80,
					   115,
					   -60,
					   89,
					   10,
					   -22,
					   53,
					   -97,
					   36,
					   -56,
					   8,
					   44,
					   14,
					   79,
					   27,
					   15,
					   -7,
					   -67,
					   -65,
					   -77,
					   -66,
					   -20,
					   12,
					   32,
					   80,
					   42,
					   45,
					   29,
					   -58,
					   18,
					   -99,
					   8,
					   -15,
					   -13,
					   107,
					   -32,
					   129,
					   -27,
					   27,
					   1,
					   -87,
					   25,
					   -101,
					   27,
					   -23,
					   10,
					   40,
					   -10,
					   15,
					   -22,
					   -54,
					   -24,
					   -69,
					   -14,
					   10,
					   4,
					   112,
					   19,
					   133,
					   19,
					   48,
					   2,
					   -65,
					   -14,
					   -111,
					   -11,
					   -69,
					   15,
					   2,
					   41,
					   40,
					   34,
					   32,
					   -13,
					   2,
					   -59,
					   -28,
					   -54,
					   -39,
					   -2,
					   -15,
					   35,
					   38,
					   9,
					   74,
					   -42,
					   47,
					   -37,
					   -21,
					   43,
					   -50,
					   109,
					   -1,
					   75,
					   56,
					   -30,
					   26,
					   -94,
					   -79,
					   -61,
					   -135,
					   7,
					   -61,
					   27,
					   73,
					   3,
					   124,
					   1,
					   58,
					   27,
					   -25,
					   16,
					   -30,
					   -54,
					   16,
					   -96,
					   23,
					   -25,
					   -23,
					   107,
					   -46,
					   151,
					   -7,
					   48,
					   37,
					   -87,
					   18,
					   -103,
					   -44,
					   -7,
					   -61,
					   62,
					   -7,
					   13,
					   51,
					   -79,
					   46,
					   -84,
					   0,
					   11,
					   -14,
					   95,
					   17,
					   76,
					   34,
					   -8,
					   -7,
					   -55,
					   -66,
					   -25,
					   -68,
					   23,
					   4,
					   27,
					   80,
					   -6,
					   86,
					   -24,
					   18,
					   -3,
					   -59,
					   24,
					   -80,
					   19,
					   -38,
					   -16,
					   23,
					   -41,
					   49,
					   -27,
					   26,
					   12,
					   -19,
					   36,
					   -41,
					   24,
					   -20,
					   -5,
					   21,
					   -17,
					   44,
					   -4,
					   36,
					   8,
					   15,
					   -9,
					   1,
					   -39,
					   -9,
					   -38,
					   -28,
					   17,
					   -48,
					   87,
					   -38,
					   105,
					   8,
					   46,
					   53,
					   -47,
					   47,
					   -105,
					   -15,
					   -98,
					   -77,
					   -50,
					   -73,
					   -2,
					   3,
					   27,
					   88,
					   43,
					   108,
					   49,
					   49,
					   43,
					   -32,
					   27,
					   -62,
					   9,
					   -26,
					   -3,
					   23,
					   -9,
					   21,
					   -13,
					   -34,
					   -16,
					   -81,
					   -18,
					   -62,
					   -21,
					   8,
					   -25,
					   59,
					   -22,
					   45,
					   -5,
					   3,
					   14,
					   0,
					   15,
					   47,
					   -1,
					   78,
					   -7,
					   37,
					   21,
					   -49,
					   57,
					   -98,
					   55,
					   -71,
					   3,
					   -12,
					   -46,
					   20,
					   -40,
					   21,
					   7,
					   27,
					   31,
					   39,
					   -4,
					   21,
					   -60,
					   -39,
					   -78,
					   -84,
					   -43,
					   -55,
					   10,
					   31,
					   51,
					   92,
					   74,
					   82,
					   78,
					   35,
					   44,
					   8,
					   -20,
					   -1,
					   -68,
					   -31,
					   -54,
					   -82,
					   4,
					   -99,
					   41,
					   -50,
					   22,
					   23,
					   -21,
					   55,
					   -37,
					   36,
					   -24,
					   13,
					   -17,
					   17,
					   -20,
					   29,
					   -6,
					   19,
					   35,
					   -5,
					   63,
					   -13,
					   40,
					   2,
					   -12,
					   16,
					   -38,
					   9,
					   -16,
					   -19,
					   20,
					   -49,
					   35,
					   -63,
					   25,
					   -45,
					   -2,
					   8,
					   -42,
					   62,
					   -83,
					   70,
					   -80,
					   23,
					   -6,
					   -23,
					   88,
					   -13,
					   108,
					   30,
					   30,
					   32,
					   -62,
					   -26,
					   -70,
					   -74,
					   1,
					   -42,
					   60,
					   38,
					   46,
					   68,
					   -4,
					   14,
					   -24,
					   -48,
					   -8,
					   -40,
					   -4,
					   16,
					   -28,
					   34,
					   -47,
					   -11,
					   -28,
					   -51,
					   14,
					   -23,
					   40,
					   45,
					   37,
					   74,
					   23,
					   35,
					   7,
					   -26,
					   -9,
					   -53,
					   -17,
					   -31,
					   -6,
					   11,
					   11,
					   37,
					   9,
					   25,
					   -16,
					   -20,
					   -35,
					   -64,
					   -23,
					   -57,
					   6,
					   8,
					   22,
					   74,
					   21,
					   75,
					   21,
					   17,
					   23,
					   -35,
					   4,
					   -33,
					   -33,
					   -5,
					   -48,
					   -4,
					   -15,
					   -28,
					   32,
					   -31,
					   34,
					   9,
					   -9,
					   50,
					   -35,
					   48,
					   -8,
					   6,
					   35,
					   -35,
					   37,
					   -45,
					   -0,
					   -25,
					   -25,
					   13,
					   -16,
					   49,
					   -3,
					   54,
					   -12,
					   9,
					   -20,
					   -54,
					   7,
					   -72,
					   47,
					   -21,
					   47,
					   42,
					   -2,
					   44,
					   -50,
					   -10,
					   -53,
					   -43,
					   -26,
					   -11,
					   -4,
					   45,
					   8,
					   57,
					   30,
					   22,
					   58,
					   -10,
					   56,
					   -10,
					   15,
					   -7,
					   -30,
					   -31,
					   -41,
					   -59,
					   -28,
					   -56,
					   -22,
					   -24,
					   -22,
					   6,
					   -4,
					   31,
					   30,
					   71,
					   34,
					   110,
					   -11,
					   92,
					   -58,
					   -6,
					   -42,
					   -116,
					   33,
					   -133,
					   93,
					   -47,
					   80,
					   45,
					   18,
					   53,
					   -30,
					   0,
					   -39,
					   -20,
					   -40,
					   26,
					   -58,
					   71,
					   -72,
					   37,
					   -44,
					   -56,
					   26,
					   -110,
					   93,
					   -67,
					   110,
					   29,
					   65,
					   89,
					   -13,
					   75,
					   -81,
					   28,
					   -101,
					   2,
					   -62,
					   2,
					   12,
					   -8,
					   73,
					   -45,
					   82,
					   -82,
					   44,
					   -76,
					   -2,
					   -21,
					   -23,
					   40,
					   -24,
					   67,
					   -32,
					   56,
					   -57,
					   36,
					   -70,
					   24,
					   -37,
					   10,
					   35,
					   -21,
					   95,
					   -56,
					   95,
					   -65,
					   41,
					   -33,
					   -18,
					   19,
					   -40,
					   53,
					   -31,
					   49,
					   -23,
					   17,
					   -26,
					   -14,
					   -22,
					   -23,
					   2,
					   -13,
					   26,
					   -4,
					   25,
					   -16,
					   1,
					   -41,
					   -17,
					   -48,
					   -11,
					   -9,
					   5,
					   60,
					   12,
					   109,
					   8,
					   97,
					   7,
					   27,
					   7,
					   -58,
					   -4,
					   -114,
					   -19,
					   -112,
					   -18,
					   -50,
					   4,
					   43,
					   27,
					   104,
					   25,
					   83,
					   3,
					   -6,
					   -13,
					   -79,
					   -13,
					   -66,
					   -11,
					   14,
					   -17,
					   71,
					   -22,
					   48,
					   -16,
					   -14,
					   -6,
					   -40,
					   2,
					   -11,
					   20,
					   25,
					   57,
					   22,
					   82,
					   -8,
					   48,
					   -26,
					   -39,
					   -26,
					   -101,
					   -24,
					   -70,
					   -18,
					   19,
					   8,
					   -15,
					   -75,
					   -22,
					   -61,
					   -6,
					   13,
					   16,
					   50,
					   24,
					   11,
					   3,
					   -30,
					   -45,
					   -4,
					   -78,
					   58,
					   -44,
					   72,
					   46,
					   22,
					   108,
					   -31,
					   74,
					   -39,
					   -17,
					   -25,
					   -61,
					   -26,
					   -13,
					   -28,
					   48,
					   -1,
					   30,
					   36,
					   -54,
					   27,
					   -98,
					   -31,
					   -44,
					   -66,
					   50,
					   -18,
					   86,
					   72,
					   38,
					   105,
					   -32,
					   41,
					   -60,
					   -49,
					   -40,
					   -77,
					   -1,
					   -33,
					   36,
					   18,
					   62,
					   22,
					   60,
					   -1,
					   20,
					   -0,
					   -32,
					   30,
					   -54,
					   41,
					   -33,
					   -5,
					   -4,
					   -80,
					   1,
					   -110,
					   -13,
					   -49,
					   -14,
					   63,
					   0,
					   131,
					   7,
					   97,
					   -3,
					   -1,
					   -2,
					   -67,
					   35,
					   -48,
					   75,
					   13,
					   59,
					   34,
					   -26,
					   -10,
					   -107,
					   -62,
					   -99,
					   -56,
					   -0,
					   4,
					   98,
					   52,
					   106,
					   41,
					   24,
					   -2,
					   -66,
					   -20,
					   -93,
					   -1,
					   -60,
					   11,
					   -13,
					   -12,
					   22,
					   -38,
					   46,
					   -9,
					   59,
					   69,
					   46,
					   109,
					   7,
					   41,
					   -23,
					   -88,
					   -17,
					   -151,
					   6,
					   -80,
					   -0,
					   53,
					   -39,
					   114,
					   -58,
					   57,
					   -16,
					   -36,
					   47,
					   -63,
					   58,
					   -18,
					   5,
					   25,
					   -45,
					   23,
					   -28,
					   6,
					   32,
					   10,
					   56,
					   23,
					   10,
					   6,
					   -56,
					   -30,
					   -70,
					   -37,
					   -24,
					   -0,
					   37,
					   34,
					   69,
					   21,
					   66,
					   -17,
					   40,
					   -25,
					   -2,
					   1,
					   -50,
					   11,
					   -77,
					   -22,
					   -64,
					   -56,
					   -23,
					   -33,
					   11,
					   37,
					   16,
					   88,
					   4,
					   74,
					   4,
					   19,
					   33,
					   -25,
					   76,
					   -36,
					   93,
					   -31,
					   46,
					   -27,
					   -49,
					   -21,
					   -120,
					   -10,
					   -100,
					   -2,
					   -7,
					   -3,
					   58,
					   -10,
					   27,
					   -13,
					   -52,
					   -3,
					   -63,
					   27,
					   23,
					   68,
					   110,
					   87,
					   92,
					   45,
					   -6,
					   -47,
					   -64,
					   -113,
					   -29,
					   -82,
					   22,
					   21,
					   2,
					   85,
					   -58,
					   38,
					   -50,
					   -64,
					   44,
					   -101,
					   111,
					   -35,
					   51,
					   56,
					   -83,
					   82,
					   -145,
					   49,
					   -72,
					   21,
					   50,
					   19,
					   102,
					   6,
					   68,
					   -39,
					   20,
					   -65,
					   6,
					   -30,
					   3,
					   27,
					   -16,
					   27,
					   -36,
					   -32,
					   -31,
					   -64,
					   -12,
					   -10,
					   -11,
					   71,
					   -25,
					   76,
					   -17,
					   -2,
					   18,
					   -64,
					   38,
					   -34,
					   17,
					   46,
					   -10,
					   77,
					   7,
					   31,
					   53,
					   -29,
					   54,
					   -49,
					   -14,
					   -47,
					   -81,
					   -57,
					   -65,
					   -60,
					   12,
					   -13,
					   55,
					   71,
					   10,
					   113,
					   -61,
					   67,
					   -69,
					   -13,
					   -7,
					   -44,
					   55,
					   -12,
					   70,
					   17,
					   59,
					   -2,
					   55,
					   -39,
					   39,
					   -48,
					   -23,
					   -30,
					   -104,
					   -23,
					   -128,
					   -24,
					   -64,
					   6,
					   27,
					   69,
					   61,
					   109,
					   31,
					   73,
					   1,
					   -8,
					   21,
					   -58,
					   64,
					   -49,
					   70,
					   -29,
					   18,
					   -36,
					   -56,
					   -45,
					   -94,
					   -12,
					   -65,
					   42,
					   13,
					   54,
					   78,
					   9,
					   63,
					   -30,
					   -38,
					   -3,
					   -137,
					   53,
					   -119,
					   54,
					   27,
					   -16,
					   174,
					   -81,
					   179,
					   -59,
					   41,
					   29,
					   -92,
					   88,
					   -97,
					   59,
					   -8,
					   -27,
					   41,
					   -89,
					   -15,
					   -83,
					   -96,
					   -27,
					   -96,
					   32,
					   -7,
					   58,
					   82,
					   49,
					   99,
					   24,
					   60,
					   8,
					   20,
					   6,
					   1,
					   4,
					   -18,
					   -10,
					   -49,
					   -28,
					   -65,
					   -31,
					   -40,
					   -16,
					   13,
					   -7,
					   45,
					   -15,
					   27,
					   -22,
					   -15,
					   1,
					   -29,
					   47,
					   8,
					   74,
					   60,
					   50,
					   67,
					   -10,
					   9,
					   -59,
					   -69,
					   -63,
					   -99,
					   -33,
					   -60,
					   1,
					   8,
					   22,
					   48,
					   31,
					   48,
					   39,
					   35,
					   41,
					   32,
					   22,
					   31,
					   -24,
					   16,
					   -71,
					   -16,
					   -79,
					   -45,
					   -30,
					   -57,
					   41,
					   -48,
					   74,
					   -25,
					   46,
					   1,
					   -4,
					   15,
					   -23,
					   14,
					   -3,
					   17,
					   11,
					   46,
					   -7,
					   89,
					   -33,
					   96,
					   -27,
					   25,
					   12,
					   -94,
					   41,
					   -177,
					   32,
					   -154,
					   -2,
					   -37,
					   -26,
					   97,
					   -33,
					   167,
					   -33,
					   140,
					   -26,
					   40,
					   0,
					   -71,
					   40,
					   -127,
					   63,
					   -97,
					   45,
					   -14,
					   4,
					   53,
					   -23,
					   58,
					   -21,
					   21,
					   -14,
					   -2,
					   -25,
					   11,
					   -44,
					   29,
					   -37,
					   12,
					   8,
					   -37,
					   57,
					   -71,
					   62,
					   -52,
					   15,
					   13,
					   -39,
					   72,
					   -42,
					   75,
					   9,
					   19,
					   56,
					   -53,
					   38,
					   -84,
					   -34,
					   -52,
					   -81,
					   9,
					   -46,
					   54,
					   34,
					   61,
					   73,
					   47,
					   33,
					   29,
					   -31,
					   5,
					   -43,
					   -32,
					   3,
					   -71,
					   40,
					   -82,
					   22,
					   -51,
					   -29,
					   4,
					   -56,
					   55,
					   -38,
					   82,
					   2,
					   73,
					   32,
					   26,
					   46,
					   -37,
					   49,
					   -73,
					   40,
					   -53,
					   14,
					   4,
					   -25,
					   41,
					   -57,
					   32,
					   -66,
					   2,
					   -50,
					   -7,
					   -18,
					   6,
					   15,
					   6,
					   35,
					   -22,
					   41,
					   -49,
					   44,
					   -40,
					   49,
					   2,
					   40,
					   44,
					   -2,
					   56,
					   -60,
					   36,
					   -85,
					   3,
					   -49,
					   -20,
					   18,
					   -18,
					   53,
					   4,
					   30,
					   15,
					   -17,
					   -11,
					   -39,
					   -57,
					   -21,
					   -69,
					   18,
					   -18,
					   51,
					   56,
					   56,
					   85,
					   24,
					   45,
					   -33,
					   -16,
					   -71,
					   -41,
					   -53,
					   -20,
					   7,
					   11,
					   50,
					   18,
					   36,
					   -0,
					   -4,
					   -28,
					   -19,
					   -43,
					   -1,
					   -25,
					   12,
					   26,
					   -2,
					   73,
					   -16,
					   67,
					   -6,
					   4,
					   6,
					   -60,
					   -11,
					   -70,
					   -37,
					   -36,
					   -20,
					   -5,
					   46,
					   3,
					   93,
					   19,
					   61,
					   55,
					   -24,
					   70,
					   -81,
					   26,
					   -69,
					   -43,
					   -24,
					   -60,
					   6,
					   -10,
					   19,
					   34,
					   33,
					   9,
					   40,
					   -51,
					   17,
					   -57,
					   -24,
					   8,
					   -38,
					   67,
					   -7,
					   48,
					   29,
					   -22,
					   22,
					   -55,
					   -20,
					   -16,
					   -44,
					   43,
					   -14,
					   55,
					   43,
					   17,
					   69,
					   -23,
					   38,
					   -34,
					   -23,
					   -25,
					   -65,
					   -17,
					   -64,
					   -9,
					   -31,
					   11,
					   0,
					   38,
					   14,
					   42,
					   20,
					   6,
					   38,
					   -50,
					   56,
					   -73,
					   46,
					   -36,
					   2,
					   34,
					   -43,
					   75,
					   -45,
					   55,
					   -10,
					   5,
					   12,
					   -23,
					   -6,
					   -18,
					   -37,
					   -7,
					   -36,
					   -9,
					   -1,
					   -7,
					   32,
					   10,
					   39,
					   11,
					   30,
					   -34,
					   21,
					   -88,
					   8,
					   -68,
					   -18,
					   42,
					   -42,
					   148,
					   -38,
					   138,
					   -5,
					   14,
					   30,
					   -103,
					   41,
					   -108,
					   26,
					   -27,
					   -0,
					   37,
					   -27,
					   26,
					   -40,
					   -22,
					   -28,
					   -37,
					   1,
					   2,
					   19,
					   57,
					   8,
					   77,
					   -9,
					   39,
					   1,
					   -38,
					   30,
					   -99,
					   37,
					   -98,
					   5,
					   -34,
					   -28,
					   43,
					   -20,
					   80,
					   12,
					   73,
					   18,
					   52,
					   -8,
					   29,
					   -24,
					   -8,
					   -5,
					   -53,
					   14,
					   -70,
					   -7,
					   -41,
					   -43,
					   -6,
					   -40,
					   -12,
					   9,
					   -37,
					   49,
					   -20,
					   41,
					   51,
					   14,
					   102,
					   10,
					   69,
					   23,
					   -13,
					   15,
					   -55,
					   -18,
					   -27,
					   -38,
					   11,
					   -27,
					   6,
					   -14,
					   -22,
					   -22,
					   -22,
					   -29,
					   5,
					   -8,
					   14,
					   22,
					   -6,
					   25,
					   -22,
					   7,
					   -9,
					   13,
					   7,
					   51,
					   2,
					   66,
					   -5,
					   15,
					   14,
					   -60,
					   46,
					   -81,
					   45,
					   -32,
					   1,
					   21,
					   -42,
					   21,
					   -46,
					   -12,
					   -20,
					   -23,
					   8,
					   1,
					   25,
					   30,
					   33,
					   34,
					   17,
					   17,
					   -28,
					   -7,
					   -64,
					   -26,
					   -46,
					   -27,
					   15,
					   -3,
					   52,
					   22,
					   29,
					   17,
					   -8,
					   -12,
					   5,
					   -14,
					   48,
					   32,
					   50,
					   69,
					   -10,
					   25,
					   -64,
					   -75,
					   -55,
					   -123,
					   -9,
					   -61,
					   7,
					   40,
					   -14,
					   75,
					   -20,
					   39,
					   13,
					   10,
					   44,
					   25,
					   36,
					   34,
					   8,
					   -8,
					   -3,
					   -54,
					   -2,
					   -39,
					   -15,
					   19,
					   -33,
					   36,
					   -20,
					   -11,
					   18,
					   -51,
					   32,
					   -26,
					   2,
					   27,
					   -20,
					   38,
					   5,
					   9,
					   38,
					   -2,
					   14,
					   16,
					   -52,
					   9,
					   -69,
					   -46,
					   -4,
					   -82,
					   62,
					   -30,
					   42,
					   73,
					   -35,
					   123,
					   -61,
					   69,
					   -3,
					   -24,
					   63,
					   -72,
					   61,
					   -61,
					   16,
					   -33,
					   -3,
					   -13,
					   8,
					   0,
					   -6,
					   5,
					   -55,
					   -3,
					   -85,
					   -6,
					   -50,
					   23,
					   11,
					   69,
					   39,
					   72,
					   28,
					   7,
					   22,
					   -66,
					   37,
					   -71,
					   39,
					   -14,
					   17,
					   25,
					   -1,
					   7,
					   3,
					   -18,
					   -3,
					   6,
					   -49,
					   49,
					   -95,
					   39,
					   -71,
					   -29,
					   23,
					   -80,
					   96,
					   -58,
					   74,
					   6,
					   -3,
					   46,
					   -41,
					   45,
					   -12,
					   38,
					   22,
					   40,
					   4,
					   27,
					   -37,
					   -13,
					   -34,
					   -48,
					   17,
					   -50,
					   58,
					   -35,
					   38,
					   -32,
					   -20,
					   -27,
					   -54,
					   15,
					   -33,
					   79,
					   17,
					   92,
					   47,
					   15,
					   27,
					   -84,
					   -27,
					   -99,
					   -67,
					   -7,
					   -51,
					   92,
					   13,
					   96,
					   69,
					   12,
					   64,
					   -63,
					   8,
					   -63,
					   -32,
					   -19,
					   -15,
					   2,
					   27,
					   -16,
					   25,
					   -24,
					   -32,
					   15,
					   -78,
					   72,
					   -54,
					   83,
					   23,
					   24,
					   75,
					   -60,
					   55,
					   -98,
					   -5,
					   -63,
					   -43,
					   7,
					   -33,
					   53,
					   -4,
					   51,
					   12,
					   26,
					   13,
					   10,
					   16,
					   9,
					   23,
					   1,
					   21,
					   -22,
					   -1,
					   -41,
					   -32,
					   -36,
					   -49,
					   -11,
					   -39,
					   8,
					   -5,
					   9,
					   34,
					   8,
					   56,
					   20,
					   45,
					   33,
					   5,
					   22,
					   -39,
					   -11,
					   -58,
					   -30,
					   -39,
					   -10,
					   3,
					   26,
					   40,
					   29,
					   49,
					   -12,
					   29,
					   -55,
					   -5,
					   -54,
					   -29,
					   -10,
					   -25,
					   36,
					   1,
					   50,
					   17,
					   36,
					   2,
					   12,
					   -28,
					   -15,
					   -34,
					   -38,
					   -3,
					   -37,
					   30,
					   -3,
					   26,
					   36,
					   -7,
					   36,
					   -20,
					   -5,
					   12,
					   -35,
					   51,
					   -14,
					   43,
					   33,
					   -7,
					   42,
					   -47,
					   -4,
					   -45,
					   -54,
					   -22,
					   -56,
					   -13,
					   -21,
					   -13,
					   8,
					   5,
					   18,
					   35,
					   34,
					   41,
					   60,
					   12,
					   59,
					   -15,
					   13,
					   -11,
					   -40,
					   6,
					   -49,
					   3,
					   -22,
					   -8,
					   -15,
					   8,
					   -44,
					   42,
					   -58,
					   35,
					   -12,
					   -40,
					   62,
					   -111,
					   92,
					   -91,
					   55,
					   8,
					   2,
					   82,
					   -20,
					   66,
					   -19,
					   15,
					   -24,
					   13,
					   -28,
					   53,
					   -10,
					   53,
					   18,
					   -24,
					   13,
					   -102,
					   -29,
					   -96,
					   -50,
					   -17,
					   -9,
					   44,
					   56,
					   36,
					   72,
					   -1,
					   18,
					   -8,
					   -44,
					   18,
					   -55,
					   32,
					   -19,
					   13,
					   18,
					   -14,
					   34,
					   -23,
					   37,
					   -9,
					   35,
					   13,
					   14,
					   28,
					   -24,
					   24,
					   -57,
					   -5,
					   -60,
					   -43,
					   -38,
					   -54,
					   -10,
					   -20,
					   15,
					   33,
					   39,
					   55,
					   52,
					   27,
					   42,
					   -22,
					   18,
					   -51,
					   3,
					   -44,
					   2,
					   -7,
					   -5,
					   40,
					   -30,
					   69,
					   -53,
					   51,
					   -42,
					   -5,
					   -2,
					   -46,
					   29,
					   -25,
					   29,
					   37,
					   12,
					   57,
					   -4,
					   -12,
					   -19,
					   -114,
					   -36,
					   -145,
					   -32,
					   -66,
					   14,
					   58,
					   77,
					   130,
					   98,
					   116,
					   46,
					   55,
					   -33,
					   6,
					   -71,
					   -11,
					   -52,
					   -10,
					   -19,
					   -15,
					   -16,
					   -38,
					   -35,
					   -67,
					   -37,
					   -75,
					   -6,
					   -43,
					   38,
					   9,
					   65,
					   40,
					   61,
					   33,
					   32,
					   17,
					   -2,
					   27,
					   -20,
					   56,
					   -13,
					   59,
					   8,
					   17,
					   15,
					   -35,
					   -5,
					   -49,
					   -41,
					   -26,
					   -68,
					   -8,
					   -67,
					   -21,
					   -34,
					   -41,
					   20,
					   -34,
					   68,
					   4,
					   81,
					   38,
					   48,
					   40,
					   1,
					   15,
					   -17,
					   -7,
					   2,
					   -4,
					   21,
					   23,
					   4,
					   44,
					   -33,
					   33,
					   -52,
					   -9,
					   -40,
					   -50,
					   -25,
					   -57,
					   -25,
					   -31,
					   -17,
					   -4,
					   22,
					   5,
					   69,
					   7,
					   73,
					   22,
					   25,
					   35,
					   -23,
					   18,
					   -23,
					   -22,
					   10,
					   -41,
					   21,
					   -12,
					   -9,
					   33,
					   -45,
					   45,
					   -50,
					   18,
					   -26,
					   -6,
					   -7,
					   -4,
					   -1,
					   1,
					   11,
					   -22,
					   42,
					   -52,
					   72,
					   -47,
					   68,
					   -4,
					   19,
					   28,
					   -52,
					   18,
					   -104,
					   2,
					   -103,
					   25,
					   -42,
					   71,
					   51,
					   68,
					   117,
					   -13,
					   111,
					   -103,
					   37,
					   -104,
					   -50,
					   -9,
					   -91,
					   79,
					   -67,
					   65,
					   -14,
					   -29,
					   20,
					   -87,
					   16,
					   -37,
					   -3,
					   67,
					   -2,
					   110,
					   28,
					   51,
					   56,
					   -36,
					   46,
					   -61,
					   -1,
					   -28,
					   -46,
					   -11,
					   -55,
					   -44,
					   -35,
					   -64,
					   -16,
					   -1,
					   -10,
					   103,
					   0,
					   130,
					   28,
					   33,
					   57,
					   -88,
					   60,
					   -103,
					   38,
					   -12,
					   7,
					   63,
					   -31,
					   35,
					   -79,
					   -35,
					   -111,
					   -40,
					   -76,
					   26,
					   28,
					   60,
					   123,
					   2,
					   117,
					   -78,
					   16,
					   -74,
					   -67,
					   13,
					   -50,
					   79,
					   23,
					   58,
					   45,
					   -3,
					   -15,
					   -20,
					   -70,
					   11,
					   -42,
					   22,
					   36,
					   -22,
					   64,
					   -71,
					   11,
					   -60,
					   -53,
					   7,
					   -47,
					   64,
					   18,
					   66,
					   64,
					   25,
					   43,
					   -15,
					   -17,
					   -32,
					   -60,
					   -31,
					   -55,
					   -24,
					   -10,
					   -19,
					   50,
					   -12,
					   87,
					   3,
					   68,
					   29,
					   -5,
					   54,
					   -84,
					   57,
					   -106,
					   26,
					   -55,
					   -20,
					   23,
					   -49,
					   67,
					   -50,
					   61,
					   -37,
					   31,
					   -26,
					   3,
					   -9,
					   -18,
					   19,
					   -39,
					   44,
					   -51,
					   45,
					   -40,
					   21,
					   3,
					   -2,
					   60,
					   -8,
					   95,
					   -10,
					   75,
					   -19,
					   -1,
					   -25,
					   -84,
					   -5,
					   -111,
					   29,
					   -63,
					   41,
					   9,
					   13,
					   40,
					   -30,
					   22,
					   -50,
					   5,
					   -42,
					   27,
					   -22,
					   59,
					   2,
					   48,
					   38,
					   -8,
					   77,
					   -52,
					   81,
					   -42,
					   24,
					   -5,
					   -65,
					   6,
					   -112,
					   -19,
					   -75,
					   -34,
					   19,
					   -6,
					   96,
					   39,
					   98,
					   55,
					   24,
					   26,
					   -71,
					   -16,
					   -110,
					   -38,
					   -61,
					   -31,
					   38,
					   -9,
					   101,
					   11,
					   73,
					   18,
					   -11,
					   12,
					   -69,
					   0,
					   -60,
					   -1,
					   -17,
					   10,
					   7,
					   16,
					   13,
					   6,
					   36,
					   -12,
					   76,
					   -16,
					   78,
					   -8,
					   7,
					   -8,
					   -91,
					   -20,
					   -127,
					   -24,
					   -69,
					   -5,
					   23,
					   21,
					   69,
					   26,
					   49,
					   13,
					   8,
					   11,
					   -7,
					   28,
					   5,
					   29,
					   19,
					   -10,
					   15,
					   -53,
					   -8,
					   -46,
					   -30,
					   13,
					   -31,
					   54,
					   -3,
					   22,
					   30,
					   -52,
					   36,
					   -83,
					   6,
					   -35,
					   -30,
					   45,
					   -36,
					   83,
					   -13,
					   58,
					   6,
					   3,
					   3,
					   -37,
					   -6,
					   -43,
					   -0,
					   -21,
					   13,
					   14,
					   15,
					   38,
					   11,
					   35,
					   21,
					   5,
					   38,
					   -27,
					   29,
					   -43,
					   -17,
					   -40,
					   -58,
					   -22,
					   -52,
					   4,
					   -13,
					   29,
					   4,
					   31,
					   -17,
					   7,
					   -30,
					   -14,
					   8,
					   3,
					   69,
					   46,
					   80,
					   55,
					   20,
					   -3,
					   -49,
					   -72,
					   -61,
					   -68,
					   -13,
					   14,
					   38,
					   77,
					   50,
					   40,
					   27,
					   -65,
					   -6,
					   -119,
					   -39,
					   -61,
					   -66,
					   49,
					   -73,
					   105,
					   -43,
					   72,
					   16,
					   9,
					   70,
					   -17,
					   90,
					   2,
					   69,
					   20,
					   24,
					   5,
					   -22,
					   -35,
					   -47,
					   -67,
					   -47,
					   -64,
					   -32,
					   -28,
					   -18,
					   17,
					   -11,
					   46,
					   0,
					   50,
					   18,
					   41,
					   26,
					   29,
					   12,
					   11,
					   -6,
					   -16,
					   1,
					   -43,
					   32,
					   -46,
					   43,
					   -23,
					   5,
					   6,
					   -53,
					   19,
					   -73,
					   18,
					   -34,
					   15,
					   22,
					   9,
					   45,
					   -12,
					   28,
					   -31,
					   5,
					   -19,
					   1,
					   29,
					   12,
					   68,
					   16,
					   46,
					   1,
					   -26,
					   -29,
					   -85,
					   -53,
					   -79,
					   -43,
					   -22,
					   8,
					   38,
					   66,
					   64,
					   75,
					   54,
					   14,
					   20,
					   -64,
					   -19,
					   -82,
					   -37,
					   -27,
					   -17,
					   42,
					   19,
					   63,
					   24,
					   38,
					   -14,
					   8,
					   -52,
					   -5,
					   -37,
					   -19,
					   21,
					   -45,
					   55,
					   -54,
					   29,
					   -23,
					   -20,
					   31,
					   -31,
					   60,
					   -1,
					   44,
					   19,
					   6,
					   0,
					   -26,
					   -26,
					   -43,
					   -13,
					   -43,
					   33,
					   -19,
					   63,
					   28,
					   39,
					   66,
					   -24,
					   60,
					   -80,
					   11,
					   -92,
					   -37,
					   -48,
					   -50,
					   32,
					   -37,
					   101,
					   -25,
					   108,
					   -18,
					   41,
					   7,
					   -50,
					   51,
					   -92,
					   77,
					   -55,
					   50,
					   12,
					   -15,
					   38,
					   -67,
					   0,
					   -69,
					   -48,
					   -36,
					   -41,
					   -7,
					   31,
					   3,
					   105,
					   17,
					   106,
					   53,
					   27,
					   91,
					   -65,
					   82,
					   -95,
					   6,
					   -57,
					   -91,
					   -14,
					   -128,
					   -19,
					   -73,
					   -50,
					   20,
					   -41,
					   67,
					   33,
					   39,
					   117,
					   -14,
					   139,
					   -30,
					   84,
					   -4,
					   0,
					   22,
					   -62,
					   26,
					   -94,
					   27,
					   -101,
					   41,
					   -78,
					   47,
					   -18,
					   13,
					   48,
					   -49,
					   70,
					   -88,
					   33,
					   -73,
					   -15,
					   -27,
					   -17,
					   1,
					   29,
					   3,
					   65,
					   6,
					   42,
					   35,
					   -26,
					   72,
					   -73,
					   85,
					   -54,
					   56,
					   10,
					   -4,
					   55,
					   -64,
					   33,
					   -92,
					   -33,
					   -68,
					   -72,
					   -6,
					   -38,
					   46,
					   37,
					   43,
					   73,
					   -7,
					   31,
					   -45,
					   -39,
					   -26,
					   -54,
					   34,
					   5,
					   72,
					   63,
					   50,
					   47,
					   -10,
					   -28,
					   -55,
					   -72,
					   -59,
					   -28,
					   -33,
					   56,
					   1,
					   84,
					   37,
					   20,
					   69,
					   -71,
					   76,
					   -100,
					   27,
					   -46,
					   -65,
					   27,
					   -131,
					   52,
					   -99,
					   29,
					   20,
					   9,
					   121,
					   21,
					   111,
					   35,
					   3,
					   6,
					   -88,
					   -52,
					   -77,
					   -70,
					   0,
					   -12,
					   46,
					   71,
					   26,
					   87,
					   0,
					   12,
					   23,
					   -80,
					   56,
					   -102,
					   27,
					   -47,
					   -57,
					   26,
					   -101,
					   70,
					   -49,
					   78,
					   37,
					   55,
					   56,
					   -7,
					   -1,
					   -86,
					   -41,
					   -112,
					   -5,
					   -37,
					   56,
					   79,
					   58,
					   119,
					   -4,
					   43,
					   -53,
					   -53,
					   -37,
					   -59,
					   14,
					   13,
					   36,
					   48,
					   15,
					   -7,
					   -15,
					   -75,
					   -22,
					   -61,
					   -6,
					   13,
					   16,
					   50,
					   24,
					   11,
					   3,
					   -30,
					   -45,
					   -4,
					   -78,
					   58,
					   -44,
					   72,
					   46,
					   22,
					   108,
					   -31,
					   74,
					   -39,
					   -17,
					   -25,
					   -61,
					   -26,
					   -13,
					   -28,
					   48,
					   -1,
					   30,
					   36,
					   -54,
					   27,
					   -98,
					   -31,
					   -44,
					   -66,
					   50,
					   -18,
					   86,
					   72,
					   38,
					   105,
					   -32,
					   41,
					   -60,
					   -49,
					   -40,
					   -77,
					   -1,
					   -33,
					   36,
					   18,
					   62,
					   22,
					   60,
					   -1,
					   20,
					   -0,
					   -32,
					   30,
					   -54,
					   41,
					   -33,
					   -5,
					   -4,
					   -80,
					   1,
					   -110,
					   -13,
					   -49,
					   -14,
					   63,
					   0,
					   131,
					   7,
					   97,
					   -3,
					   -1,
					   -2,
					   -67,
					   35,
					   -48,
					   75,
					   13,
					   59,
					   34,
					   -26,
					   -10,
					   -107,
					   -62,
					   -99,
					   -56,
					   -0,
					   4,
					   98,
					   52,
					   106,
					   41,
					   24,
					   -2,
					   -66,
					   -20,
					   -93,
					   -1,
					   -60,
					   11,
					   -13,
					   -12,
					   22,
					   -38,
					   46,
					   -9,
					   59,
					   69,
					   46,
					   109,
					   7,
					   41,
					   -23,
					   -88,
					   -17,
					   -151,
					   6,
					   -80,
					   -0,
					   53,
					   -39,
					   114,
					   -58,
					   57,
					   -16,
					   -36,
					   47,
					   -63,
					   58,
					   -18,
					   5,
					   25,
					   -45,
					   23};

int json_myobj_read(const char *buf, parse_t *myobj)
{

	const struct json_attr_t json_attrs[] = {
		{"msg_type", t_uinteger, .addr.uinteger = &(myobj->msg_type)},
		{"gscn", t_uinteger, .addr.uinteger = &(myobj->gscn)},
		{NULL},
	};

	return json_read_object(buf, json_attrs, NULL);
}

static int server_init(__attribute__((unused)) void *dummy)
{
	{
		int server_fd, new_socket, n;
		struct sockaddr_in address;
		// int i=0;
		int opt = 1;
		int ret, val;
		int addrlen = sizeof(address);
		// char* hello = "Server is connected";
		char request[64] = {0};
		fapi_request_nr_t *fapi_req;
		nr_cell_follow_request_t *cfb_req;
		nr_cell_follow_request_t *cf_req;

		// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		// Forcefully attaching socket to the port 8080
		if (setsockopt(server_fd, SOL_SOCKET,
					   SO_REUSEADDR | SO_REUSEPORT, &opt,
					   sizeof(opt)))
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		// Forcefully attaching socket to the port 8080
		if (bind(server_fd, (struct sockaddr *)&address,
				 sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(server_fd, 3) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
								 (socklen_t *)&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		/*send(new_socket, hello, strlen(hello), 0);
		printf("Server is connected\n");*/

		while (1)
		{
			bzero(buffer, 1024);
			n = recv(new_socket, buffer, 1024, MSG_DONTWAIT);
			if (n < 0)
				nmm_print_dbg("Error on reading.");
			//printf("New Messaage %s\n", buffer);
			// Case to identify command type and send it to controller for processing
			parse_t *myobj;
			int status = json_myobj_read(buffer, myobj);
			if (status == 0)
			{
				nmm_print_dbg("msg_type: %d\n", myobj->msg_type);
				nmm_print_dbg("gscn: %d\n", myobj->gscn);
			}
			else
			{
				nmm_print_dbg(json_error_string(status));
			}
			switch (myobj->msg_type)
			{
			case 9:
				val = 9;
				fapi_req = (fapi_request_ssb_t *)request;
				fapi_req->message_type = BAND_SCAN_N77_REQUEST;
				ret = nmm_fapi_send_request((fapi_request_t *)fapi_req);
				if (ret)
					printf("Error in band scan n77 request\n");
				break;
			case 1:
				val = 1;
				fapi_req = (fapi_request_nr_t *)request;
				fapi_req->message_type = CELL_FOLLOW_REQUEST;
				cf_req = (nr_cell_follow_request_t *)&fapi_req->message_body;
				cf_req->gscn = myobj->gscn;
				ret = nmm_fapi_send_request((fapi_request_t *)fapi_req);
				if (ret < 0)
					printf("Error in cell follow request\n");
				break;
			case 2:
				val = 2;
				fapi_req = (fapi_request_nr_t *)request;
				fapi_req->message_type = CELL_FOLLOW_MIB_REQUEST;
				cfb_req = (nr_cell_follow_request_t *)&fapi_req->message_body;
				cfb_req->gscn = myobj->gscn;
				ret = nmm_fapi_send_request((fapi_request_t *)fapi_req);
				if (ret < 0)
					printf("Error in cell follow request\n");
				break;
			case 3:
				val = 3;
				fapi_req = (fapi_request_ssb_t *)request;
				fapi_req->message_type = CELL_FOLLOW_STOP_REQUEST;
				ret = nmm_fapi_send_request((fapi_request_t *)fapi_req);
				if (ret < 0)
					printf("Error disabling test mode\n");
				break;
			default:
				nmm_print_dbg("Message type not supported");
				break;
			}
			
			// 1.Send Cell Acquisition information, Request cell acquisition
			// 2.Send SSB stats
			// 3.Send MIB info send SSB stats also
			if (val == 9)
			{
				usleep(65000000);
				sprintf(reply, "\n{\"msg_type\":0,\"cell_follow_cell_id\":%d,\"cell_rssi_dBm\":%d,\"cell_snr_dB\":%d}\n\r",
						cell_follow_stats.cell_follow_cell_id,
						cell_follow_stats.cell_rssi_dBm,
						cell_follow_stats.cell_snr_dB);
				send(new_socket, reply, strlen(reply), 0);
				val=0;
			}
			else if (val == 1)
			{
				usleep(5000000);
				sprintf(reply, "\n{\"msg_type\":1,\"cell_follow_cell_id\":%d,\"cell_rssi_dBm\":%d,\"cell_snr_dB\":%d,\"cfo_ppb\":%d,\"cell_tracking\":%d,\"msg_type\":2,\"n_missed_detection\":%d,\"n_false_alarm\": %d,\"n_rx_ssb\":%d,\"n_rx_mib\":%d,\"n_mib_crc_fail\":%d}\n\r",
						cell_follow_stats.cell_follow_cell_id,
						cell_follow_stats.cell_rssi_dBm,
						cell_follow_stats.cell_snr_dB,
						cell_follow_stats.cfo_ppb,
						cell_follow_stats.cell_tracking,
						cell_follow_stats.n_missed_detection,
						cell_follow_stats.n_false_alarm,
						cell_follow_stats.n_rx_ssb,
						cell_follow_stats.n_rx_mib,
						cell_follow_stats.n_mib_crc_fail);
				send(new_socket, reply, strlen(reply), 0);
				printf("%s", reply);
				/*
				bzero(buffer, 1024);
				if (n = read(new_socket, buffer, 1024));
				{
				parse_t *myobj = malloc(sizeof(parse_t));
				int status = json_myobj_read(buffer, myobj);
				   if (status == 0) {
					nmm_print_dbg("msg_type: %d\n", myobj->msg_type);
				   }
				if (myobj->msg_type==3){
				fapi_req = (fapi_request_ssb_t *)request;
				fapi_req->message_type = CELL_FOLLOW_STOP_REQUEST;
				ret = nmm_fapi_send_request((fapi_request_t*)fapi_req);
				break;
				}
				}*/
			}
			else if (val == 2)
			{
				usleep(5000000);
				sprintf(reply, "\n{\"msg_type\":3,\"ssb_subcarrierOffset\":%d,\"pdcch_configSIB1\":%d,\"dmrs_typeA_Position\":%d,\"cellBarred\":%d,\"intraFreqReselection\":%d,\"subCarrierSpacingCommon\":%d,\"msg_type\":1,\"cell_follow_cell_id\":%d,\"cell_rssi_dBm\":%d,\"cell_snr_dB\":%d,\"cfo_ppb\":%d,\"cell_tracking\":%d,\"msg_type\":2,\"n_missed_detection\":%d,\"n_false_alarm\": %d,\"n_rx_ssb\":%d,\"n_rx_mib\":%d,\"n_mib_crc_fail\":%d}\n\r",
						stats.ssb_subcarrierOffset,
						stats.pdcch_configSIB1,
						stats.dmrs_typeA_Position,
						stats.cellBarred,
						stats.intraFreqReselection,
						stats.subCarrierSpacingCommon,
						cell_follow_stats.cell_follow_cell_id,
						cell_follow_stats.cell_rssi_dBm,
						cell_follow_stats.cell_snr_dB,
						cell_follow_stats.cfo_ppb,
						cell_follow_stats.cell_tracking,
						cell_follow_stats.n_missed_detection,
						cell_follow_stats.n_false_alarm,
						cell_follow_stats.n_rx_ssb,
						cell_follow_stats.n_rx_mib,
						cell_follow_stats.n_mib_crc_fail);
				send(new_socket, reply, strlen(reply), 0);
				printf("%s", reply);
			}
			else if (val == 3)
			{	usleep(5000000);
				//sprintf(reply, "\n{\"msg_type\":3,\"Cell Follow Stopped\"}\n");
				sprintf(reply, "\n{\"Cell Follow Stopped\"}\n");
				send(new_socket, reply, strlen(reply), 0);
				val=0;
			}
			if (n < 0)
				nmm_print_dbg("Error on writing.");
		}
		close(new_socket);
		close(server_fd);
		return 0;
	}

	return 0;
}

void fapi_receive_response_dflt(fapi_response_t *rsp)
{
	nmm_print("Received FAPI response: type = %d, status_code = %d\n",
			  rsp->message_type, rsp->error_code);
}

void fapi_receive_indication_dflt(fapi_indication_t *ind)
{
	nmm_print("Received FAPI indication: type = %d, err_code = %d\n",
			  ind->message_type, ind->error_code);
}

void dbg_receive_response_dflt(nmm_dbg_response_t *res)
{
	nmm_print("Received dbg result: type = %d, err_code = %d\n",
			  res->message_type, res->error_code);
}

static int register_callbacks(struct nmm_cb *cbs)
{
	if (!cbs)
	{
		nmm_print("No user callback provided\n");
		return -EINVAL;
	}
	user_cbs = cbs;

	/* If any of the user callbacks is missing, use default stub */
	if (!user_cbs->fapi_receive_response)
		user_cbs->fapi_receive_response = fapi_receive_response_dflt;
	if (!user_cbs->fapi_receive_indication)
		user_cbs->fapi_receive_indication = fapi_receive_indication_dflt;
	if (!user_cbs->dbg_receive_response)
		user_cbs->dbg_receive_response = dbg_receive_response_dflt;

	return 0;
}

static int nmm_user_cb_loop(__attribute__((unused)) void *dummy)
{
	uint32_t type;
	void *arg;
	int ret;

	while (do_poll)
	{
		do
		{
			if (!do_poll)
				return 0;
			ret = rte_ring_dequeue(user_cb_ring, &arg);
		} while (ret == -ENOENT);

		type = *(uint32_t *)arg;
		switch (type)
		{
		case CELL_SEARCH_RESPONSE:
		case SSB_SCAN_RESPONSE:
			break;
		case BROADCAST_DETECT_RESPONSE:
		case SYSTEM_INFORMATION_SCHEDULE_RESPONSE:
		case SYSTEM_INFORMATION_RESPONSE:
		case NMM_STOP_RESPONSE:
		case CELL_FOLLOW_RESPONSE:
			user_cbs->fapi_receive_response(arg);
			break;
		case CELL_FOLLOW_MIB_RESPONSE:
			user_cbs->fapi_receive_response(arg);
			break;
		case CELL_FOLLOW_SIB_1_RESPONSE:
			user_cbs->fapi_receive_response(arg);
		case CELL_FOLLOW_STOP_RESPONSE:
			break;
		case CELL_SEARCH_INDICATION:
		case BROADCAST_DETECT_INDICATION:
		case SYSTEM_INFORMATION_SCHEDULE_INDICATION:
		case SYSTEM_INFORMATION_INDICATION:
		case CELL_FOLLOW_INDICATION:
			user_cbs->fapi_receive_indication(arg);
			break;
		case NMM_DBG_RX_CAPTURE_RESULT:
		case NMM_DBG_DECIMATOR_CAPTURE_RESULT:
		case NMM_DBG_TEST_MODE_ENABLE_RESULT:
		case NMM_DBG_TEST_MODE_DISABLE_RESULT:
		case NMM_DBG_FORCE_RESET_RESULT:
			user_cbs->dbg_receive_response(arg);
			break;
		default:
			nmm_print("Unknown message type %d\n", type);
			break;
		}
		rte_free(arg);
	}

	return 0;
}

int nmm_init(struct nmm_cb *cbs)
{
	const char lib_ver[] = "libnmm.a version " NMM_LIB_VERSION;
	unsigned int lcore_id;
	int ret;

	if (rte_atomic16_test_and_set(&state.initialized) == 0)
	{
		nmm_print("NMM Library already initialized\n");
		return -EINVAL;
	}

	ret = register_callbacks(cbs);
	if (ret < 0)
		goto out_err;

	ret = init_bbdev();
	if (ret < 0)
		goto out_err;

	ret = init_vspa_mem();
	if (ret < 0)
		goto out_err;

	memset(&state, 0, sizeof(state));

	dl_proc_setup();

	ret = create_cmd_timer();
	if (ret < 0)
		goto out_err;

	/* Start the IPC Rx polling routine on another core */
	lcore_id = rte_lcore_id();
	lcore_id = rte_get_next_lcore(lcore_id, 0, 1);

	nmm_print_dbg("nmm_poll_loop: waiting for messages on core %d\n",
				  rte_lcore_index(lcore_id));

	do_poll = 1;
	rte_eal_remote_launch(nmm_poll_loop, NULL, lcore_id);

	/*
	 * create user_cb ring: single consumer dequeue,
	 * but allow multi-producer safe enqueues
	 */
	lcore_id = rte_get_next_lcore(lcore_id, 0, 1);
	user_cb_ring = rte_ring_create("user_cb", NMM_CB_RING_MAX_SIZE,
								   rte_lcore_to_socket_id(lcore_id),
								   RING_F_SC_DEQ);
	if (!user_cb_ring)
	{
		nmm_print("Error creating rte_ring\n");
		goto out_err;
	}

	/* Start the user callback thread */
	nmm_print_dbg("user_cb loop: preparing to execute callbacks on core %d\n",
				  rte_lcore_index(lcore_id));
	rte_eal_remote_launch(nmm_user_cb_loop, NULL, lcore_id);

	/* Creating SIB decoding task  */

	lcore_id = rte_get_next_lcore(lcore_id, 0, 1);

	nmm_print_dbg("server_init: waiting for messages on core %d\n",
				  rte_lcore_index(lcore_id));

	rte_eal_remote_launch(server_init, NULL, lcore_id);

	nmm_print("NMM Library initialized (%s)\n", lib_ver);

	return 0;

out_err:
	rte_atomic16_clear(&state.initialized);
	return ret;
}

static void signal_ipc_reset(void)
{
	struct nmm_msg *msg;
	int ret;

	/* Notify FreeRTOS we're shutting down */
	msg = (struct nmm_msg *)get_tx_buf(BBDEV_IPC_H2M_QUEUE);
	if (!msg)
		return;
	msg->type = NMM_IPC_RESET;

	ret = send_nmm_command(msg);
	if (ret < 0)
		nmm_print("Failed to send IPC_RESET message\n");

	disarm_cmd_timer();
	state.vspa_active = 0;
	rte_atomic16_clear(&state.is_active);
}

void nmm_free(void)
{

	do_poll = 0;
	/*
	 * Wait for any Rx message already received to finish processing
	 * before freeing bbdev resources
	 */
	rte_eal_mp_wait_lcore();

	signal_ipc_reset();
	sleep(1);
	rte_bbdev_close(dev_id);

	dl_proc_clean();
	delete_cmd_timer();

	rte_atomic16_clear(&state.initialized);
}