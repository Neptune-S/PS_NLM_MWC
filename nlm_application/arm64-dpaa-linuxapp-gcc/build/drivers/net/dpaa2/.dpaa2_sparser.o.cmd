cmd_dpaa2_sparser.o = /opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc -Wp,-MD,./.dpaa2_sparser.o.d.tmp   -pthread -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_eal/linux/eal/include  -march=armv8-a+crc -mtune=cortex-a72 -mtls-dialect=trad -DRTE_MACHINE_CPUFLAG_NEON -DRTE_MACHINE_CPUFLAG_CRC32  -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include -DRTE_USE_FUNCTION_VERSIONING -include /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include/rte_config.h -D_GNU_SOURCE -O3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -Wno-error=pedantic -Wno-missing-field-initializers -Wimplicit-fallthrough=2 -Wno-format-truncation -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/common/dpaax -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa2 -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa2/mc -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/qbman/include -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/mc -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/portal -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/mempool/dpaa2 -DALLOW_EXPERIMENTAL_API    -o dpaa2_sparser.o -c /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa2/dpaa2_sparser.c 
