cmd_dpaa_fmc.o = /opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc -Wp,-MD,./.dpaa_fmc.o.d.tmp  -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa  -pthread -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_eal/linux/eal/include  -march=armv8-a+crc -mtune=cortex-a72 -mtls-dialect=trad -DRTE_MACHINE_CPUFLAG_NEON -DRTE_MACHINE_CPUFLAG_CRC32  -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include -DRTE_USE_FUNCTION_VERSIONING -include /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include/rte_config.h -D_GNU_SOURCE -O3 -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -Wno-error=pedantic -Wno-missing-field-initializers -Wimplicit-fallthrough=2 -Wno-format-truncation -Wno-pointer-arith -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa/ -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa/include -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa/fmlib -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/dpaa -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/dpaa/include/ -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/bus/dpaa/base/qbman -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/mempool/dpaa -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/common/dpaax -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/event/dpaa -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_eal/common/include -DALLOW_EXPERIMENTAL_API    -o dpaa_fmc.o -c /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/drivers/net/dpaa/dpaa_fmc.c 