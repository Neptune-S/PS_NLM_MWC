// ===========================================================================
//! @file            genGoldSeqNr.h
//! @brief           generate Gold sequence for LTE/NR standard
//! @copyright       Copyright (C) 2018 NXP Semiconductors
//!
//! The genGoldSeqNr.h header defines Gold sequence generateor
//!
// ===========================================================================

//#include <vspa/vspa.h>

#define GOLDSEQSEED1 0x5e485840

#ifndef __GENGOLDSEQNR_H__
#define __GENGOLDSEQNR_H__


// ---------------------------------------------------------------------------
//! @defgroup        GROUP_GENGOLDSEQNR
//!
//! The GENGOLDSEQNR module provides the following functions:
//!      - genGoldSeq():
//!      - shiftOffsetSeed():
//!      - genPbchDmrs():
//!      - genOffsetSeedPdschDmrs():
//!      - genPdschDmrs():
//!      - genOffsetSeedPdDmrs():
//!      - genPdDmrs():
//! 
//! @{
// ---------------------------------------------------------------------------


#ifndef __ASSEMBLER__
// ---------------------------------------------------------------------------
//! @brief           function to generate gold sequence according to LTE/5g std
//!
//! @param[out]	output pointer to the output vector (dmem vector aligned)
//! @param[in]	seeds  structure containing initial 2 seeds for gold sequence
//! @param[in]	Nbtis  integer indicating the number of bits to generate
//! @return     structure containing output 2 seeds at offset Nbits 
//! @cycle      49+floor(Nbits/32)  if Nbit<=320
//!
//! This function takes 2 initial seeds (contained in a structure) and outputs a gold 
//! sequence (QPSK squence with unit energy samples). 
//!
//! @attention	The minumum size of output is 320 bits (160 QPSK samples or 5 lines), 
//!             although the output state can be set to any offset. For example, one can
//!             set Nbits=100, the output seeds will be based on offset of 100 bits, while
//!             the output vector contains 320 bits (5 lines).
//! @attention  The MSB of output seeds (MSB of output.x0 and output.x0) are random:
//!             they can be bit 0 or 1. They are not used anyway.
//! @attention  If mod(Nbits, 64) = 0, this function will output an extra half line
// ---------------------------------------------------------------------------
extern struct gold_seq_seed_t genGoldSeq(
	vspa_complex_float16   *output,
	struct gold_seq_seed_t	seeds,
	size_t	 				Nbits
);

// ---------------------------------------------------------------------------
//! @brief           structure for seeds to the gold sequence generator
//!
//! The type defines the structure for the gold sequence generator
// ---------------------------------------------------------------------------
//struct gold_seq_seed_t {
//	uint32_t x0;
//	uint32_t x1;
//};

// ---------------------------------------------------------------------------
//! @brief           generate the seed information for LFSR at an offset
//!
//! @param[in]	p      ptr to the P matrix (31 words, may NOT be vector aligned)
//! @param[in]	seed   input seed information (31-bit integer)
//! @return     output seed information (31-bit integer)
//! @cycle      99
//!
//! This function takes input seed information and computes the seed state at an offset 
//! using a P matrix (calculated offline). See the IP for details 
//!	
// ---------------------------------------------------------------------------
extern uint32_t shiftOffsetSeed(
	uint32_t			*P,
	uint32_t            seed
);

// ---------------------------------------------------------------------------
//! @brief           function to generate PBCH DMRS sequences according to 5g std
//!
//! @param[in]	p          ptr to the P matrix (31 words, may NOT be vector aligned)
//! @param[out]	output     ptr to the output buffer (vector aligned)
//! @param[in]	Scratch    ptr to the Scratch buffer of size 5 lines
//! @param[in]	N_cell_id  integer of parameter N_cell_id
//! @param[in]	n_hf       integer of parameter n_hf
//! @param[in]	Nseq       number of output sequences
//! @return     void 
//! @cycle      1468 for Nseq = 8
//!
//! This function generates PBCH DMRS sequences based on the input parameters N_cell_id
//! and n_hf. It will generate Nseq sequences corresponding to i_SSB = 0 to Nseq-1.
//!
//! @attention	This function will generate Nseq DMRS sequences, each of which occupies
//!             5 DMEM lines (see IP for DMEM layout).
// ---------------------------------------------------------------------------
extern void genPbchDmrs(
	uint32_t			   *P,
	vspa_complex_float16   *output,
	vspa_complex_float16   *Scratch,
	uint32_t			    N_cell_id,
	size_t			 		n_hf,
	size_t			 		Nseq
);

// ---------------------------------------------------------------------------
//! @brief   function to create a table for 2nd seeds of Gold sequence generator in NR std
//!
//! @param[in]	N_cell_id  integer parameter of N_cell_id
//! @param[in]	n_SCID	   integer parameter of n_SCID
//! @return     void 
//! @cycle      12014
//!
//! This function generates a table of size 8x14 of 2nd seeds for Gold sequence generator at
//!	offset 1600. The (i,j)-th element of this table corresponds to the seed at i-th slot and
//! j-th symbol. Each element is a 31-bit integer stored as a word in DMEM. The table is stored
//! in a buffer created by this function.
// ---------------------------------------------------------------------------
extern void genOffsetSeedPdschDmrs(
	uint32_t			 	N_cell_id,
	uint32_t			 	n_SCID
);

// ---------------------------------------------------------------------------
//! @brief   function to create PDSCH DMRS sequences according to 5g std
//!
//! @param[out]	 output 	pointer to the output DMRS sequence (dmem vector aligned)
//! @param[in]	 l			integer parameter of symbol index
//! @param[in]	 ns			integer parameter of slot index
//! @param[in]	 Nbit		integer parameter of output sequence length in terms of bits
//! @return     void 
//! @cycle      Approx. 150-160 (depending on the output vector length)
//!
//! This function generates the PDSCH DMRS QPSK sequence for ns-th slot, l-th symbol.
//! The length of output sequence is given by Nbit in terms of bits.
//!
//! @attention  For Nbit <= 320, the output vector length will be 5 lines (Nbit=320).
//! @attention	This function outputs data in increment of half lines. So the actual
//!             output size is 32*ceil(Nbit/32)
//! @attention  If mod(Nbits, 64) = 0, this function will output an extra half line
// ---------------------------------------------------------------------------
extern void genPdschDmrs(
	vspa_complex_float16   *output,
	size_t			 		l,
	size_t 			 		ns,
	size_t			 		Nbit
);


// ---------------------------------------------------------------------------
//! @brief   function to create a table for 2nd seeds of Gold sequence generator in NR std
//!
//! @param[out]	TableOut   Ptr to output table for the 2nd seeds (vector aligned)
//! @param[in]	N_cell_id  integer parameter of N_cell_id
//! @param[in]	n_SCID	   integer parameter of n_SCID
//! @param[in]	n_s        integer parameter of value: 14*n_s,f + 1
//! @return     void 
//! @cycle      1500
//!
//! This function generates a table of size 1x14 of 2nd seeds for Gold sequence generator at
//!	offset 1600. The (i,j)-th element of this table corresponds to the seed at i-th slot and
//! j-th symbol. Each element is a 31-bit integer stored as a word in DMEM. 
// ---------------------------------------------------------------------------
extern void genOffsetSeedPdDmrs(
	uint32_t                TableOut[14],	
	uint32_t			 	N_cell_id,
	uint32_t			 	n_SCID,
	uint32_t                n_s
);

// ---------------------------------------------------------------------------
//! @brief   function to create PDSCH/PDCCH DMRS sequences according to 5g std
//!
//! @param[out]	 output 	pointer to the output DMRS sequence (dmem vector aligned)
//! @param[in]	 TableIn    Ptr to input table for the 2nd seeds (vector aligned)
//! @param[in]	 l			integer parameter of symbol index within the slot
//! @param[in]	 Nbit		integer parameter of output sequence length in terms of bits
//! @return     void 
//! @cycle      Approx. 150-160 (depending on the output vector length)
//!
//! This function generates the PDSCH/PDCCH DMRS QPSK sequence for ns-th slot, l-th symbol.
//! The length of output sequence is given by Nbit in terms of bits.
//!
//! @attention  For Nbit <= 320, the output vector length will be 5 lines (Nbit=320).
//! @attention	This function outputs data in increment of half lines. So the actual
//!             output size is 32*ceil(Nbit/32)
//! @attention  If mod(Nbits, 64) = 0, this function will output an extra half line
// ---------------------------------------------------------------------------
extern void genPdDmrs(
	vspa_complex_float16   *output,
	uint32_t                TableIn[14],	
	uint32_t                l,
	size_t			 		Nbit
);

// ---------------------------------------------------------------------------
//! @brief           function to descramble PBCH symbols
//!
//! @param[in]	pbchIn    pointer to input PBCH symbol (864 real HFLoat samples, dmem vector aligned)
//! @param[out]	pbchOut   pointer to output descrabled PBCH symbol (864 real HFLoat samples, dmem vector aligned)
//! @param[in]	seeds     structure containing initial 2 seeds for gold sequence
//! @return     structure containing output 2 seeds at offset 864 bits 
//! @cycle      42
//!
//! This function takes a PBCH qpsk sequence of 864 real samples (432 IQ samples), de-scramble
//! it with a gold sequence generated by 2 initial seeds (contained in a structure) and outputs
//! the descrambled sequence. It also outputs the updated seeds at offset 864 bits. 
//!
//! @attention	This function works for in-place input/output buffer. 
//! @attention  The MSB of output seeds (MSB of output.x0 and output.x1) are random:
//!             they can be bit 0 or 1. They are not used anyway.
// ---------------------------------------------------------------------------
extern struct gold_seq_seed_t qpskScrambleNr(
		cfloat16_t *pbchIn, 
		cfloat16_t *pbchOut,
		struct gold_seq_seed_t seeds);


#endif //__ASSEMBLER__


#endif // __GENGOLDSEQNR_H__

//! @} GROUP___GENGOLDSEQNR__
