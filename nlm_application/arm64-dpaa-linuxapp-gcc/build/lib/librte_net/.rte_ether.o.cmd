cmd_rte_ether.o = /opt/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc -Wp,-MD,./.rte_ether.o.d.tmp   -pthread -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_eal/linux/eal/include  -march=armv8-a+crc -mtune=cortex-a72 -mtls-dialect=trad -DRTE_MACHINE_CPUFLAG_NEON -DRTE_MACHINE_CPUFLAG_CRC32  -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include -DRTE_USE_FUNCTION_VERSIONING -include /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/arm64-dpaa-linuxapp-gcc/include/rte_config.h -D_GNU_SOURCE -DALLOW_EXPERIMENTAL_API -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral -Wformat-security -Wundef -Wwrite-strings -Wdeprecated -Wno-error=pedantic -Wno-missing-field-initializers -Wimplicit-fallthrough=2 -Wno-format-truncation -I/home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_net -O3    -o rte_ether.o -c /home/deepika/30_ddr_sit/PS_NLM_5G_LAT/nlm_application/lib/librte_net/rte_ether.c 
