/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2016 Intel Corporation. All rights reserved.
 * Copyright 2019-2020 NXP
 */

/*
 * Sample application demostrating how to do packet I/O in a multi-process
 * environment. The same code can be run as a primary process and as a
 * secondary process, just with a different proc-id parameter in each case
 * (apart from the EAL flag to indicate a secondary process).
 *
 * Each process will read from the same ports, given by the port-mask
 * parameter, which should be the same in each case, just using a different
 * queue per port as determined by the proc-id parameter.
 *
 * Rx is performed from each port, QDMA'd to a memory location and from there
 * pushed out to the Tx Port
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/queue.h>
#include <getopt.h>
#include <signal.h>
#include <inttypes.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_atomic.h>
#include <rte_branch_prediction.h>
#include <rte_debug.h>
#include <rte_interrupts.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_memcpy.h>
#include <rte_mbuf.h>
#include <rte_string_fns.h>
#include <rte_cycles.h>
#include <rte_pmd_dpaa2_qdma.h>

#define RTE_LOGTYPE_APP RTE_LOGTYPE_USER1

#define NB_MBUFS 64*1024 /* use 64k mbufs */
#define MBUF_CACHE_SIZE 256
#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define PARAM_PROC_ID "proc-id"
#define PARAM_NUM_PROCS "num-procs"

/* Signal handler */
static int signal_received = 0;

/* for each lcore, record the elements of the ports array to use */
struct lcore_ports{
	unsigned start_port;
	unsigned num_ports;
};

/* structure to record the rx and tx packets. Put two per cache line as ports
 * used in pairs */
struct port_stats{
	unsigned rx;
	unsigned tx;
	unsigned drop;
} __attribute__((aligned(RTE_CACHE_LINE_SIZE / 2)));

/* Global pools containing QDMA objects and scheduled jobs */
struct rte_mempool *qdma_obj_pool = NULL;
struct rte_mempool *qdma_job_pool = NULL;

struct qdma_job_cnxt {
	struct rte_mbuf *in_mbuf;
	struct rte_mbuf *out_mbuf;
	unsigned int dst_port;
};

#define QDMA_JOB_CNXT_SIZE (sizeof(struct rte_qdma_job) + \
	sizeof(struct qdma_job_cnxt))

static int proc_id = -1;
static unsigned num_procs = 0;

static uint16_t ports[RTE_MAX_ETHPORTS];
static unsigned num_ports = 0;
static unsigned port_mask = 0;
static int qdma_dev_id;

static struct lcore_ports lcore_ports[RTE_MAX_LCORE];
static struct port_stats pstats[RTE_MAX_ETHPORTS];

#define QDMA_MAX_HW_QUEUES_PER_CORE	1
#define QDMA_FLE_POOL_COUNT		2048
#define QDMA_MAX_VQS			128
#define QDMA_OBJ_POOL_NAME		"qdma_obj_pool"
#define QDMA_JOB_POOL_NAME		"qdma_job_pool"

/* Determines H/W or virtual mode */
uint8_t qdma_mode = RTE_QDMA_MODE_HW;

/* prints the usage statement and quits with an error message */
static void
smp_usage(const char *prgname, const char *errmsg)
{
	printf("\nError: %s\n",errmsg);
	printf("\n%s [EAL options] -- -p <port mask> "
			"--"PARAM_NUM_PROCS" <n>"
			" --"PARAM_PROC_ID" <id>\n"
			"-p         : a hex bitmask indicating what ports are to be used\n"
			"--num-procs: the number of processes which will be used\n"
			"--proc-id  : the id of the current process (id < num-procs)\n"
			"-m mode: HW (0) or Virtual (1) mode for QDMA; Default HW\n"
			"\n",
			prgname);
	exit(1);
}


/* signal handler configured for SIGTERM and SIGINT to print stats on exit */
static void
print_stats(int signum)
{
	unsigned i;
	signal_received = 1;
	printf("\nExiting on signal %d\n\n", signum);
	for (i = 0; i < num_ports; i++){
		const uint8_t p_num = ports[i];
		printf("Port %u: RX - %u, TX - %u, Drop - %u\n", (unsigned)p_num,
				pstats[p_num].rx, pstats[p_num].tx, pstats[p_num].drop);
	}
}

static int
qdma_parse_mode(const char *mode)
{
	char *end = NULL;
	int m;

	/* parse hexadecimal string */
	m = strtoul(mode, &end, 16);
	if ((mode[0] == '\0') || (end == NULL) || (*end != '\0'))
		return -1;

	return m;
}

/* Parse the argument given in the command line of the application */
static int
smp_parse_args(int argc, char **argv)
{
	int opt, ret, mode;
	char **argvopt;
	int option_index;
	uint16_t i;
	char *prgname = argv[0];
	static struct option lgopts[] = {
			{PARAM_NUM_PROCS, 1, 0, 0},
			{PARAM_PROC_ID, 1, 0, 0},
			{NULL, 0, 0, 0}
	};

	argvopt = argv;

	while ((opt = getopt_long(argc, argvopt, "p:q:m:", \
			lgopts, &option_index)) != EOF) {

		switch (opt) {
		case 'p':
			port_mask = strtoull(optarg, NULL, 16);
			break;
		case 'm':
			mode = qdma_parse_mode(optarg);
			if (mode < 0) {
				printf("Invalid QDMA Mode (%d)\n", mode);
				smp_usage(prgname, "Invalid QDMA Mode\n");
				return -1;
			}
			qdma_mode = mode;
			break;
			/* long options */
		case 0:
			if (strncmp(lgopts[option_index].name, PARAM_NUM_PROCS, 8) == 0)
				num_procs = atoi(optarg);
			else if (strncmp(lgopts[option_index].name, PARAM_PROC_ID, 7) == 0)
				proc_id = atoi(optarg);
			break;

		default:
			smp_usage(prgname, "Cannot parse all command-line arguments\n");
		}
	}

	if (optind >= 0)
		argv[optind-1] = prgname;

	if (proc_id < 0)
		smp_usage(prgname, "Invalid or missing proc-id parameter\n");
	if (rte_eal_process_type() == RTE_PROC_PRIMARY && num_procs == 0)
		smp_usage(prgname, "Invalid or missing num-procs parameter\n");
	if (port_mask == 0)
		smp_usage(prgname, "Invalid or missing port mask\n");

	/* get the port numbers from the port mask */
	RTE_ETH_FOREACH_DEV(i)
		if(port_mask & (1 << i))
			ports[num_ports++] = (uint8_t)i;

	ret = optind-1;
	optind = 1; /* reset getopt lib */

	return ret;
}

/*
 * Initialises a given port using global settings and with the rx buffers
 * coming from the mbuf_pool passed as parameter
 */
static inline int
smp_port_init(uint16_t port, struct rte_mempool *mbuf_pool,
	       uint16_t num_queues)
{
	struct rte_eth_conf port_conf = {
			.rxmode = {
				.mq_mode	= ETH_MQ_RX_RSS,
				.split_hdr_size = 0,
				.offloads = DEV_RX_OFFLOAD_CHECKSUM,
			},
			.rx_adv_conf = {
				.rss_conf = {
					.rss_key = NULL,
					.rss_hf = ETH_RSS_IP,
				},
			},
			.txmode = {
				.mq_mode = ETH_MQ_TX_NONE,
			}
	};
	const uint16_t rx_rings = num_queues, tx_rings = num_queues;
	struct rte_eth_dev_info info;
	struct rte_eth_rxconf rxq_conf;
	struct rte_eth_txconf txq_conf;
	int retval;
	uint16_t q;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	uint64_t rss_hf_tmp;

	if (rte_eal_process_type() == RTE_PROC_SECONDARY)
		return 0;

	if (!rte_eth_dev_is_valid_port(port))
		return -1;

	printf("# Initialising port %u... ", port);
	fflush(stdout);

	rte_eth_dev_info_get(port, &info);
	info.default_rxconf.rx_drop_en = 1;

	if (info.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
		port_conf.txmode.offloads |=
			DEV_TX_OFFLOAD_MBUF_FAST_FREE;

	rss_hf_tmp = port_conf.rx_adv_conf.rss_conf.rss_hf;
	port_conf.rx_adv_conf.rss_conf.rss_hf &= info.flow_type_rss_offloads;
	if (port_conf.rx_adv_conf.rss_conf.rss_hf != rss_hf_tmp) {
		printf("Port %u modified RSS hash function based on hardware support,"
			"requested:%#"PRIx64" configured:%#"PRIx64"\n",
			port,
			rss_hf_tmp,
			port_conf.rx_adv_conf.rss_conf.rss_hf);
	}

	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval < 0)
		return retval;

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval < 0)
		return retval;

	rxq_conf = info.default_rxconf;
	rxq_conf.offloads = port_conf.rxmode.offloads;
	for (q = 0; q < rx_rings; q ++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
				rte_eth_dev_socket_id(port),
				&rxq_conf,
				mbuf_pool);
		if (retval < 0)
			return retval;
	}

	txq_conf = info.default_txconf;
	txq_conf.offloads = port_conf.txmode.offloads;
	for (q = 0; q < tx_rings; q ++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd,
				rte_eth_dev_socket_id(port),
				&txq_conf);
		if (retval < 0)
			return retval;
	}

	rte_eth_promiscuous_enable(port);

	retval  = rte_eth_dev_start(port);
	if (retval < 0)
		return retval;

	return 0;
}

/* Goes through each of the lcores and calculates what ports should
 * be used by that core. Fills in the global lcore_ports[] array.
 */
static void
assign_ports_to_cores(void)
{

	const unsigned lcores = rte_lcore_count();
	const unsigned port_pairs = num_ports / 2;
	const unsigned pairs_per_lcore = port_pairs / lcores;
	unsigned extra_pairs = port_pairs % lcores;
	unsigned ports_assigned = 0;
	unsigned i;

	RTE_LCORE_FOREACH(i) {
		lcore_ports[i].start_port = ports_assigned;
		lcore_ports[i].num_ports = pairs_per_lcore * 2;
		if (extra_pairs > 0) {
			lcore_ports[i].num_ports += 2;
			extra_pairs--;
		}
		ports_assigned += lcore_ports[i].num_ports;
	}
}

static void
qdma_forward(uint16_t vq_id, int nb_jobs)
{
	struct rte_qdma_job *qdma_job[RTE_QDMA_BURST_NB_MAX];
	struct rte_mbuf *out_mbuf[RTE_QDMA_BURST_NB_MAX];
	struct qdma_job_cnxt *qdma_job_cnxt;
	unsigned int dst_port = 0;
	int to_sent = 0, sent, num_rx;
	struct rte_qdma_enqdeq context;

	context.vq_id = vq_id;
	context.job = qdma_job;
	num_rx = rte_qdma_dequeue_buffers(qdma_dev_id, NULL, nb_jobs, &context);

	if (num_rx <= 0)
		return;

	for (int i = 0; i < num_rx; i++) {
		qdma_job_cnxt = (struct qdma_job_cnxt *)qdma_job[i]->cnxt;
		rte_pktmbuf_free(qdma_job_cnxt->in_mbuf);

		if (qdma_job[i]->status != 0) {
			printf("QDMA operation returned err: %d\n",
			       qdma_job[i]->status);
			rte_mempool_put(qdma_job_pool, qdma_job[i]);
			rte_pktmbuf_free(qdma_job_cnxt->out_mbuf);
			continue;
		}

		out_mbuf[to_sent++] = qdma_job_cnxt->out_mbuf;
		dst_port = qdma_job_cnxt->dst_port;

		rte_mempool_put(qdma_job_pool, qdma_job[i]);
	}
	if (!to_sent)
		return;

	sent = rte_eth_tx_burst(dst_port, 0, out_mbuf, to_sent);
	if (sent)
		pstats[dst_port].tx += sent;
}

static void
qdma_copy(struct rte_mbuf **m, unsigned int portid,
	  uint16_t vq_id, uint8_t nb_jobs)
{
	struct rte_mbuf *m_new;
	void *m_data, *m_new_data;
	struct rte_qdma_job *qdma_job[RTE_QDMA_BURST_NB_MAX], *job;
	struct qdma_job_cnxt *qdma_job_cnxt;
	int i, ret;
	struct rte_qdma_enqdeq context;

	for (i = 0; i < nb_jobs; i++) {
		m_new = rte_pktmbuf_alloc(qdma_obj_pool);
		m_new->nb_segs = m[i]->nb_segs;
		m_new->ol_flags = m[i]->ol_flags;
		m_new->data_off = m[i]->data_off;
		m_new->data_len = m[i]->data_len;
		m_new->pkt_len = m[i]->pkt_len;
		m_new->next = m[i]->next;
		rte_mbuf_refcnt_set(m_new, 1);

		m_data = (void *)rte_pktmbuf_iova(m[i]);
		m_new_data = (void *)rte_pktmbuf_iova(m_new);

		rte_mempool_get(qdma_job_pool, (void **)&job);

		qdma_job_cnxt = (struct qdma_job_cnxt *)(job + 1);
		qdma_job_cnxt->in_mbuf = m[i];
		qdma_job_cnxt->out_mbuf = m_new;
		qdma_job_cnxt->dst_port = portid;

		job->src = (uint64_t)m_data;
		job->dest = (uint64_t)m_new_data;
		job->len = m[i]->data_len;
		job->cnxt = (uint64_t)qdma_job_cnxt;
		job->flags = RTE_QDMA_JOB_SRC_PHY | RTE_QDMA_JOB_DEST_PHY;

		qdma_job[i] = job;
	}

	context.vq_id = vq_id;
	context.job = qdma_job;
	ret = rte_qdma_enqueue_buffers(qdma_dev_id, NULL, nb_jobs, &context);
	if (ret <= 0) {
		printf("Error in QDMA job submit. Dropping packet\n");

		for (i = 0; i < nb_jobs; i++) {
			rte_pktmbuf_free(m[i]);
			rte_pktmbuf_free(
			((struct qdma_job_cnxt *)job->cnxt)->out_mbuf);
			rte_mempool_put(qdma_job_pool, qdma_job[i]);
		}
	}
}

static int
qdma_init(void) {
	int ret;
	enum rte_proc_type_t proc_type;
	struct rte_qdma_config qdma_config;
	struct rte_qdma_info dev_conf;

	proc_type = rte_eal_process_type();
	if (proc_type == RTE_PROC_PRIMARY) {
		/* create the QDMA mbuf pool */
		qdma_obj_pool = rte_pktmbuf_pool_create(QDMA_OBJ_POOL_NAME,
			NB_MBUFS + MBUF_CACHE_SIZE, MBUF_CACHE_SIZE, 0,
			RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
		if (qdma_obj_pool == NULL) {
			printf("Cannot init QDMA buffer pool\n");
			goto cleanup;
		}

		/* create the QDMA job pool */
		qdma_job_pool = rte_mempool_create(QDMA_JOB_POOL_NAME,
			NB_MBUFS, QDMA_JOB_CNXT_SIZE, MBUF_CACHE_SIZE, 0,
			NULL, NULL, NULL, NULL, rte_socket_id(), 0);
		if (qdma_job_pool == NULL) {
			printf("Cannot init QDMA job pool\n");
			goto cleanup;
		}

		ret = rte_qdma_reset(qdma_dev_id);
		if (ret)
			rte_exit(EXIT_FAILURE, "QDMA reset failed\n");
	} else {
		qdma_obj_pool = rte_mempool_lookup(QDMA_OBJ_POOL_NAME);
		if (qdma_obj_pool == NULL) {
			printf("Cannot get QDMA buffer pool\n");
			goto cleanup;
		}

		qdma_job_pool = rte_mempool_lookup(QDMA_JOB_POOL_NAME);
		if (qdma_job_pool == NULL) {
			printf("Cannot get QDMA job pool\n");
			goto cleanup;
		}
	}

	memset(&qdma_config, 0, sizeof(struct rte_qdma_config));
	qdma_config.max_hw_queues_per_core = QDMA_MAX_HW_QUEUES_PER_CORE;
	qdma_config.fle_queue_pool_cnt = QDMA_FLE_POOL_COUNT;
	qdma_config.max_vqs = QDMA_MAX_VQS;

	dev_conf.dev_private = (void *)&qdma_config;
	ret = rte_qdma_configure(qdma_dev_id, &dev_conf);
	if (ret) {
		printf("QDMA configuration failed\n");
		return -1;
	}

	ret = rte_qdma_start(qdma_dev_id);
	if (ret) {
		printf("QDMA start failed\n");
		return -1;
	}

	return 0;

cleanup:
	if (proc_type == RTE_PROC_PRIMARY) {
		rte_mempool_free(qdma_obj_pool);
		rte_mempool_free(qdma_job_pool);
	}

	qdma_obj_pool = NULL;
	qdma_job_pool = NULL;

	return -1;
}

static void
qdma_deinit(void)
{
	rte_rawdev_stop(qdma_dev_id);
	if (rte_eal_process_type() == RTE_PROC_PRIMARY)
		rte_rawdev_close(qdma_dev_id);
}

/* Main function used by the processing threads.
 * Prints out some configuration details for the thread and then begins
 * performing packet RX and TX.
 */
static int
lcore_main(void *arg __rte_unused)
{
	const unsigned id = rte_lcore_id();
	const unsigned start_port = lcore_ports[id].start_port;
	const unsigned end_port = start_port + lcore_ports[id].num_ports;
	const uint16_t q_id = (uint16_t)proc_id;
	unsigned p;
	char msgbuf[256];
	int msgbufpos = 0;
	int vq_id;
	struct rte_qdma_queue_config q_config;

	if (start_port == end_port){
		printf("Lcore %u has nothing to do\n", id);
		return 0;
	}

	q_config.lcore_id = id;
	q_config.flags = RTE_QDMA_VQ_FD_LONG_FORMAT;
	if (qdma_mode == RTE_QDMA_MODE_HW)
		q_config.flags |= RTE_QDMA_VQ_EXCLUSIVE_PQ;
	q_config.rbp = NULL;

	vq_id = rte_qdma_queue_setup(qdma_dev_id, -1, &q_config);
	if (vq_id < 0) {
		printf("QDMA VQ creation failed\n");
		return 0;
	}

	/* build up message in msgbuf before printing to decrease likelihood
	 * of multi-core message interleaving.
	 */
	msgbufpos += snprintf(msgbuf, sizeof(msgbuf) - msgbufpos,
			"Lcore %u using ports ", id);
	for (p = start_port; p < end_port; p++){
		msgbufpos += snprintf(msgbuf + msgbufpos, sizeof(msgbuf) - msgbufpos,
				"%u ", (unsigned)ports[p]);
	}
	printf("%s\n", msgbuf);
	printf("lcore %u using queue %u of each port\n", id, (unsigned)q_id);

	/* handle packet I/O from the ports, reading and writing to the
	 * queue number corresponding to our process number (not lcore id)
	 */

	for (;;) {
		struct rte_mbuf *buf[RTE_QDMA_BURST_NB_MAX];

		for (p = start_port; p < end_port; p++) {
			const uint8_t src = ports[p];
			const uint8_t dst = ports[p ^ 1]; /* 0 <-> 1, 2 <-> 3 etc */

			/* Dequeue and forward any completed jobs */
			qdma_forward(vq_id, RTE_QDMA_BURST_NB_MAX);

			const uint16_t rx_c = rte_eth_rx_burst(src, q_id, buf, RTE_QDMA_BURST_NB_MAX);
			if (unlikely(signal_received)) {
				qdma_deinit();
				exit(0);
			}
			if (rx_c == 0)
				continue;
			pstats[src].rx += rx_c;
			qdma_copy(buf, dst, vq_id, rx_c);
		}
	}
}

/* Check the link status of all ports in up to 9s, and print them finally */
static void
check_all_ports_link_status(uint16_t port_num, uint32_t mask)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint16_t portid;
	uint8_t count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;

	printf("\nChecking link status");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		all_ports_up = 1;
		for (portid = 0; portid < port_num; portid++) {
			if ((mask & (1 << portid)) == 0)
				continue;
			memset(&link, 0, sizeof(link));
			rte_eth_link_get_nowait(portid, &link);
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf(
					"Port%d Link Up. Speed %u Mbps - %s\n",
						portid, link.link_speed,
				(link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
					("full-duplex") : ("half-duplex\n"));
				else
					printf("Port %d Link Down\n", portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == ETH_LINK_DOWN) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
			printf("done\n");
		}
	}
}

/* Main function.
 * Performs initialisation and then calls the lcore_main on each core
 * to do the packet-processing work.
 */
int
main(int argc, char **argv)
{
	static const char *_SMP_MBUF_POOL = "SMP_MBUF_POOL";
	int ret;
	unsigned i;
	enum rte_proc_type_t proc_type;
	struct rte_mempool *mp;

	/* set up signal handlers to print stats on exit */
	signal(SIGINT, print_stats);
	signal(SIGTERM, print_stats);

	/* initialise the EAL for all */
	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Cannot init EAL\n");
	argc -= ret;
	argv += ret;

	/* determine the NIC devices available */
	if (rte_eth_dev_count_avail() == 0)
		rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

	/* parse application arguments (those after the EAL ones) */
	smp_parse_args(argc, argv);

	proc_type = rte_eal_process_type();
	mp = (proc_type == RTE_PROC_SECONDARY) ?
			rte_mempool_lookup(_SMP_MBUF_POOL) :
			rte_pktmbuf_pool_create(_SMP_MBUF_POOL, NB_MBUFS,
				MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
				rte_socket_id());
	if (mp == NULL)
		rte_exit(EXIT_FAILURE, "Cannot get memory pool for buffers\n");

	if (num_ports & 1)
		rte_exit(EXIT_FAILURE, "Application must use an even number of ports\n");
	for(i = 0; i < num_ports; i++){
		if(proc_type == RTE_PROC_PRIMARY)
			if (smp_port_init(ports[i], mp, (uint16_t)num_procs) < 0)
				rte_exit(EXIT_FAILURE, "Error initialising ports\n");
	}

	if (proc_type == RTE_PROC_PRIMARY)
		check_all_ports_link_status(rte_eth_dev_count_avail(), port_mask);

	assign_ports_to_cores();

	ret = qdma_init();
	if (ret)
		rte_exit(EXIT_FAILURE, "Unable to initialize QDMA\n");

	RTE_LOG(INFO, APP, "Finished Process Init.\n");

	rte_eal_mp_remote_launch(lcore_main, NULL, CALL_MASTER);

	return 0;
}
