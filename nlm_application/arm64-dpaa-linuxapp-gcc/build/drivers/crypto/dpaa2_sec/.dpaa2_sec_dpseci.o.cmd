cmd_dpaa2_sec_dpseci.o = /opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc -Wp,-MD,./.dpaa2_sec_dpseci.o.d.tmp   -pthread -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_eal/linux/eal/include  -march=armv8-a+crc -mtune=cortex-a72 -mtls-dialect=trad -DRTE_MACHINE_CPUFLAG_NEON -DRTE_MACHINE_CPUFLAG_CRC32  -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include -DRTE_USE_FUNCTION_VERSIONING -include /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include/rte_config.h -D_GNU_SOURCE -DALLOW_EXPERIMENTAL_API -O3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -Wno-error=pedantic -Wno-missing-field-initializers -Wimplicit-fallthrough=2 -Wno-format-truncation -Wno-implicit-fallthrough -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/common/dpaax -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/common/dpaax/caamflib -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/crypto/dpaa2_sec/ -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/crypto/dpaa2_sec/mc -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/ -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/qbman/include -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/mc -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/fslmc/portal -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/mempool/dpaa2/    -o dpaa2_sec_dpseci.o -c /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/crypto/dpaa2_sec/dpaa2_sec_dpseci.c 