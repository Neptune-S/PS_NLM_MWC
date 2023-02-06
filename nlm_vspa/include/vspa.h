// =============================================================================
//! @file           vspa.h
//! @brief          VSPA core definitions.
//! @copyright      Copyright (C) 2013 Freescale Semiconductor, Inc.
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
#ifndef __VSPA_H__
#define __VSPA_H__

#if !defined( __ASSEMBLER__ )
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vspa/intrinsics.h>

typedef struct vspa2_complex_fixed16 {
    __fx16 real;
    __fx16 imag;
}   vspa2_complex_fixed16;  // CMPVSPA-1101

typedef struct vspa2_complex_float16 {
    __fp16 real;
    __fp16 imag;
}   vspa2_complex_float16;  // CMPVSPA-1101

#include "commonTypes.h"

#endif  // !defined( __ASSEMBLER__ )

#define __section(x)    __attribute__((section(x)))
#define __aligned(x)    __attribute__((aligned(x)))
#define __vec_aligned   __aligned(4*__AU_COUNT__)

#define SIZEOF8(x)      (sizeof(x) * 2)
#define SIZEOF16(x)     (sizeof(x) * 1)
#define SIZEOF32(x)     (sizeof(x) / 2)
#define SIZEOFVEC(x)    ((sizeof(x) + (4 * __AU_COUNT__) - 1) / (4 * __AU_COUNT__))

//! @defgroup       GROUP_VSPA Vector Signal Processing Acceleration
//! @{

// =============================================================================
//! @defgroup       GROUP_VSPA_C99 Compiler wrappers
//! @{
// =============================================================================

#if !defined( __ASSEMBLER__ )
// -----------------------------------------------------------------------------
//! @brief          Emit an assembly instruction.
//! @return         void.
//! @par            Example 1: ASM( "fnop;" );
//! @par            Example 2: ASM( "set c_reg %0, %1;" : : "n" (creg), "n" (data) );
// -----------------------------------------------------------------------------
#define ASM( ... )        __asm__ volatile ( __VA_ARGS__ )

// -----------------------------------------------------------------------------
//! @brief          Discard warning about an unused variable.
//! @param[in]      x variable name that is unused.
//! @return         void.
//! @note           This macro does not generate code instructions.
// -----------------------------------------------------------------------------
#define UNUSED( x )     (void)(sizeof(x))

#endif // !defined( __ASSEMBLER__ )
//! @}


#if !defined( __ASSEMBLER__ )

#pragma cplusplus on
// -----------------------------------------------------------------------------
//! @brief          Write to an IP register.
//! @param[in]      reg     specifies the IP register to write to.
//! @param[in]      data    specifies the data to write to the IP register.
//! @return         void.
//! @see            ipWriteField()
// -----------------------------------------------------------------------------
static inline void __ipwr(uint32_t const reg, uint32_t data)
{
    __ip_write(reg, 0xFFFFFFFF, data); // TBD TECH - CW 10.2.4
}

// -----------------------------------------------------------------------------
//! @brief          Modify a field in an IP register.
//! @param[in]      reg     specifies the IP register to modify.
//! @param[in]      mask    specifies the field to modify.
//! @param[in]      data    specifies the data to write to the field.
//! @return         void.
//! @see            ipWriteReg()
// -----------------------------------------------------------------------------
static inline void __ipwr(uint32_t const reg, uint32_t mask, uint32_t data)
{
    __ip_write(reg, mask, data);
}

// -----------------------------------------------------------------------------
//! @brief          Read an IP register.
//! @param[in]      reg     specifies the IP register to read from.
//! @return         the value of the IP register.
//! @see            ipReadField()
// -----------------------------------------------------------------------------
static inline uint32_t __iprd(uint32_t const reg)
{
    return __ip_read(reg, 0xFFFFFFFF); // TBD TECH - CW 10.2.4
}

// -----------------------------------------------------------------------------
//! @brief          Read a field from an IP register.
//! @param[in]      reg     specifies the IP register to read from.
//! @param[in]      mask    specifies the field to read.
//! @return         the value of the IP register field.
//! @see            ipReadReg()
// -----------------------------------------------------------------------------
static inline uint32_t __iprd(uint32_t const reg, uint32_t mask)
{
    return __ip_read(reg, mask);
}
#pragma cplusplus reset

//! @}

// =============================================================================
//! @defgroup       GROUP_VSPA_ASM Assembly instructions
//! @{
// =============================================================================

// -----------------------------------------------------------------------------
//! @brief          Put the core in low power mode.
//! @return         void.
//! @see            __idle()
//! @note           Invoking this function on the simulator will stop
//!                 the simulation.
//! @note           Invoking this function on the hardware will put the core
//!                 in low power mode, waiting for a go event. In that case,
//!                 this function is equivalent to __idle().
// -----------------------------------------------------------------------------
#define __halt()    __builtin_done()

// -----------------------------------------------------------------------------
//! @brief          Insert a software breakpoint.
//! @return         void.
// -----------------------------------------------------------------------------
#define __bkpt()    __swbreak()

// -----------------------------------------------------------------------------
//! @brief          Insert a no-operation instruction.
//! @return         void.
// -----------------------------------------------------------------------------
#define __nop()     ASM( "fnop .asmvol" )

// -----------------------------------------------------------------------------
//! @brief          Put the core in low power mode, waiting for a go event.
//! @return         void.
// -----------------------------------------------------------------------------
__attribute__(( noreturn ))
static inline void __idle(
    void )
{
    __builtin_done();
}

//! @}

// =============================================================================
//! @defgroup       GROUP_VSPA_MISC Miscellaneous utilities
//! @{
// =============================================================================

// -----------------------------------------------------------------------------
//! @brief          Minimum value.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         minimum value between a and b.
//! @see            MAX()
// -----------------------------------------------------------------------------
#define MIN( a , b )	((a) < (b) ? (a) : (b))

// -----------------------------------------------------------------------------
//! @brief          Maximum value.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         maximum value between a and b.
//! @see            MIN()
// -----------------------------------------------------------------------------
#define MAX( a, b )	    ((a) > (b) ? (a) : (b))

// -----------------------------------------------------------------------------
//! @brief          Minimum value between two 32-bit signed integers.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         minimum value between a and b.
//! @see            max_int32()
// -----------------------------------------------------------------------------
static inline int min_int32(
    int a,
    int b )
{
    return MIN( a, b );
}

// -----------------------------------------------------------------------------
//! @brief          Maximum value between two 32-bit signed integers.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         minimum value between a and b.
//! @see            min_int32().
// -----------------------------------------------------------------------------
static inline int max_int32(
    int a,
    int b )
{
    return MAX( a, b );
}

// -----------------------------------------------------------------------------
//! @brief          Minimum value between two 32-bit unsigned integers.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         minimum value between a and b.
//! @see            max_uint32()
// -----------------------------------------------------------------------------
static inline unsigned int min_uint32(
    unsigned int a,
    unsigned int b )
{
    return MIN( a, b );
}

// -----------------------------------------------------------------------------
//! @brief          Maximum value between two 32-bit unsigned integers.
//! @param[in]      a value a.
//! @param[in]      b value b.
//! @return         minimum value between a and b.
//! @see            min_uint32().
// -----------------------------------------------------------------------------
static inline unsigned int max_uint32(
    unsigned int a,
    unsigned int b )
{
    return MAX( a, b );
}

// -----------------------------------------------------------------------------
//! @brief      Modulo increment operation on an unsigned 32-bit integer
//! @param      base Unsigned 32-bit number.
//! @param      incr Increment.
//! @param      size Modulo size.
//! @return     modulo incremented base.
// -----------------------------------------------------------------------------
static inline uint32_t __inc_mod(uint32_t base, int incr, size_t size)
{
    base += incr;
    base -= base >= size ? size : 0;
    return base;
}

// -----------------------------------------------------------------------------
//! @brief      Copy one or more vectors.
//! @param      d Destination address, vector-aligned.
//! @param      s Source address, vector-aligned.
//! @param      n Number of vectors to copy.
//! @return     This function does not return a value.
// -----------------------------------------------------------------------------
extern void veccpy(void *d, void const *s, size_t n);

// -----------------------------------------------------------------------------
//! @brief      ATID value.
//!
//! A VSPA instance is assigned a unique identifier. VSPA can discover its own
//! unique identifier by reading the ATID value in the PARAM0 register.
//!
//! @return     The VSPA instance unique identifier.
// -----------------------------------------------------------------------------
static inline uint32_t atid(void)
{
    return __iprd(0x40 >> 2, 0x7F);
}

/**
 * @brief       Absolute value of a 2's complement integer.
 *
 * @param       x Real scalar integer.
 * @return      The absolute value of x.
 * @see         abs()
 */
/*static inline int __abs(int x)
{
    unsigned int r;
    __asm volatile ("abs %0, %1" : "=g" (r) : "g" (x));
    return r;
}*/

/**
 * @brief       Absolute value of a 2's complement integer.
 *
 * @param       x Real scalar integer.
 * @return      The absolute value of x.
 * @see         __abs()
 */
static inline int abs(int x)
{
    unsigned int r;
    __asm volatile ("abs %0, %1" : "=g" (r) : "g" (x));
    return r;
}

/**
 * @brief       SP float -> 32bit integer conversion
 *
 * @param       f SP floating point scalar.
 * @return      32bit integer scalar value of f.
 * @see         __float2fix()
 */
/*static inline int32_t __float2fix(float f) {
    int32_t i32;
    __asm volatile ("float2fix %0, %1" : "=g" (i32) : "f32" (f));
    return i32;
}*/

/**
 * @brief       Cast a 32-bit unsigned integer to a single precision
 *              floating point number.
 * @param       u32 The 32-bit unsigned integer.
 * @return      The same value interpreted as a single precision floating point
 *              number.
 */
static inline float __cast_float32(uint32_t u32)
{
    return *((float *) &u32);
}

#pragma cplusplus on
/**
 * @brief       Cast a single precision floating point number to a 32-bit
 *              unsigned integer.
 * @param       f32 The single precision floating point number.
 * @return      The same value interpreted as unsigned integer.
 */
static inline uint32_t __cast_uint32(float f32)
{
    return *((uint32_t *) &f32);
}

/**
 * @brief       Cast a complex half precision floating point number to a 32-bit
 *              unsigned integer.
 * @param       f32 The complex half precision floating point number.
 * @return      The same value interpreted as unsigned integer.
 */
/*
static inline uint32_t __cast_uint32(cfloat16_t cf16)
{
    return *((uint32_t *) &cf16);
}
*/
#pragma cplusplus reset

/**
 * @brief       Cast a single precision floating point number to a 32-bit
 *              signed integer.
 * @param       f32 The single precision floating point number.
 * @return      The same value interpreted as signed integer.
 */
static inline int32_t __cast_int32(float f32)
{
    return *((int32_t *) &f32);
}

/**
 * @brief       Cast a 32-bit unsigned integer to a complex half precision
 *              floating point number.
 * @param       u32 The 32-bit unsigned integer.
 * @return      The same value interpreted as a complex half precision
 *              floating point number.
 */
/*
static inline cfloat16_t __cast_cfloat16(uint32_t u32)
{
    return *((cfloat16_t *) &u32);
} */

#endif // !defined( __ASSEMBLER__ )
//! @}

//! @}
#endif // __VSPA_H__
