//============================================================================
// Subroutine name: __start
//
// General Description: Start VSP and setup C environment.
//
// This function initializes the stack pointer and the control code switch,
// then jumps to main.
//
//============================================================================
// Copyright (C) 2013 Freescale Semiconductor, Inc.
//============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
//============================================================================
// Subroutines called by this subroutine:
//    (1)  NONE
//============================================================================
//
// Inputs:
//    Register inputs:
//       NONE
//    Memory inputs:
//       NONE
//    IP register inputs:
//       NONE
//    GP register inputs:
//       NONE
// Outputs:
//    Register outputs:
//       NONE
//    Memory outputs:
//       NONE
//    IP register outputs:
//       NONE
//    GP register outputs:
//       NONE
//
// Registers modified: NONE
//
// RAGs modified: NONE
//
// MAGs modified: as15
//
// Storage MAGs modified:  NONE
//
// Persistent memory:
//       NONE
// Scratch memory:
//       NONE
// No. of processing cycles:
//       min:     12.
//       max:     12.
//       typical: 12.
//
// Code size (PRAM/PROM lines): 9.
//
// Notes:
//
//============================================================================
	.section	.init,"ax",@progbits
	.global		_start
	.type		_start, @function
_start
	set.creg	22, 0;				/* Disable VSPA1 mode / enable half-word addressing */
    mv          sp, __stack;    // Initialize stack pointer.
    set.creg      4, 0;         // Disable condition code update.
    set.creg     12, 0;         // Disable H capture and increment.
    set.creg     15, 0;         // Disable fft_mode
    set.creg     16, 0;         // Reset fractional interpolation numerator.
    set.creg     17, 0;         // Reset fractional interpolation denominator
    set.creg     18, 0;         // Reset fractional interpolation phase.
    set.creg     20, 0;         // AU output lane [4095:0] in single-precision mode.
    set.creg    255, 0;         // Real mode, full data path, normal AU output.
    mv          a15, 0;        // To disable modulo addressing.
    jmp         _main;          // Jump to C entry-point.
    set.range   a2, a15, 0;    // Disable modulo addressing.
    set.range   a3, a15, 0;    // Disable modulo addressing.
    done;

	.size		_start, .-_start
FuncEnd_start

