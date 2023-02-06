// =============================================================================
//! @file           vspa2_fw.h
//! @brief          This header file defines macros to enable forward 
//!                 compatibility of VSPA2 software with VSPA3.
//! @copyright      Copyright 2017 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */

#ifndef __VSPA2_FW__
#define __VSPA2_FW__


//#define SIZEVEC8          (__AU_COUNT__ * 8)        					// Number of bytes per vector.
//#define SIZEVEC16         (__AU_COUNT__ * 4)        					// Number of half-words per vector.
//#define SIZEVEC32         (__AU_COUNT__ * 2)        					// Number of words per vector.
//#define SIZEVEC64         (__AU_COUNT__ * 1)        					// Number of double-words per vector.


#if defined(__VSPA2__)

//#define SIZEOF8(x)        ((sizeof(x) + 0) << 1)    					// Number of bytes.
//#define SIZEOF16(x)       ((sizeof(x) + 0) >> 0)    					// Number of half-words.
//#define SIZEOF32(x)       ((sizeof(x) + 1) >> 1)    					// Number of words.
#define ADDR8(x)          (((uint32_t)(x)) << 1)    					// Byte address.
#define ADDR16(x)         (((uint32_t)(x)) >> 0)    					// Half-word address.
#define ADDR32(x)         (((uint32_t)(x)) >> 1)    					// Word address.
#define __align_vec       __attribute__((aligned(SIZEVEC16)))           // Align to vector-boundary.
#define __align_vspa      __attribute__((aligned(AXI_WIDTH_VSPA / 16))) // Align to VSPA AXI-boundary.
#define __align_feca      __attribute__((aligned(AXI_WIDTH_FECA / 16))) // Align to FECA AXI-boundary.
#define UPHW              1                         					// Minimum addressable units per half-word.

#elif defined (__VSPA3__)

#define SIZEOF8(x)        ((sizeof(x) + 0) << 0)    					// Number of bytes.
#define SIZEOF16(x)       ((sizeof(x) + 1) >> 1)    					// Number of half-words.
#define SIZEOF32(x)       ((sizeof(x) + 3) >> 2)    					// Number of words.
#define ADDR8(x)          (((uint32_t)(x)) << 0)    					// Byte address.
#define ADDR16(x)         (((uint32_t)(x)) >> 1)    					// Half-word address.
#define ADDR32(x)         (((uint32_t)(x)) >> 2)    					// Word address.
#define __align_vec       __attribute__((aligned(SIZEVEC8)))            // Align to vector-boundary.
#define __align_vspa      __attribute__((aligned(AXI_WIDTH_VSPA / 8)))  // Align to VSPA AXI-boundary.
#define __align_feca      __attribute__((aligned(AXI_WIDTH_FECA / 8)))  // Align to FECA AXI-boundary.
#define UPHW              2                         					// Minimum addressable units per half-word.

#else
#error "VSPA architecture not supported."
#endif

#endif // __VSPA2_FW__
