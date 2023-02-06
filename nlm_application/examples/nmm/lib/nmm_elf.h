/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021 NXP
 */

#ifndef __NMM_ELF_H__
#define __NMM_ELF_H__

#define EI_NUM			0x10

struct elf_header {
	uint8_t  e_ident[EI_NUM];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint32_t e_entry;
	uint32_t e_phoff;
	uint32_t fh_shoff;
	uint32_t fh_flags;
	uint16_t fh_ehsize;
	uint16_t fh_phentsize;
	uint16_t fh_phnum;
	uint16_t fh_shentsize;
	uint16_t fh_shnum;
	uint16_t fh_shstrndx;
};
struct section_header {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
};

/* Section types reserved for processor-specific semantics */
#define SHT_LOPROC		0x70000000
#define SHT_HIPROC		0x7fffffff

#endif /* __NMM_ELF_H__ */
