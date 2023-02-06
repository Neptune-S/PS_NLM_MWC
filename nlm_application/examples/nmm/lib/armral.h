/*
    Arm RAN Acceleration Library
    Copyright 2020-2022 Arm Limited (or its affiliates).
    All rights reserved.
*/
#pragma once

/**
 * @defgroup groupVector Vector functions
 *
 * \brief Functions for working with vectors.
 *
 * Functions are provided for working with arrays of 16-bit integers
 * (Q15 format) and 32-bit floating-point numbers. In particular:
 * + Vector elementwise multiplication (vector multiply)
 * + Vector dot product
 */

/**
 * @defgroup groupMatrix Matrix functions
 *
 * \brief Functions for working with matrices.
 *
 * Functions are provided for working with matrices, including:
 * + Matrix-vector multiplication for 16-bit integer datatypes.
 * + Matrix-matrix multiplication. Supports both 16-bit integer and 32-bit
 * floating-point datatypes. In addition, the `solve` routines
 * support specifying a custom Q-format specifier for both input and output
 * matrices, instead of assuming that the input is in Q15 format.
 * + Matrix inversion. Supports the 32-bit floating-point datatype.
 */

/**
 * @defgroup groupLowPhy Lower PHY support functions
 *
 * \brief Functions for working in the lower physical layer (lower PHY).
 *
 * The Lower PHY functions include support for:
 * + A Gold Sequence generator
 * + A correlation coefficient of a pair of 16-bit integer arrays (in Q15
 * format)
 * + FIR filters. Supports both 16-bit integer and 32-bit floating-point
 * datatypes. Support is provided for decimation factors of both one and two.
 * + Fast Fourier Transforms (FFTs). Supports both 16-bit integer and 32-bit
 * floating-point datatypes.
 */

/**
 * @defgroup groupUpPhy Upper PHY support functions
 *
 * \brief Functions for working in the upper physical layer (upper PHY).
 *
 * The Upper PHY functions include support for:
 * + Digital modulation and demodulation, using QPSK, 16QAM, 64QAM, or 256QAM.
 * + Cyclic Redundancy Check (CRC), both little-endian and big-endian, for the
     six 5G polynomials (CRC24A, CRC24B, CRC24C, CRC16, CRC11, and CRC6).
 * + Polar encoding and decoding.
 * + Low-Density Parity Check (LDPC) encoding and decoding.
 * + LTE Turbo encoding and decoding.
 * + Rate matching and rate recovery for Polar coding.
 * + Rate matching and rate recovery for LDPC coding.
 */

/**
 * @defgroup groupDuRuInterface DU-RU IF support functions
 *
 * \brief Functions for working with Distributed Units (DUs) and Radio Units
 * (RUs).
 *
 * The DU-RU IF functions include support for:
 * + Mu-Law compression and decompression, in 8-bit, 9-bit, and 14-bit formats.
 * + Block floating-point compression and decompression, in 8-bit, 9-bit, and
 *   14-bit formats.
 * + Block scaling compression and decompression, in 8-bit, 9-bit, and 14-bit
 *   formats.
 */

// GCC sometimes complains about use of uninitialized values in arm_neon.h.
// nothing we can do about that, so ignore it!
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include <arm_neon.h>

// Restore original warning flags.
#ifndef __clang__
#pragma GCC diagnostic pop
#endif

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error status returned by functions in the library.
 */
typedef enum {
  ARMRAL_SUCCESS = 0,         ///< No error.
  ARMRAL_ARGUMENT_ERROR = -1, ///< One or more arguments are incorrect.
} armral_status;

/**
 * Formats that are supported by modulation and demodulation.
 * See \link armral_modulation \endlink and \link armral_demodulation \endlink.
 */
typedef enum {
  ARMRAL_MOD_QPSK = 0,  ///< QPSK, size 4 constellation, 2 bits per symbol.
  ARMRAL_MOD_16QAM = 1, ///< 16QAM, size 16 constellation, 4 bits per symbol.
  ARMRAL_MOD_64QAM = 2, ///< 64QAM, size 64 constellation, 6 bits per symbol.
  ARMRAL_MOD_256QAM = 3 ///< 256QAM, size 256 constellation, 8 bits per symbol.
} armral_modulation_type;

/**
 * Fixed-point format index `Q[integer_bits, fractional_bits]` for `int16_t`.
 * For usage information, see the `armral_solve_*` functions.
 */
typedef enum {
  /// 1 sign bit, 0 integer bits, 15 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q15 = 15,
  /// 1 sign bit, 1 integer bit, 14 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q1_14 = 14,
  /// 1 sign bit, 2 integer bits, 13 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q2_13 = 13,
  /// 1 sign bit, 3 integer bits, 12 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q3_12 = 12,
  /// 1 sign bit, 4 integer bits, 11 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q4_11 = 11,
  /// 1 sign bit, 5 integer bits, 10 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q5_10 = 10,
  /// 1 sign bit, 6 integer bits, 9 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q6_9 = 9,
  /// 1 sign bit, 7 integer bits, 8 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q7_8 = 8,
  /// 1 sign bit, 8 integer bits, 7 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q8_7 = 7,
  /// 1 sign bit, 9 integer bits, 6 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q9_6 = 6,
  /// 1 sign bit, 10 integer bits, 5 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q10_5 = 5,
  /// 1 sign bit, 11 integer bits, 4 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q11_4 = 4,
  /// 1 sign bit, 12 integer bits, 3 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q12_3 = 3,
  /// 1 sign bit, 13 integer bits, 2 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q13_2 = 2,
  /// 1 sign bit, 14 integer bits, 1 fractional bit.
  ARMRAL_FIXED_POINT_INDEX_Q14_1 = 1,
  /// 1 sign bit, 15 integer bits, 0 fractional bits.
  ARMRAL_FIXED_POINT_INDEX_Q15_0 = 0
} armral_fixed_point_index;

/**
 * Defines the values that can be stored in the output `frozen` mask that is
 * created by \link armral_polar_frozen_mask \endlink. For a given input bit
 * array, each index `i` in the `frozen` mask describes the corresponding bit
 * index `i` in the array. Each entry describes the origin of the bit at the
 * point of output from \link armral_polar_encode_block \endlink, in particular
 * whether the origin of the bit was an information bit (present in the
 * original codeword), a parity bit (calculated from the codeword bits), or a
 * `frozen` bit (set to zero).
 */
typedef enum {
  ARMRAL_POLAR_INFO_BIT = 0,    ///< Information bit.
  ARMRAL_POLAR_PARITY_BIT = 1,  ///< Parity bit.
  ARMRAL_POLAR_FROZEN_BIT = 255 ///< Frozen bit (set to zero).
} armral_polar_frozen_bit_type;

/**
 * \brief 16-bit signed integer complex data type.
 */
typedef struct {
  int16_t re; ///< 16-bit real component.
  int16_t im; ///< 16-bit imaginary component.
} armral_cmplx_int16_t;

/**
 * \brief 32-bit floating-point complex data type.
 */
typedef struct {
  float re; ///< 32-bit real component.
  float im; ///< 32-bit imaginary component.
} armral_cmplx_f32_t;

/**
 * The number of complex samples in each compressed block.
 */
#define ARMRAL_NUM_COMPLEX_SAMPLES 12

/**
 * \brief The structure for an 8-bit compressed block.
 *
 * See \link armral_block_float_compr_8bit \endlink and \link
 * armral_block_float_decompr_8bit \endlink.
 */
typedef struct {
  int8_t exp;          ///< Block exponent, in the range 0-8 (inclusive).
  int8_t mantissa[24]; ///< Packed data, 8 bits per element.
} armral_compressed_data_8bit;

/**
 * \brief The structure for a 9-bit compressed block.
 *
 * See \link armral_block_float_compr_9bit \endlink and \link
 * armral_block_float_decompr_9bit \endlink.
 */
typedef struct {
  int8_t exp;          ///< Block exponent, in the range 0-7 (inclusive).
  int8_t mantissa[27]; ///< Packed data, 9 bits per element.
} armral_compressed_data_9bit;

/**
 * \brief The structure for a 12-bit compressed block.
 *
 * See \link armral_block_float_compr_12bit \endlink and \link
 * armral_block_float_decompr_12bit \endlink.
 */
typedef struct {
  int8_t exp;          ///< Block exponent, in the range 0-4 (inclusive).
  int8_t mantissa[36]; ///< Packed data, 12 bits per element.
} armral_compressed_data_12bit;

/**
 * \brief The structure for a 14-bit compressed block.
 *
 * See \link armral_block_float_compr_14bit \endlink and \link
 * armral_block_float_decompr_14bit \endlink.
 */
typedef struct {
  int8_t exp;          ///< Block exponent, in the range 0-2 (inclusive).
  int8_t mantissa[42]; ///< Packed data, 14 bits per element.
} armral_compressed_data_14bit;

/**
 * @ingroup groupVector
 */
/**
 * @addtogroup cmplx_by_cmplx_mult Vector Multiply
 * @{
 * \brief Multiplies a complex vector by another complex vector and generates a
 * complex result.
 *
 * The complex arrays have a total of `2*n` real values.<br> The
 * vector multiplication algorithm is:
 * <pre>
 * for (n = 0; n < numSamples; n++) {
 *     pDst[2n+0] = pSrcA[2n+0] * pSrcB[2n+0] - pSrcA[2n+1] * pSrcB[2n+1];
 *     pDst[2n+1] = pSrcA[2n+0] * pSrcB[2n+1] + pSrcA[2n+1] * pSrcB[2n+0];
 * }
 * </pre>
 */
/**
 * This algorithm performs the element-wise complex multiplication between two
 * complex input sequences, `A` and `B`, of the same length, (`N`).<br>
 * The implementation uses saturating arithmetic. Intermediate operations are
 * performed on 32-bit variables in Q31 format. To convert the final result back
 * into Q15 format, the final result is right-shifted and narrowed to 16 bits.
 *
 * <pre>
 *  C[n] = A[n] * B[n], where 0 ≤ n < N-1
 * </pre>
 *
 * where:
 *
 * <pre>
 *  Re{C[n]} = Re{A[n]}*Re{B[n]} - Im{A[n]}*Im{B[n]}
 *  Im{C[n]} = Re{A[n]}*Im{B[n]} + Im{A[n]}*Re{B[n]}
 * </pre>
 *
 * Both input and output arrays populate with int16_t elements in Q15
 * format, with interleaved real and imaginary components:
 *
 * <pre>
 *  x = {x[0], x[1], ..., x[N-1]}
 * </pre>
 *
 * where:
 *
 * <pre>
 *  x[i] = (Re(x[i]), Im(x[i])), 0 ≤ i < N
 * </pre>
 *
 * @param[in]     n       The number of samples in each vector.
 * @param[in]     a       Points to the first input vector.
 * @param[in]     b       Points to the second input vector.
 * @param[out]    c       Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecmul_i16(int32_t n, const armral_cmplx_int16_t *a,
                                      const armral_cmplx_int16_t *b,
                                      armral_cmplx_int16_t *c);

/**
 * This algorithm performs the element-wise complex multiplication between two
 * complex [I and Q separated] input sequences, `A` and `B`, of the same length
 * (`N`).<br>
 * The implementation uses saturating arithmetic. Intermediate operations are
 * performed on 32-bit variables in Q31 format. To convert the final result back
 * into Q15 format, the final result is right-shifted and narrowed to 16 bits.
 *
 * <pre>
 *  C[n] = A[n] * B[n], where 0 ≤ n < N-1
 * </pre>
 *
 *  where:
 *
 * <pre>
 *  Re{C[n]} = Re{A[n]}*Re{B[n]} - Im{A[n]}*Im{B[n]}
 *  Im{C[n]} = Re{A[n]}*Im{B[n]} + Im{A[n]}*Re{B[n]}
 * </pre>
 *
 * Both input and output arrays populate with int16_t elements in Q15 format,
 * with separate arrays for real and imaginary components:
 *
 * <pre>
 *  Re(x) = {Re(x[0]), Re(x[1]), ..., Re(x[N-1])}
 *  Im(x) = {Im(x[0]), Im(x[1]), ..., Im(x[N-1])}
 * </pre>
 *
 * @param[in]  n     The number of samples in each vector.
 * @param[in]  a_re  Points to the real part of the first input vector.
 * @param[in]  a_im  Points to the imaginary part of the first input vector.
 * @param[in]  b_re  Points to the real part of the second input vector.
 * @param[in]  b_im  Points to the imaginary part of the second input vector.
 * @param[out] c_re  Points to the real part of the output result.
 * @param[out] c_im  Points to the imaginary part of the output result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecmul_i16_2(int32_t n, const int16_t *a_re,
                                        const int16_t *a_im,
                                        const int16_t *b_re,
                                        const int16_t *b_im, int16_t *c_re,
                                        int16_t *c_im);

/**
 * This algorithm performs the element-wise complex multiplication between two
 * complex input sequences, `A` and `B`, of the same length (`N`).
 *
 * <pre>
 *  C[n] = A[n] * B[n], where 0 ≤ n < N-1
 * </pre>
 *
 * where:
 *
 * <pre>
 *  Re{C[n]} = Re{A[n]}*Re{B[n]} - Im{A[n]}*Im{B[n]}
 *  Im{C[n]} = Re{A[n]}*Im{B[n]} + Im{A[n]}*Re{B[n]}
 * </pre>
 *
 * Both input and output arrays populate with 32-bit float elements, with
 * interleaved real and imaginary components:
 *
 * <pre>
 *  x = {x[0], x[1], ..., x[N-1]}
 * </pre>
 *
 * where:
 *
 * <pre>
 *  x[i] = (Re(x[i]), Im(x[i])), 0 ≤ i < N
 * </pre>
 *
 * @param[in]     n       The number of samples in each vector.
 * @param[in]     a       Points to the first input vector.
 * @param[in]     b       Points to the second input vector.
 * @param[out]    c       Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecmul_f32(int32_t n, const armral_cmplx_f32_t *a,
                                      const armral_cmplx_f32_t *b,
                                      armral_cmplx_f32_t *c);

/**
 * This algorithm performs the element-wise complex multiplication between two
 * complex [I and Q separated] input sequences, `A` and `B`, of the same length
 * (`N`).
 *
 * <pre>
 *  C[n] = A[n] * B[n], where 0 ≤ n < N-1
 * </pre>
 *
 * where:
 *
 * <pre>
 *  Re{C[n]} = Re{A[n]}*Re{B[n]} - Im{A[n]}*Im{B[n]}
 *  Im{C[n]} = Re{A[n]}*Im{B[n]} + Im{A[n]}*Re{B[n]}
 * </pre>
 *
 * Both input and output arrays populate with 32-bit float elements, with
 * separate arrays for real and imaginary components:
 *
 * <pre>
 *  Re(x) = {Re(x[0]), Re(x[1]), ..., Re(x[N-1])}
 *  Im(x) = {Im(x[0]), Im(x[1]), ..., Im(x[N-1])}
 * </pre>
 *
 * @param[in]  n     The number of samples in each vector.
 * @param[in]  a_re  Points to the real part of the first input vector.
 * @param[in]  a_im  Points to the imaginary part of the first input vector.
 * @param[in]  b_re  Points to the real part of the second input vector.
 * @param[in]  b_im  Points to the imaginary part of the second input vector.
 * @param[out] c_re  Points to the real part of the output result.
 * @param[out] c_im  Points to the imaginary part of the output result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecmul_f32_2(int32_t n, const float *a_re,
                                        const float *a_im, const float *b_re,
                                        const float *b_im, float *c_re,
                                        float *c_im);

/** @} end of cmplx_by_cmplx_mult group */

/**
 * @ingroup groupVector
 */
/**
 * @addtogroup cmplx_dot_prod Vector Dot Product
 * @{
 * \brief Computes the dot product of two complex vectors.
 *
 * The vectors are multiplied element-by-element and then summed.
 *
 * `pSrcA` points to the first complex input vector and
 * `pSrcB` points to the second complex input vector.
 * `n` specifies the number of complex samples. The data in each array is
 * stored as \link armral_cmplx_f32_t \endlink elements, with separate arrays
 * for real and imaginary components:
 * <pre>
 *  (real, imag, real, imag, ...)
 * </pre>
 * Each array has a total of `n` complex values.
 *
 * The dot product algorithm is:
 * <pre>
 *  real_result = 0;
 *  imag_result = 0;
 *  for (n = 0; n < numSamples; n++) {
 *     real_result += p_src_a[2n+0]*p_src_b[2n+0] - p_src_a[2n+1]*p_src_b[2n+1];
 *     imag_result += p_src_a[2n+0]*p_src_b[2n+1] + p_src_a[2n+1]*p_src_b[2n+0];
 *  }
 * </pre>
 */
/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed. Array
 * elements are assumed to be complex float32 and with interleaved real and
 * imaginary parts.
 *
 * @param[in]     n           The number of samples in each vector.
 * @param[in]     p_src_a     Points to the first complex input vector.
 * @param[in]     p_src_b     Points to the second complex input vector.
 * @param[out]    p_src_c     Points to the output complex vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_f32(int32_t n,
                                      const armral_cmplx_f32_t *p_src_a,
                                      const armral_cmplx_f32_t *p_src_b,
                                      armral_cmplx_f32_t *p_src_c);

/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed. Array
 * elements are assumed to be 32-bit floats, and separate arrays are used for
 * the real and imaginary parts of the input data.
 *
 * @param[in]  n           The number of samples in each vector.
 * @param[in]  p_src_a_re  Points to the real part of the first input vector.
 * @param[in]  p_src_a_im  Points to the imaginary part of the first input
 *                         vector.
 * @param[in]  p_src_b_re  Points to the real part of the second input vector.
 * @param[in]  p_src_b_im  Points to the imaginary part of the second input
 *                         vector.
 * @param[out] p_src_c_re  Points to the real part of the output result.
 * @param[out] p_src_c_im  Points to the imaginary part of the output result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_f32_2(int32_t n, const float *p_src_a_re,
                                        const float *p_src_a_im,
                                        const float *p_src_b_re,
                                        const float *p_src_b_im,
                                        float *p_src_c_re, float *p_src_c_im);

/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed. Array
 * elements are assumed to be complex int16 in Q15 format and interleaved. <br>
 * To avoid overflow issues input values are internally extended to 32-bit
 * variables and all intermediate calculations results are stored in 64-bit
 * internal variables. To get the final result in Q15 and to avoid overflow,
 * the accumulator narrows to 16 bits with saturation.
 *
 * @param[in]     p_src_a     Points to the first input vector.
 * @param[in]     p_src_b     Points to the second input vector.
 * @param[in]     n           The number of samples in each vector.
 * @param[out]    p_src_c     Points to the output complex result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_i16(int32_t n,
                                      const armral_cmplx_int16_t *p_src_a,
                                      const armral_cmplx_int16_t *p_src_b,
                                      armral_cmplx_int16_t *p_src_c);

/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed. Array
 * elements are assumed to be int16 in Q15 format and separate arrays are used
 * for real parts and imaginary parts of the input data.<br>
 * To avoid overflow issues input values are internally extended to 32-bit
 * variables and all intermediate calculations results are stored in 64-bit
 * internal variables. To get the final result in Q15 and to avoid overflow,
 * the accumulator narrows to 16 bits with saturation.
 *
 * @param[in]  p_src_a_re  Points to the real part of first input vector.
 * @param[in]  p_src_a_im  Points to the imag part of first input vector.
 * @param[in]  p_src_b_re  Points to the real part of second input vector.
 * @param[in]  p_src_b_im  Points to the imag part of second input vector.
 * @param[in]  n           The number of samples in each vector.
 * @param[out] p_src_c_re  Points to the real part of output complex result.
 * @param[out] p_src_c_im  Points to the imag part of output complex result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_i16_2(int32_t n, const int16_t *p_src_a_re,
                                        const int16_t *p_src_a_im,
                                        const int16_t *p_src_b_re,
                                        const int16_t *p_src_b_im,
                                        int16_t *p_src_c_re,
                                        int16_t *p_src_c_im);

/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed. Array
 * elements are assumed to be complex int16 in Q15 format and interleaved. <br>
 * All intermediate calculations results are stored in 32-bit internal
 * variables, saturating the value to prevent overflow. To get the final result
 * in Q15 and to avoid overflow, the accumulator narrows to 16 bits with
 * saturation.
 *
 * @param[in]     n             The number of samples in each vector.
 * @param[in]     p_src_a       Points to the first input vector.
 * @param[in]     p_src_b       Points to the second input vector.
 * @param[out]    p_src_c       Points to the output complex result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_i16_32bit(int32_t n,
                                            const armral_cmplx_int16_t *p_src_a,
                                            const armral_cmplx_int16_t *p_src_b,
                                            armral_cmplx_int16_t *p_src_c);

/**
 * This algorithm computes the dot product between a pair of arrays of complex
 * values. The arrays are multiplied element-by-element and then summed.<br>
 * Array elements are assumed to be int16 in Q15 format and separate arrays are
 * used for both the real parts and imaginary parts of the input data. <br> All
 * intermediate calculation results are stored in 32-bit internal variables,
 * saturating the value to prevent overflow. To get the final result in Q15 and
 * to avoid overflow, the accumulator narrows to 16 bits with saturation.
 *
 * @param[in]  n           The number of samples in each vector.
 * @param[in]  p_src_a_re  Points to the real part of the first input vector.
 * @param[in]  p_src_a_im  Points to the imaginary part of the first input
 *                         vector.
 * @param[in]  p_src_b_re  Points to the real part of the second input vector.
 * @param[in]  p_src_b_im  Points to the imaginary part of the second input
 *                         vector.
 * @param[out] p_src_c_re  Points to the real part of the output result.
 * @param[out] p_src_c_im  Points to the imaginary part of the output result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_vecdot_i16_2_32bit(
    int32_t n, const int16_t *p_src_a_re, const int16_t *p_src_a_im,
    const int16_t *p_src_b_re, const int16_t *p_src_b_im, int16_t *p_src_c_re,
    int16_t *p_src_c_im);

/** @} end of cmplx_dot_prod group */

/**
 * @ingroup groupMatrix
 */
/**
 * @addtogroup cmplx_matrix_vector_mult  Complex Matrix-Vector Multiplication
 * @{
 * \brief Computes a matrix-by-vector multiplication, storing the result in a
 * destination vector.
 *
 * The destination vector is only written to and can be uninitialized.
 */
/**
 * This algorithm performs the multiplication `A x` for matrix `A` and vector
 * `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * A 64-bit Q32.31 accumulator is used internally. If you do not need such a
 * large range, consider using \link armral_cmplx_mat_vec_mult_i16_32bit
 * \endlink instead. To get the final result in Q15 and to avoid overflow, the
 * accumulator narrows to 16 bits with saturation.
 *
 * @param[in]     m          The number of rows in matrix `A` and the length of
 *                           the output vector `y`.
 * @param[in]     n          The number of columns in matrix `A` and the length
 *                           of the input vector `x`.
 * @param[in]     p_src_a    Points to the input matrix.
 * @param[in]     p_src_x    Points to the input vector.
 * @param[out]    p_dst      Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_i16(uint16_t m, uint16_t n,
                                            const armral_cmplx_int16_t *p_src_a,
                                            const armral_cmplx_int16_t *p_src_x,
                                            armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors. Each multiplication is of the
 * form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * The matrix elements are interleaved such that all elements for a particular
 * location within the matrix are stored together. This means that, for
 * instance, the first `num_mats` complex numbers stored are the first element
 * of each of the matrices in the batch. The offset to the next location in the
 * same matrix is given by the `num_mats` batch size:
 * <pre>
 *   {Re(0), Im(0), Re(1), Im(1), Re(2), Im(2), ...}
 * </pre>
 * The same layout is used for vector elements, except that the offset to the
 * next vector element is `num_mats * num_vecs_per_mat`.
 *
 * The total number of elements in the batch (`num_mats * num_vecs_per_mat`)
 * must be a multiple of 12. The number of vectors per matrix must be either 1
 * or a multiple of 4.
 *
 * A 64-bit Q32.31 accumulator is used internally. If you do not need such a
 * large range, consider using \link armral_cmplx_mat_vec_mult_batch_i16_32bit
 * \endlink instead. To get the final result in Q15 and to avoid overflow, the
 * accumulator narrows to 16 bits with saturation.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix
 *                                  `A` and the length of each output vector
 *                                  `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_src_a           Points to the input matrix.
 * @param[in]     p_src_x           Points to the input vector.
 * @param[out]    p_dst             Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_i16(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_int16_t *p_src_a, const armral_cmplx_int16_t *p_src_x,
    armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors, utilizing a "pointer array"
 * storage layout for the input and output matrix batches. Each multiplication
 * is of the form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * The `p_srcs_a` parameter is an array of pointers of length `M * N`. The
 * value of `p_srcs_a[i]` is a pointer to the i-th element of the first matrix
 * in the batch, as represented in row-major ordering, such that the i-th
 * element of the j-th matrix in the batch is located at `p_srcs_a[i][j]`.
 * For example, the j-th matrix in a batch of `2-by-2` matrices is formed as:
 * <pre>
 *   p_srcs_a[0][j]  p_srcs_a[1][j]
 *   p_srcs_a[2][j]  p_srcs_a[3][j]
 * </pre>
 * The input vector array `p_srcs_x` and output vector array `p_dsts` also point
 * to an array of pointers, such that the i-th element of the j-th vector is
 * located at `p_srcs_x[i][j]` (and similarly for `p_dsts`). For example, the
 * j-th vector in a batch of vectors of length `2` is formed as:
 * <pre>
 *   p_srcs_x[0][j]
 *   p_srcs_x[1][j]
 * </pre>
 * representing an identical format to the input matrices.
 *
 * The total number of elements in the batch (`num_mats * num_vecs_per_mat`)
 * must be a multiple of 12. The number of vectors per matrix must be either 1
 * or a multiple of 4.
 *
 * A 64-bit Q32.31 accumulator is used internally. If you do not need such a
 * large range, consider using \link
 * armral_cmplx_mat_vec_mult_batch_i16_32bit_pa \endlink instead. To get the
 * final result in Q15 and to avoid overflow, the accumulator narrows to 16 bits
 * with saturation.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix `A`
 *                                  and the length of each output vector `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_srcs_a          Points to the input matrix structure.
 * @param[in]     p_srcs_x          Points to the input vector structure.
 * @param[out]    p_dsts            Points to the output vector structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_i16_pa(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_int16_t **p_srcs_a,
    const armral_cmplx_int16_t **p_srcs_x, armral_cmplx_int16_t **p_dsts);

/**
 * This algorithm performs the multiplication `A x` for matrix `A` and vector
 * `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * A 32-bit Q0.31 saturating accumulator is used internally. If you need a
 * larger range, consider using \link armral_cmplx_mat_vec_mult_i16 \endlink
 * instead. To get a Q15 result, the final result is narrowed to 16 bits with
 * saturation.
 *
 * @param[in]     m          The number of rows in matrix `A` and the length of
 *                           the output vector `y`.
 * @param[in]     n          The number of columns in matrix `A` and the length
 *                           of each input vector `x`.
 * @param[in]     p_src_a    Points to the input matrix.
 * @param[in]     p_src_x    Points to the input vector.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_i16_32bit(
    uint16_t m, uint16_t n, const armral_cmplx_int16_t *p_src_a,
    const armral_cmplx_int16_t *p_src_x, armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors. Each multiplication is of the
 * form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * The matrix elements are interleaved such that all elements for a particular
 * location within the matrix are stored together. This means that, for
 * instance, the first `num_mats` complex numbers stored are the first element
 * of each of the matrices in the batch. The offset to the next location in the
 * same matrix is given by the `num_mats` batch size:
 * <pre>
 *  {Re(0), Im(0), Re(1), Im(1), Re(2), Im(2), ...}
 * </pre>
 * The same layout is used for vector elements, except that the offset to the
 * next vector element is `num_mats * num_vecs_per_mat`.
 *
 * A 32-bit Q0.31 saturating accumulator is used internally. If you need a
 * larger range, consider using \link armral_cmplx_mat_vec_mult_batch_i16
 * \endlink instead. To get a Q15 result, the final result is narrowed to 16
 * bits with saturation.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix `A`
 *                                  and the length of each output vector `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_src_a           Points to the input matrix.
 * @param[in]     p_src_x           Points to the input vector.
 * @param[out]    p_dst             Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_i16_32bit(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_int16_t *p_src_a, const armral_cmplx_int16_t *p_src_x,
    armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors, utilizing a "pointer array"
 * storage layout for the input and output matrix batches. Each multiplication
 * is of the form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * The `p_srcs_a` parameter is an array of pointers of length `M * N`. The
 * value of `p_srcs_a[i]` is a pointer to the i-th element of the first matrix
 * in the batch, as represented in row-major ordering, such that the i-th
 * element of the j-th matrix in the batch is located at `p_srcs_a[i][j]`.
 * For example, the j-th matrix in a batch of `2-by-2` matrices is formed as:
 * <pre>
 *   p_srcs_a[0][j]  p_srcs_a[1][j]
 *   p_srcs_a[2][j]  p_srcs_a[3][j]
 * </pre>
 * The input vector array `p_srcs_x` and output vector array `p_dsts` also point
 * to an array of pointers, such that the i-th element of the j-th vector is
 * located at `p_srcs_x[i][j]` (and similarly for `p_dsts`). For example, the
 * j-th vector in a batch of vectors of length `2` is formed as:
 * <pre>
 *   p_srcs_x[0][j]
 *   p_srcs_x[1][j]
 * </pre>
 * representing an identical format to the input matrices.
 *
 * A 32-bit Q0.31 saturating accumulator is used internally. If you need a
 * larger range, consider using \link armral_cmplx_mat_vec_mult_batch_i16_pa
 * \endlink instead. To get a Q15 result, the final result is narrowed to 16
 * bits with saturation.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix `A`
 *                                  and the length of each output vector `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_srcs_a          Points to the input matrix structure.
 * @param[in]     p_srcs_x          Points to the input vector structure.
 * @param[out]    p_dsts            Points to the output vector structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_i16_32bit_pa(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_int16_t **p_srcs_a,
    const armral_cmplx_int16_t **p_srcs_x, armral_cmplx_int16_t **p_dsts);

/**
 * This algorithm performs the multiplication `A x` for matrix `A` and vector
 * `x`, and assumes that:
 * + Matrix and vector elements are complex float values.
 * + Matrices are stored in memory in row-major order.
 *
 * @param[in]     m          The number of rows in matrix `A` and the length of
 *                           the output vector `y`.
 * @param[in]     n          The number of columns in matrix `A` and the length
 *                           of the input vector `x`.
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_x    Points to the input vector.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_f32(uint16_t m, uint16_t n,
                                            const armral_cmplx_f32_t *p_src_a,
                                            const armral_cmplx_f32_t *p_src_x,
                                            armral_cmplx_f32_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors. Each multiplication is of the
 * form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex float values.
 * + Matrices are stored in memory in row-major order.
 *
 * The matrix elements are interleaved such that all elements for a particular
 * location within the matrix are stored together. This means that, for
 * instance, the first `num_mats` complex numbers stored are the first element
 * of each of the matrices in the batch. The offset to the next location in the
 * same matrix is given by the `num_mats` batch size:
 * <pre>
 *   {Re(0), Im(0), Re(1), Im(1), Re(2), Im(2), ...}
 * </pre>
 * The same layout is used for vector elements, except that the offset to the
 * next vector element is `num_mats * num_vecs_per_mat`.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix
 *                                  `A` and the length of each output vector
 *                                  `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_src_a           Points to the input matrix.
 * @param[in]     p_src_x           Points to the input vector.
 * @param[out]    p_dst             Points to the output vector.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_f32(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_f32_t *p_src_a, const armral_cmplx_f32_t *p_src_x,
    armral_cmplx_f32_t *p_dst);

/**
 * This algorithm performs matrix-vector multiplication for a batch of
 * `M-by-N` matrices and length `N` input vectors, utilizing a "pointer array"
 * storage layout for the input and output matrix batches. Each multiplication
 * is of the form `A x` for a matrix `A` and vector `x`, and assumes that:
 * + Matrix and vector elements are complex float values.
 * + Matrices are stored in memory in row-major order.
 *
 * The `p_srcs_a` parameter is an array of pointers of length `M * N`. The
 * value of `p_srcs_a[i]` is a pointer to the i-th element of the first matrix
 * in the batch, as represented in row-major ordering, such that the i-th
 * element of the j-th matrix in the batch is located at `p_srcs_a[i][j]`.
 * For example, the j-th matrix in a batch of `2-by-2` matrices is formed as:
 * <pre>
 *   p_srcs_a[0][j]  p_srcs_a[1][j]
 *   p_srcs_a[2][j]  p_srcs_a[3][j]
 * </pre>
 * The input vector array `p_srcs_x` and output vector array `p_dsts` also point
 * to an array of pointers, such that the i-th element of the j-th vector is
 * located at `p_srcs_x[i][j]` (and similarly for `p_dsts`). For example, the
 * j-th vector in a batch of vectors of length `2` is formed as:
 * <pre>
 *   p_srcs_x[0][j]
 *   p_srcs_x[1][j]
 * </pre>
 * representing an identical format to the input matrices.
 *
 * @param[in]     num_mats          The number of input matrices.
 * @param[in]     num_vecs_per_mat  The number of input and output vectors
 *                                  for each input matrix. The total number of
 *                                  input vectors is
 *                                  `num_mats * num_vecs_per_mat`. There are
 *                                  the same number of output vectors.
 * @param[in]     m                 The number of rows (`M`) in each matrix `A`
 *                                  and the length of each output vector `y`.
 * @param[in]     n                 The number of columns (`N`) in each matrix
 *                                  `A` and the length of each input vector `x`.
 * @param[in]     p_srcs_a          Points to the input matrix structure.
 * @param[in]     p_srcs_x          Points to the input vector structure.
 * @param[out]    p_dsts            Points to the output vector structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_vec_mult_batch_f32_pa(
    uint16_t num_mats, uint16_t num_vecs_per_mat, uint16_t m, uint16_t n,
    const armral_cmplx_f32_t **p_srcs_a, const armral_cmplx_f32_t **p_srcs_x,
    armral_cmplx_f32_t **p_dsts);

/** @} end of cmplx_matrix_vector_mult group */

/**
 * @ingroup groupMatrix
 */
/**
 * @addtogroup cmplx_matrix_mult  Complex Matrix-Matrix Multiplication
 * @{
 * \brief Computes a matrix-by-matrix multiplication, storing the result in a
 * destination matrix.
 *
 * The destination matrix is only written to and can be uninitialized.
 *
 * To permit specifying different fixed-point formats for the input and output
 * matrices, the `solve` routines take an extra fixed-point type specifier.
 */
/**
 * This algorithm performs the multiplication `A B` for matrices, and assumes
 * that:
 * + Matrix elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * A 64-bit Q32.31 accumulator is used internally. If you do not need such a
 * large range, consider using \link armral_cmplx_mat_mult_i16_32bit \endlink
 * instead. To get the final result in Q15 and to avoid overflow, the
 * accumulator narrows to 16 bits with saturation.
 *
 * @param[in]     m          The number of rows in matrix `A`.
 * @param[in]     n          The number of columns in matrix `A`.
 * @param[in]     k          The number of columns in matrix `B`.
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_b    Points to the second input matrix.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_i16(uint16_t m, uint16_t n, uint16_t k,
                                        const armral_cmplx_int16_t *p_src_a,
                                        const armral_cmplx_int16_t *p_src_b,
                                        armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs the multiplication `A B` for matrices, and assumes
 * that:
 * + Matrix elements are complex int16 in Q15 format.
 * + Matrices are stored in memory in row-major order.
 *
 * A 32-bit Q0.31 saturating accumulator is used internally. If you need a
 * larger range, consider using \link armral_cmplx_mat_mult_i16 \endlink
 * instead. To get a Q15 result, the final result is narrowed to 16 bits with
 * saturation.
 *
 * @param[in]     m          The number of rows in matrix `A`.
 * @param[in]     n          The number of columns in matrix `A`.
 * @param[in]     k          The number of columns in matrix `B`.
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_b    Points to the second input matrix.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_i16_32bit(
    uint16_t m, uint16_t n, uint16_t k, const armral_cmplx_int16_t *p_src_a,
    const armral_cmplx_int16_t *p_src_b, armral_cmplx_int16_t *p_dst);

/**
 * This algorithm performs the multiplication `A B` for matrices of float
 * values, and assumes that matrices are stored in memory row-major.
 *
 * @param[in]     m          The number of rows in matrix `A`.
 * @param[in]     n          The number of columns in matrix `A`.
 * @param[in]     k          The number of columns in matrix `B`.
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_b    Points to the second input matrix.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_f32(uint16_t m, uint16_t n, uint16_t k,
                                        const armral_cmplx_f32_t *p_src_a,
                                        const armral_cmplx_f32_t *p_src_b,
                                        armral_cmplx_f32_t *p_dst);

/**
 * This algorithm performs an optimized product of two square `2-by-2` matrices.
 * The algorithm assumes that matrix `A` (first matrix) is column-major before
 * entering the `armral_cmplx_mat_mult_2x2_f32` function.<br>
 * Matrix `B` (second matrix) is also assumed to be column-major. The result of
 * the product is a column-major matrix. In LTE and 5G, you can use the
 * `armral_cmplx_mat_mult_2x2_f32` function in the equalization
 * step in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * Equalization matrix `G` corresponds to the first input matrix (matrix `A`) of
 * the function.  The algorithm assumes that matrix `G` is transposed during
 * computation so that the matrix presents as column-major on input. <br> The
 * second input matrix (matrix `B`) is formed by two `2-by-1` vectors (`y`
 * vectors in the preceding formula) so that each row of B represents a `2-by-1`
 * vector output from each antenna port, and each call to
 * `armral_cmplx_mat_mult_2x2_f32` computes two distinct `x̂` estimates.
 *
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_b    Points to the second input matrix.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_2x2_f32(const armral_cmplx_f32_t *p_src_a,
                                            const armral_cmplx_f32_t *p_src_b,
                                            armral_cmplx_f32_t *p_dst);

/**
 * This algorithm performs an optimized product of two square `2-by-2` matrices
 * whose complex elements have already been separated into real component and
 * imaginary component arrays. The algorithm assumes that matrix `A` (first
 * matrix) is column-major before entering the
 * `armral_cmplx_mat_mult_2x2_f32_iq` function.<br> Matrix `B` (second matrix)
 * is also considered to be column-major. The result of the product is a
 * column-major matrix. In LTE and 5G, you can use the
 * `armral_cmplx_mat_mult_2x2_f32_iq` function in the equalization step in the
 * formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * Equalization matrix `G` corresponds to the first input matrix (matrix `A`) of
 * the function. The algorithm assumes matrix `G` is transposed during
 * computation so that the matrix presents as column-major on input. <br> The
 * second input matrix (matrix `B`) is formed by two `2-by-1` vectors (`y`
 * vectors in the preceding formula) so that each row of B represents a `2-by-1`
 * vector output from each antenna port, and each call to
 * `armral_cmplx_mat_mult_2x2_f32_iq` computes two distinct `x̂` estimates.
 *
 * @param[in]     src_a_re   Points to the real part of the first input matrix.
 * @param[in]     src_a_im   Points to the imag part of the first input matrix.
 * @param[in]     src_b_re   Points to the real part of the second input matrix.
 * @param[in]     src_b_im   Points to the imag part of the second input matrix.
 * @param[out]    dst_re     Points to the real part of the output matrix.
 * @param[out]    dst_im     Points to the imag part of the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_2x2_f32_iq(const float32_t *src_a_re,
                                               const float32_t *src_a_im,
                                               const float32_t *src_b_re,
                                               const float32_t *src_b_im,
                                               float32_t *dst_re,
                                               float32_t *dst_im);

/**
 * This algorithm performs an optimized product of two square `4-by-4` matrices.
 * The algorithm assumes that matrix `A` (first matrix) is column-major before
 * entering the `armral_cmplx_mat_mult_4x4_f32` function.<br>
 * Matrix `B` (second matrix) is also considered to be column-major. The result
 * of the product is a column-major matrix. In LTE and 5G, you can use the
 * `armral_cmplx_mat_mult_4x4_f32` function in the equalization step in the
 * formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * Equalization matrix `G` corresponds to the first input matrix (matrix `A`) of
 * the function. <br> The algorithm assumes that matrix `G` is transposed during
 * computation so that the matrix presents as column-major on input. <br> The
 * second input matrix (matrix `B`) is formed by four `4-by-1` vectors (`y`
 * vectors in the preceding formula) so that each row of B represents a `4-by-1`
 * vector output from each antenna port, and each call to
 * `cmplx_mat_mult_4x4_f32` computes four distinct `x̂` estimates.
 *
 * @param[in]     p_src_a    Points to the first input matrix.
 * @param[in]     p_src_b    Points to the second input matrix.
 * @param[out]    p_dst      Points to the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_4x4_f32(const armral_cmplx_f32_t *p_src_a,
                                            const armral_cmplx_f32_t *p_src_b,
                                            armral_cmplx_f32_t *p_dst);

/**
 * This algorithm performs an optimized product of two square `4-by-4` matrices
 * whose complex elements have already been separated into real and imaginary
 * component arrays. The algorithm assumes that matrix `A` (first matrix) is
 * column-major before entering the `armral_cmplx_mat_mult_4x4_f32_iq`
 * function.<br> Matrix `B` (second matrix) is also considered to be
 * column-major. The result of the product is a column-major matrix. In LTE and
 * 5G, you can use the `armral_cmplx_mat_mult_4x4_f32_iq`
 * function in the equalization step in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * Equalization matrix `G` corresponds to the first input matrix (matrix `A`) of
 * the function.  The algorithm assumes that matrix `G` is transposed during
 * computation so that the matrix presents as column-major on input. <br> The
 * second input matrix (matrix `B`) is formed by four `4-by-1` vectors (`y`
 * vectors in the preceding formula) so that each row of B represents a `4-by-1`
 * vector output from each antenna port, and each call to
 * `armral_cmplx_mat_mult_4x4_f32_iq` computes four distinct `x̂` estimates.
 *
 * @param[in]     src_a_re   Points to the real part of the first input matrix.
 * @param[in]     src_a_im   Points to the imag part of the first input matrix.
 * @param[in]     src_b_re   Points to the real part of the second input matrix.
 * @param[in]     src_b_im   Points to the imag part of the second input matrix.
 * @param[out]    dst_re     Points to the real part of the output matrix.
 * @param[out]    dst_im     Points to the imag part of the output matrix.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_mult_4x4_f32_iq(const float32_t *src_a_re,
                                               const float32_t *src_a_im,
                                               const float32_t *src_b_re,
                                               const float32_t *src_b_im,
                                               float32_t *dst_re,
                                               float32_t *dst_im);

/**
 * In LTE and 5G, you can use the `armral_solve_2x2_f32` function
 * in the equalization step, as in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * where `y` is a vector for the received signal, size corresponds to the
 * number of antennae and `x̂` is the estimate of the transmitted signal, size
 * corresponds to the number of layers. `G` is the equalization complex matrix
 * and is assumed to be a `2-by-2` matrix. I and Q components of `G` elements
 * are assumed to be stored separated in memory.<br> Also, each coefficient of
 * `G`
 * (`G11`,`G12`,`G21`,`G22`) is assumed to be stored separated in memory
 * locations set at `pGstride` one from the other.<br> The number of input
 * signals is assumed to be a multiple of 12, and must be divisible by the
 * number of subcarriers per `G` matrix.<br>
 * For type 1 equalization, the number of subcarriers per `G` matrix must be
 * four. For type 2 equalization, the number of subcarriers per `G` matrix must
 * be six. An implementation is also available for cases where the number of
 * subcarriers per `G` matrix is equal to one.
 *
 * @param[in]  num_sub_carrier     The number of subcarriers to equalize.
 * @param[in]  num_sc_per_g        The number of subcarriers per `G` matrix.
 * @param[in]  p_y                 Points to the input received signal.
 * @param[in]  p_ystride           The stride between two Rx antennae.
 * @param[in]  p_y_num_fract_bits  The number of fractional bits in `y`.
 * @param[in]  p_g_real            The real part of coefficient matrix `G`.
 * @param[in]  p_g_imag            The imag part of coefficient matrix `G`.
 * @param[in]  p_gstride           The stride between elements of `G`.
 * @param[out] p_x                 Points to the output received signal.
 * @param[in]  p_xstride           The stride between two layers.
 * @param[in]  num_fract_bits_x    The number of fractional bits in `x`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_solve_2x2_f32(uint32_t num_sub_carrier, uint32_t num_sc_per_g,
                     const armral_cmplx_int16_t *p_y, uint32_t p_ystride,
                     const armral_fixed_point_index *p_y_num_fract_bits,
                     const float32_t *p_g_real, const float32_t *p_g_imag,
                     uint32_t p_gstride, armral_cmplx_int16_t *p_x,
                     uint32_t p_xstride,
                     armral_fixed_point_index num_fract_bits_x);

/**
 * In LTE and 5G, you can use the `armral_solve_2x4_f32` function
 * in the equalization step, as in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * where `y` is a vector for the received signal, size corresponds to the
 * number of antennae and `x̂` is the estimate of the transmitted signal, size
 * corresponds to the number of layers.<br> `G` is the equalization complex
 * matrix and is assumed to be a `2-by-4` matrix. I and Q components of `G`
 * elements are assumed to be stored separated in memory.<br> Also, each
 * coefficient of `G` (`G11`,`G12`,`G21`,`G22`) is assumed to be stored
 * separated in memory locations set at `pGstride` one from the other.<br> The
 * number of input signals is assumed to be a multiple of 12, and must be
 * divisible by the number of subcarriers per `G` matrix.<br> For type 1
 * equalization, the number of subcarriers per `G` matrix must be four. For type
 * 2 equalization, the number of subcarriers per `G` matrix must be six.  An
 * implementation is also available for cases where the number of subcarriers
 * per `G` matrix is equal to one.
 *
 * @param[in]  num_sub_carrier      The number of subcarrier to equalize.
 * @param[in]  num_sc_per_g         The number of subcarriers per `G` matrix.
 * @param[in]  p_y                  Points to the input received signal.
 * @param[in]  p_ystride            The stride between two Rx antennae.
 * @param[in]  p_y_num_fract_bits   The number of fractional bits in `y`.
 * @param[in]  p_g_real             The real part of coefficient matrix `G`.
 * @param[in]  p_g_imag             The imag part of coefficient matrix `G`.
 * @param[in]  p_gstride            The stride between elements of `G`.
 * @param[out] p_x                  Points to the output received signal.
 * @param[in]  p_xstride            The stride between two layers.
 * @param[in]  num_fract_bits_x     The number of fractional bits in `x`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_solve_2x4_f32(uint32_t num_sub_carrier, uint32_t num_sc_per_g,
                     const armral_cmplx_int16_t *p_y, uint32_t p_ystride,
                     const armral_fixed_point_index *p_y_num_fract_bits,
                     const float32_t *p_g_real, const float32_t *p_g_imag,
                     uint32_t p_gstride, armral_cmplx_int16_t *p_x,
                     uint32_t p_xstride,
                     armral_fixed_point_index num_fract_bits_x);

/**
 * In LTE and 5G, you can use the `armral_solve_4x4_f32` function
 * in the equalization step, as in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * where `y` is a vector for the received signal, size corresponds to the
 * number of antennae and `x̂` is the estimate of the transmitted signal, size
 * corresponds to the number of layers.<br> `G` is the equalization complex
 * matrix and is assumed to be a `2-by-4` matrix. I and Q components of `G`
 * elements are assumed to be stored separated in memory.<br> Also, each
 * coefficient of `G` (`G11`,`G12`,`G21`,`G22`) is assumed to be stored
 * separated in memory locations set at `pGstride` one from the other.<br> The
 * number of input signals is assumed to be a multiple of 12, and must be
 * divisible by the number of subcarriers per `G` matrix.<br> For type 1
 * equalization, the number of subcarriers per `G` matrix must be four. For type
 * 2 equalization, the number of subcarriers per `G` matrix must be six.  An
 * implementation is also available for cases where the number of subcarriers
 * per `G` matrix is equal to one.
 *
 * @param[in]  num_sub_carrier       The number of subcarrier to equalize.
 * @param[in]  num_sc_per_g          The number of subcarriers per `G` matrix.
 * @param[in]  p_y                   Points to the input received signal.
 * @param[in]  p_ystride             The stride between two Rx antennae.
 * @param[in]  p_y_num_fract_bits    The number of fractional bits in `y`.
 * @param[in]  p_g_real              The real part of coefficient matrix `G`.
 * @param[in]  p_g_imag              The imag part of coefficient matrix `G`.
 * @param[in]  p_gstride             The stride between elements of `G`.
 * @param[out] p_x                   Points to the output received signal.
 * @param[in]  p_xstride             The stride between two layers.
 * @param[in]  num_fract_bits_x      The number of fractional bits in `x`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_solve_4x4_f32(uint32_t num_sub_carrier, uint32_t num_sc_per_g,
                     const armral_cmplx_int16_t *p_y, uint32_t p_ystride,
                     const armral_fixed_point_index *p_y_num_fract_bits,
                     const float32_t *p_g_real, const float32_t *p_g_imag,
                     uint32_t p_gstride, armral_cmplx_int16_t *p_x,
                     uint32_t p_xstride,
                     armral_fixed_point_index num_fract_bits_x);

/**
 * In LTE and 5G, you can use the `armral_solve_1x4_f32` function
 * in the equalization step, as in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * where `y` is a vector for the received signal, size corresponds to the
 * number of antennae and `x̂` is the estimate of the transmitted signal, size
 * corresponds to the number of layers.<br> `G` is the equalization complex
 * matrix and is assumed to be a `2-by-4` matrix. I and Q components of `G`
 * elements are assumed to be stored separated in memory.<br> Also, each
 * coefficient of `G` (`G11`,`G12`,`G21`,`G22`) is assumed to be stored
 * separated in memory locations set at `pGstride` one from the other.<br> The
 * number of input signals is assumed to be a multiple of 12, and must be
 * divisible by the number of subcarriers per `G` matrix.<br> For type 1
 * equalization, the number of subcarriers per `G` matrix must be four. For type
 * 2 equalization, the number of subcarriers per `G` matrix must be six.  An
 * implementation is also available for cases where the number of subcarriers
 * per `G` matrix is equal to one.
 *
 * @param[in]  num_sub_carrier         The number of subcarrier to equalize.
 * @param[in]  num_sc_per_g            The number of subcarriers per `G` matrix.
 * @param[in]  p_y                     Points to the input received signal.
 * @param[in]  p_ystride               The stride between two Rx antennae.
 * @param[in]  p_y_num_fract_bits      The number of fractional bits in `y`
 *                                     conversion.
 * @param[in]  p_g_real                The real part of coefficient matrix `G`.
 * @param[in]  p_g_imag                The imag part of coefficient matrix `G`.
 * @param[in]  p_gstride               The stride between elements of `G`.
 * @param[out] p_x                     Points to the output received signal.
 * @param[in]  num_fract_bits_x        The number of fractional bits in `x`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_solve_1x4_f32(uint32_t num_sub_carrier, uint32_t num_sc_per_g,
                     const armral_cmplx_int16_t *p_y, uint32_t p_ystride,
                     const armral_fixed_point_index *p_y_num_fract_bits,
                     const float32_t *p_g_real, const float32_t *p_g_imag,
                     uint32_t p_gstride, armral_cmplx_int16_t *p_x,
                     armral_fixed_point_index num_fract_bits_x);

/**
 * In LTE and 5G, you can use the `armral_solve_1x2_f32` function in the
 * equalization step, as in the formula:
 *
 * <pre>
 *  x̂ = G y
 * </pre>
 *
 * where `y` is a vector for the received signal, size corresponds to the
 * number of antennae and `x̂` is the estimate of the transmitted signal, size
 * corresponds to the number of layers.  `G` is the equalization complex matrix
 * and is assumed to be a `2-by-4` matrix. I and Q components of `G` elements
 * are assumed to be stored separated in memory.<br> Also, each coefficient of
 * `G`
 * (`G11`,`G12`,`G21`,`G22`) is assumed to be stored separated in memory
 * locations set at `pGstride` one from the other.<br> The number of input
 * signals is assumed to be a multiple of 12, and must be divisible by the
 * number of subcarriers per `G` matrix.<br> For type 1 equalization, the
 * number of subcarriers per `G` matrix must be four. For type 2 equalization,
 * the number of subcarriers per `G` matrix must be six.  An implementation
 * is also available for cases where the number of subcarriers per `G` matrix is
 * equal to one.
 *
 * @param[in]  num_sub_carrier       The number of subcarrier to equalize.
 * @param[in]  num_sc_per_g          The number of subcarriers per `G` matrix.
 * @param[in]  p_y                   Points to the input received signal.
 * @param[in]  p_ystride             The stride between two Rx antennae.
 * @param[in]  p_y_num_fract_bits    The number of fractional bits in `y`
 *                                   conversion.
 * @param[in]  p_g_real              The real part of coefficient matrix `G`.
 * @param[in]  p_g_imag              The imag part of coefficient matrix `G`.
 * @param[in]  p_gstride             The stride between elements of `G`.
 * @param[out] p_x                   Points to the output received signal.
 * @param[in]  num_fract_bits_x      The number of fractional bits in `x`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_solve_1x2_f32(uint32_t num_sub_carrier, uint32_t num_sc_per_g,
                     const armral_cmplx_int16_t *p_y, uint32_t p_ystride,
                     const armral_fixed_point_index *p_y_num_fract_bits,
                     const float32_t *p_g_real, const float32_t *p_g_imag,
                     uint32_t p_gstride, armral_cmplx_int16_t *p_x,
                     armral_fixed_point_index num_fract_bits_x);

/** @} end of cmplx_matrix_mult group */

/**
 * @ingroup groupMatrix
 */
/**
 * @addtogroup cmplx_matrix_inv Complex Matrix Inversion
 * @{
 * \brief Computes the inverse of a complex Hermitian square matrix of size
 * `N-by-N`.
 */
/**
 * This algorithm computes the inverse of a single complex Hermitian square
 * matrix of size `N-by-N`.<br> The supported dimensions are `2-by-2`, `3-by-3`,
 * `4-by-4`, `8-by-8`, and `16-by-16`.<br> The input and output matrices are
 * filled in row-major order with complex `float32_t` elements.
 *
 * @param[in]    size       The size of the input matrix.
 * @param[in]    p_src      Points to the input matrix structure.
 * @param[out]   p_dst      Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_hermitian_mat_inverse_f32(
    uint32_t size, const armral_cmplx_f32_t *p_src, armral_cmplx_f32_t *p_dst);

/**
 * This algorithm computes the inverse of a single complex square
 * matrix of size `N-by-N`.<br> The supported dimensions are `2-by-2`, `3-by-3`,
 * `4-by-4`, `8-by-8`, and `16-by-16`.<br> The input and output matrices are
 * filled in row-major order with complex `float32_t` elements.
 *
 * @param[in]    size       The size of the input matrix.
 * @param[in]    p_src      Points to the input matrix structure.
 * @param[out]   p_dst      Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_mat_inverse_f32(uint32_t size,
                                           const armral_cmplx_f32_t *p_src,
                                           armral_cmplx_f32_t *p_dst);

/**
 * This algorithm computes the inverse of a batch of `M` complex Hermitian
 * square matrices, each of size `N-by-N`.<br> The supported matrix dimensions
 * are `2-by-2`, `3-by-3`, and `4-by-4`.<br> The input and output matrices are
 * filled in row-major order with complex `float32_t` elements, interleaved such
 * that all elements for a particular location within the matrix are stored
 * together. This means that, for instance, the first four complex numbers
 * stored are the first element from each of the first four matrices in the
 * batch. The offset to the next location in the same matrix is given by the
 * `num_mats` batch size:
 * <pre>
 *   {Re(0), Im(0), Re(1), Im(1), ..., Re(M - 1), Im(M - 1)}
 * </pre>
 * The number of matrices in a batch (`M`) must be a multiple of the matrix
 * dimension. So, if `N = 2` then `M` must be a multiple of two, and if `N = 4`
 * then `M` must be a multiple of four.
 *
 * @param[in]  num_mats   The number (`M`) of input and output matrices.
 * @param[in]  size       The size (`N`) of the input and output matrix.
 * @param[in]  p_src      Points to the input matrix structure.
 * @param[out] p_dst      Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_cmplx_hermitian_mat_inverse_batch_f32(uint32_t num_mats, uint32_t size,
                                             const armral_cmplx_f32_t *p_src,
                                             armral_cmplx_f32_t *p_dst);

/**
 * This algorithm computes the inverse of a batch of `M` complex general
 * square matrices, each of size `N-by-N`.<br> The supported matrix dimensions
 * are `2-by-2`, `3-by-3`, and `4-by-4`.<br> The input and output matrices are
 * filled in row-major order with complex `float32_t` elements, interleaved such
 * that all elements for a particular location within the matrix are stored
 * together. This means that, for instance, the first four complex numbers
 * stored are the first element from each of the first four matrices in the
 * batch.  The offset to the next location in the same matrix is given by the
 * `num_mats` batch size:
 * <pre>
 *   {Re(0), Im(0), Re(1), Im(1), ..., Re(M - 1), Im(M - 1)}
 * </pre>
 * The number of matrices in a batch (`M`) must be a multiple of the matrix
 * dimension. So, if `N = 2` then `M` must be a multiple of two, and if `N = 4`
 * then `M` must be a multiple of four.
 *
 * @param[in]  num_mats   The number (`M`) of input and output matrices.
 * @param[in]  size       The size (`N`) of the input and output matrix.
 * @param[in]  p_src      Points to the input matrix structure.
 * @param[out] p_dst      Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_cmplx_mat_inverse_batch_f32(uint32_t num_mats, uint32_t size,
                                   const armral_cmplx_f32_t *p_src,
                                   armral_cmplx_f32_t *p_dst);

/**
 * This algorithm computes the inverse of a batch of `M` complex Hermitian
 * square matrices, each of size `N-by-N`, utilizing a "pointer array" storage
 * layout for the input and output matrix batches.<br> The supported matrix
 * dimensions are `2-by-2`, `3-by-3`, and `4-by-4`.<br> The `p_srcs` parameter
 * is an array of pointers of length `N-by-N`. The value of `p_srcs[i]` is a
 * pointer to the i-th element of the first matrix in the batch, as represented
 * in row-major ordering, such that the i-th element of the j-th matrix in the
 * batch is located at `p_srcs[i][j]`. Similarly, the j-th matrix in a batch of
 * `2-by-2` matrices is formed as:
 * <pre>
 *   p_srcs[0][j]  p_srcs[1][j]
 *   p_srcs[2][j]  p_srcs[3][j]
 * </pre>
 * The output array `p_dsts` points to an array of pointers, representing an
 * identical format to the input.
 *
 * The number of matrices in a batch (`M`) must be a multiple of the matrix
 * dimension. So, if `N = 2` then `M` must be a multiple of two, and if `N = 4`
 * then `M` must be a multiple of four.
 *
 * @param[in]  num_mats   The number (`M`) of input and output matrices.
 * @param[in]  size       The size (`N`) of the input and output matrix.
 * @param[in]  p_srcs     Points to the input matrix structure.
 * @param[out] p_dsts     Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_cmplx_hermitian_mat_inverse_batch_f32_pa(
    uint32_t num_mats, uint32_t size, const armral_cmplx_f32_t **p_srcs,
    armral_cmplx_f32_t **p_dsts);

/**
 * This algorithm computes the inverse of a batch of `M` complex general
 * square matrices, each of size `N-by-N`, utilizing a "pointer array" storage
 * layout for the input and output matrix batches.<br> The supported matrix
 * dimensions are `2-by-2`, `3-by-3`, and `4-by-4`.<br> The `p_srcs` parameter
 * is an array of pointers of length `N-by-N`. The value of `p_srcs[i]` is a
 * pointer to the i-th element of the first matrix in the batch, as represented
 * in row-major ordering, such that the i-th element of the j-th matrix in the
 * batch is located at `p_srcs[i][j]`. Similarly, the j-th matrix in a batch of
 * `2-by-2` matrices is formed as:
 * <pre>
 *   p_srcs[0][j]  p_srcs[1][j]
 *   p_srcs[2][j]  p_srcs[3][j]
 * </pre>
 * The output array `p_dsts` points to an array of pointers, representing an
 * identical format to the input.
 *
 * The number of matrices in a batch (`M`) must be a multiple of the matrix
 * dimension. So, if `N = 2` then `M` must be a multiple of two, and if `N = 4`
 * then `M` must be a multiple of four.
 *
 * @param[in]  num_mats   The number (`M`) of input and output matrices.
 * @param[in]  size       The size (`N`) of the input and output matrix.
 * @param[in]  p_srcs     Points to the input matrix structure.
 * @param[out] p_dsts     Points to the output matrix structure.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_cmplx_mat_inverse_batch_f32_pa(uint32_t num_mats, uint32_t size,
                                      const armral_cmplx_f32_t **p_srcs,
                                      armral_cmplx_f32_t **p_dsts);

/** @} end of cmplx_matrix_inv group */

/**
 * @ingroup groupLowPhy
 */
/**
 * @addtogroup seqGen Sequence Generator
 * @{
 * \brief Fills a pointer with a Gold Sequence of the specified length,
 * generated from the specified seed.
 *
 * The sequence generator is the same generator that is described in the 3GPP
 * Technical Specification (TS) 36.211, Chapter 7.2.
 */
/**
 * This algorithm generates a pseudo-random sequence (Gold Sequence) that is
 * used in 4G and 5G networks to scramble data of a specific channel or to
 * generate a specific sequence (for example for Downlink Reference Signal
 * generation).<br> The sequence generator is the same generator that is
 * described in the 3GPP Technical Specification (TS) 36.211, Chapter 7.2. The
 * generator uses two polynomials, `x1` and `x2`, defined as:
 *
 * <pre>
 *  x1(n+31) = (x1(n+3) + x1(n)) mod 2
 *  x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2
 * </pre>
 *
 * to generate the output sequence:
 *
 * <pre>
 *  c(n) = (x1(n+Nc) + x2(n+Nc)) mod 2
 * </pre>
 *
 * where `Nc` is a constant with a value of 1600. The initialization for `x1`
 * and `x2` satisfies the condition that:
 *
 * <pre>
 *  x1(0) = 1
 *  x1(i) = 0               for i=1,2,...,30
 *  x2(i) = cinit(i) >> i   for i=0,1,...,30
 * </pre>
 *
 * The `cinit` parameter is provided as an input parameter for the algorithm,
 * which is used to derive `x2`. The algorithm generates `x1` and `x2` and
 * skips the first 1600 bits.
 *
 * @param[in]    sequence_len   The length of the sequence in bits (cinit).
 * @param[in]    seed           The random sequence starting point.
 * @param[in]    p_dst          Points to the output bits.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_seq_generator(uint16_t sequence_len, uint32_t seed,
                                   uint8_t *p_dst);

/** @} end seqGen */

/**
 * @ingroup groupUpPhy
 */
/**
 * @addtogroup modul Modulation
 * @{
 * \brief Performs modulation and demodulation of digital signals. Modulation
 * takes a bitstream and outputs a series of Q2.13 fixed-point complex symbols.
 * Demodulation takes Q2.13 fixed-point complex symbols and generates a series
 * of log-likelihood ratios (LLRs), which can be used in Polar decoding.
 *
 * The functions take as parameter the modulation type being used, namely either
 * QPSK or QAM, see `armral_modulation_type`.
 *
 * The number of complex samples needed for a given bitstream (and therefore the
 * size of the memory buffer passed) depends on the modulation type being used:
 * QPSK, 16QAM, 64QAM, and 256QAM correspond to two, four, six, and eight bits
 * per symbol, respectively (log base-2 of the constellation size).
 */
/**
 * Performs modulation of a bitstream, outputs a series of Q2.13 fixed-point
 * complex symbols.
 *
 * The expected size of `p_dst` depends on the modulation type being used:
 * QPSK, 16QAM, 64QAM, and 256QAM consume two, four, six, and eight bits per
 * symbol, respectively.
 *
 * @param[in]    nbits     The number of input modulated bits.
 * @param[in]    mod_type  The type of modulation to perform.
 * @param[in]    p_src     Points to input bit flow.
 * @param[out]   p_dst     Points to output complex symbols (format Q2.13).
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_modulation(uint32_t nbits, armral_modulation_type mod_type,
                                const uint8_t *p_src,
                                armral_cmplx_int16_t *p_dst);

/**
 * This algorithm implements the soft-demodulation (or soft bit demapping) for
 * QPSK, 16QAM, 64QAM, and 256QAM constellations.<br> For complex symbols `x_i`,
 * the input sequence is assumed to be made of complex symbols `rx = rx_re + j *
 * rx_im`, whose components I and Q are 16 bits each (format Q2.13) and in an
 * interleaved form:
 *
 * <pre>
 *  {Re(0), Im(0), Re(1), Im(1), ..., Re(N - 1), Im(N - 1)}
 * </pre>
 *
 * The output of the soft-demodulation algorithm is a sequence of
 * log-likelihood ratio (LLR) int8_t values, which indicate the relative
 * confidence of the demapping decision, component by component, instead of
 * taking a hard decision and giving the bit value itself.<br>
 *
 * The LLRs calculations are made approximately with thresholds method, to have
 * similar performance of the raw calculation, but with a lower complexity. The
 * base of the logarithm used depends on the noise power and the specified
 * `ulp`.<br>
 * All the constellations mapping follow those defined in the 3GPP Technical
 * Specification (TS) 38.211 V15.2.0, Chapter 5.1.
 *
 * @param[in]    n_symbols   The number of complex symbols in the input.
 * @param[in]    ulp         The change in input amplitude corresponding to a
                             unit change in the output LLRs (format Q2.13).
                             The integer representation of `ulp` must lie in
                             the range [2, 2^15].
 * @param[in]    mod_type    The modulation type.
 * @param[in]    p_src       Points to input complex source (format Q2.13).
 * @param[out]   p_dst       Points to the output byte seq.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_demodulation(uint32_t n_symbols, uint16_t ulp,
                                  armral_modulation_type mod_type,
                                  const armral_cmplx_int16_t *p_src,
                                  int8_t *p_dst);

/** @} end modul */

/**
 * @ingroup groupLowPhy
 */
/**
 * @addtogroup armral_corr_coeff Correlation Coefficient
 * @{
 * \brief Calculates Pearson's Correlation Coefficient from a pair of complex
 * vectors.
 */
/**
 * Calculates Pearson's Correlation Coefficient from a pair of vectors of
 * complex numbers in Q15 format with real component and imaginary component
 * interleaved, with the result stored to a pointer to a single complex number.
 *
 * Pearson's correlation coefficient is calculated using:
 *
 * <pre>
 *             SUM(x*conj(y)) - n*avg(x)*avg(y)
 *  Rxy = -----------------------------------------------
 *        SQRT(SUM(x*conj(x)) - n*avg(x)*conj(avg(x)))
 *        * SQRT(SUM(y*conj(y)) - n*avg(y)*conj(avg(y)))
 * </pre>
 *
 * @param[in]  n         The number of complex samples in each vector.
 * @param[in]  p_src_a   Points to the first input vector.
 * @param[in]  p_src_b   Points to the second input vector.
 * @param[out] c         Points to the result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_corr_coeff_i16(int32_t n,
                                    const armral_cmplx_int16_t *p_src_a,
                                    const armral_cmplx_int16_t *p_src_b,
                                    armral_cmplx_int16_t *c);

/** @} end of armral_corr_coeff group */

/**
 * @ingroup groupLowPhy
 */
/**
 * @addtogroup fir_filter FIR filter
 * @{
 * \brief FIR filter implemented for single-precision floating-point and 16-bit
 * signed integers.
 *
 * For example, given an input array `x`, an output array `y`, and a set of
 * coefficients `b`, the following is calculated:
 *
 * <pre>
 *  y[n] = b[0] x[N-1] +
 *         b[1] x[N-2] +
 *         ... +
 *         b[N-1] x[0]
 *       = &sum;<sub>i=0</sub><sup>N-1</sup> b[i] x[N-1-i]
 * </pre>
 *
 * The FIR coefficients are assumed to be reversed in memory, such that `b_N`
 * above is the first coefficient in memory rather than the last.
 */
/**
 * Computes a complex floating-point single-precision FIR filter.
 *
 * The `size` parameter, which is the length of the input array, must
 * be a multiple of four. Both the input array and the coefficients
 * array must be padded with zeros up to the next multiple of four.
 *
 * @param[in]     size      The number of complex samples in input.
 * @param[in]     taps      The number of taps of the FIR filter.
 * @param[in]     input     Points to the input samples buffer.
 * @param[in]     coeffs    Points to the coefficients array.
 * @param[out]    output    Points to the output array.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fir_filter_cf32(uint32_t size, uint32_t taps,
                                     const armral_cmplx_f32_t *input,
                                     const armral_cmplx_f32_t *coeffs,
                                     armral_cmplx_f32_t *output);

/**
 * Computes a complex floating-point single-precision FIR filter with
 * a decimation factor of two.
 *
 * The `size` parameter, which is the length of the input array before
 * decimation, must be a multiple of eight. The input array must be
 * padded with zeros up to the next multiple of eight, and the
 * coefficients array must be padded with zeros up to the next
 * multiple of four.
 *
 * @param[in]     size      The number of complex samples in input.
 * @param[in]     taps      The number of taps of the FIR filter.
 * @param[in]     input     Points to the input samples buffer.
 * @param[in]     coeffs    Points to the coefficients array.
 * @param[out]    output    Points to the output array.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fir_filter_cf32_decimate_2(
    uint32_t size, uint32_t taps, const armral_cmplx_f32_t *input,
    const armral_cmplx_f32_t *coeffs, armral_cmplx_f32_t *output);

/**
 * Computes a complex signed 16-bit integer FIR filter.
 *
 * The `size` parameter, which is the length of the input array, must
 * be a multiple of eight. Both the input array and the coefficients
 * array must be padded with zeros up to the next multiple of eight.
 *
 * @param[in]     size      The number of complex samples in input.
 * @param[in]     taps      The number of taps of the FIR filter.
 * @param[in]     input     Points to the input samples buffer.
 * @param[in]     coeffs    Points to the coefficients array.
 * @param[out]    output    Points to the output array.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fir_filter_cs16(uint32_t size, uint32_t taps,
                                     const armral_cmplx_int16_t *input,
                                     const armral_cmplx_int16_t *coeffs,
                                     armral_cmplx_int16_t *output);

/**
 * Computes a complex signed 16-bit integer FIR filter with a decimation factor
 * of two.
 *
 * The `size` parameter, which is the length of the input array before
 * decimation, must be a multiple of eight. The input array must be
 * padded with zeros up to the next multiple of eight, and the
 * coefficients array must be padded with zeros up to the next
 * multiple of four.
 *
 * @param[in]     size      The number of complex samples in input.
 * @param[in]     taps      The number of taps of the FIR filter.
 * @param[in]     input     Points to the input samples buffer.
 * @param[in]     coeffs    Points to the coefficients array.
 * @param[out]    output    Points to the output array.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fir_filter_cs16_decimate_2(
    uint32_t size, uint32_t taps, const armral_cmplx_int16_t *input,
    const armral_cmplx_int16_t *coeffs, armral_cmplx_int16_t *output);

/** @} end of fir_filter group */

/**
 * @ingroup groupDuRuInterface
 */
/**
 * @addtogroup muLaw Mu-Law Compression
 * @{
 * \brief The Mu-Law algorithm enables the compression of User Plane
 * (UP) data over the fronthaul interface.
 */
/**
 * The Mu-Law compression method combines a bit-shift operation for dynamic
 * range with a nonlinear piece-wise approximation of the original logarithmic
 * Mu-Law. The Mu-Law compression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 16-bit complex resource
 * elements. Each block taken as input is compressed into 12 complex output
 * samples, each 8 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale
 * values before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 8-bit data and exponent.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_compr_8bit(uint32_t n_prb,
                                       const armral_cmplx_int16_t *src,
                                       armral_compressed_data_8bit *dst,
                                       const armral_cmplx_int16_t *scale);

/**
 * The Mu-Law compression method combines a bit-shift operation for dynamic
 * range with a nonlinear piece-wise approximation of the original logarithmic
 * Mu-Law. The Mu-Law compression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 16-bit complex resource
 * elements. Each block taken as input is compressed into 12 complex output
 * samples, each 9 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale
 * values before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 9-bit data and shift.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_compr_9bit(uint32_t n_prb,
                                       const armral_cmplx_int16_t *src,
                                       armral_compressed_data_9bit *dst,
                                       const armral_cmplx_int16_t *scale);
/**
 * The Mu-Law compression method combines a bit-shift operation for dynamic
 * range with a nonlinear piece-wise approximation of the original logarithmic
 * Mu-Law. The Mu-Law compression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 16-bit complex resource
 * elements. Each block taken as input is compressed into 12 complex output
 * samples, each 14 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale
 * values before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 14-bit data and shift.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_compr_14bit(uint32_t n_prb,
                                        const armral_cmplx_int16_t *src,
                                        armral_compressed_data_14bit *dst,
                                        const armral_cmplx_int16_t *scale);

/**
 * The Mu-Law decompression method is a logical reverse function
 * of the compression method.
 * The Mu-Law decompression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 8-bit complex resource
 * elements. Each block taken as input is expanded into 12 complex output
 * samples, each 16 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 8-bit data and shift.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_decompr_8bit(uint32_t n_prb,
                                         const armral_compressed_data_8bit *src,
                                         armral_cmplx_int16_t *dst,
                                         const armral_cmplx_int16_t *scale);

/**
 * The Mu-Law decompression method is a logical reverse function
 * of the compression method.
 * The Mu-Law decompression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 9-bit complex resource
 * elements. Each block taken as input is expanded into 12 complex output
 * samples, each 16 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 9-bit data and shift.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_decompr_9bit(uint32_t n_prb,
                                         const armral_compressed_data_9bit *src,
                                         armral_cmplx_int16_t *dst,
                                         const armral_cmplx_int16_t *scale);

/**
 * The Mu-Law decompression method is a logical reverse function
 * of the compression method.
 * The Mu-Law decompression operates on `n_prb` Resource Blocks
 * (RB) of fixed size. Each block consists of 12 14-bit complex resource
 * elements. Each block taken as input is expanded into 12 complex output
 * samples, each 16 bits wide, and the shift applied to the block.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 14-bit data and shift.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_mu_law_decompr_14bit(
    uint32_t n_prb, const armral_compressed_data_14bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/** @} end of muLaw group */

/**
 * @ingroup groupDuRuInterface
 */
/**
 * @addtogroup  BlockScalingCompr Block Scaling Compression
 * @{
 * \brief Implements algorithms for data compression and decompression using
 * block scaling representation of complex samples.
 *
 */
/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 16-bit complex resource elements. Each block
 * taken as input is compressed into 24 8-bit post-scaled samples and a common
 * unsigned scaling factor.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 8-bit data and a scaling
 *                          factor.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_block_scaling_compr_8bit(uint32_t n_prb, const armral_cmplx_int16_t *src,
                                armral_compressed_data_8bit *dst,
                                const armral_cmplx_int16_t *scale);

/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 16-bit complex resource elements. Each block
 * taken as input is compressed into 24 9-bit post-scaled samples and a common
 * unsigned scaling factor.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 9-bit data and a scaling
 *                          factor.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status
armral_block_scaling_compr_9bit(uint32_t n_prb, const armral_cmplx_int16_t *src,
                                armral_compressed_data_9bit *dst,
                                const armral_cmplx_int16_t *scale);

/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 16-bit complex resource elements. Each block
 * taken as input is compressed into 24 14-bit post-scaled samples and a common
 * unsigned scaling factor.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb The number of input resource blocks.
 * @param[in]     src   Points to the input complex samples sequence.
 * @param[out]    dst   Points to the output 14-bit data and a scaling factor.
 * @param[in]     scale Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_scaling_compr_14bit(
    uint32_t n_prb, const armral_cmplx_int16_t *src,
    armral_compressed_data_14bit *dst, const armral_cmplx_int16_t *scale);

/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 8-bit complex post-scaled resource elements
 * and an unsigned scaling factor. Each block taken as input is expanded into 12
 * 16-bit complex samples.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 8-bit data and scaling factor.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_scaling_decompr_8bit(
    uint32_t n_prb, const armral_compressed_data_8bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 9-bit complex post-scaled resource elements
 * and an unsigned scaling factor. Each block taken as input is expanded into 12
 * 16-bit complex samples.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 9-bit data and a scaling factor.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_scaling_decompr_9bit(
    uint32_t n_prb, const armral_compressed_data_9bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/**
 * The algorithm operates on a fixed block size of one Physical Resource Block
 * (PRB). Each block consists of 12 14-bit complex post-scaled resource elements
 * and an unsigned scaling factor. Each block taken as input is expanded into 12
 * 16-bit complex samples.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input 14-bit data and a scaling
 *                          factor.
 * @param[out]    dst       Points to the output complex samples sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_scaling_decompr_14bit(
    uint32_t n_prb, const armral_compressed_data_14bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/** @} end of BlockScalingCompr  group */

/**
 * @ingroup groupDuRuInterface
 */
/**
 * @addtogroup BlockFloatCompr Block Floating Point
 * @{
 * \brief Implements algorithms for data compression and decompression through
 * block floating-point representation of complex samples.
 */
/**
 * @brief         Block floating-point compression to 8-bit
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 16-bit complex resource elements. Each block taken
 * as input is compressed into 24 8-bit samples and one unsigned exponent.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 8-bit data and exponent.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_compr_8bit(uint32_t n_prb,
                                            const armral_cmplx_int16_t *src,
                                            armral_compressed_data_8bit *dst,
                                            const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point compression to 9-bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 16-bit complex resource elements. Each block taken
 * as input is compressed into 24 9-bit big-endian samples and one unsigned
 * exponent. Big-endian means that where data from a 9-bit element is split
 * across multiple bytes, the most significant bits are stored in the output
 * byte with lowest address, and remaining bits are stored in the high bits of
 * the next output byte.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 9-bit data and exponent.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_compr_9bit(uint32_t n_prb,
                                            const armral_cmplx_int16_t *src,
                                            armral_compressed_data_9bit *dst,
                                            const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point compression to 12-bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 16-bit complex resource elements. Each block taken
 * as input is compressed into 24 12-bit big-endian samples and one unsigned
 * exponent. Big-endian means that where data from a 12-bit element is split
 * across multiple bytes, the most significant bits are stored in the output
 * byte with lowest address, and remaining bits are stored in the high bits of
 * the next output byte.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 12-bit data and exponent.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_compr_12bit(uint32_t n_prb,
                                             const armral_cmplx_int16_t *src,
                                             armral_compressed_data_12bit *dst,
                                             const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point compression to 14-bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 16-bit complex resource elements. Each block taken
 * as input is compressed into 24 14-bit big-endian samples and one unsigned
 * exponent. Big-endian means that where data from a 14-bit element is split
 * across multiple bytes, the most significant bits are stored in the output
 * byte with lowest address, and remaining bits are stored in the high bits of
 * the next output byte.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * before compression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input complex samples sequence.
 * @param[out]    dst       Points to the output 14-bit data and exponent.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_compr_14bit(uint32_t n_prb,
                                             const armral_cmplx_int16_t *src,
                                             armral_compressed_data_14bit *dst,
                                             const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating-point decompression from 8 bit
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 8-bit complex resource elements and an unsigned
 * exponent. Each block taken as input is expanded into 12 16-bit complex
 * samples.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input compressed block sequence.
 * @param[out]    dst       Points to the complex output sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_decompr_8bit(
    uint32_t n_prb, const armral_compressed_data_8bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point decompression from 9 bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 9-bit big-endian complex resource elements and an
 * unsigned exponent. Each block taken as input is expanded into 12 16-bit
 * complex samples. Big-endian here means that where data from a 9-bit element
 * is split across multiple bytes, the most significant bits are stored in the
 * output byte with lowest address, and remaining bits are stored in the high
 * bits of the next output byte.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input compressed block sequence.
 * @param[out]    dst       Points to the complex output sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_decompr_9bit(
    uint32_t n_prb, const armral_compressed_data_9bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point decompression from 12 bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 12-bit big-endian complex resource elements and an
 * unsigned exponent. Each block taken as input is expanded into 12 16-bit
 * complex samples. Big-endian here means that where data from a 12-bit element
 * is split across multiple bytes, the most significant bits are stored in the
 * output byte with lowest address, and remaining bits are stored in the high
 * bits of the next output byte.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input compressed block sequence.
 * @param[out]    dst       Points to the complex output sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_decompr_12bit(
    uint32_t n_prb, const armral_compressed_data_12bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/**
 * @brief         Block floating point decompression from 14 bit big-endian
 *
 * The algorithm operates on a fixed block size of one Resource Block (RB).
 * Each block consists of 12 14-bit big-endian complex resource elements and an
 * unsigned exponent. Each block taken as input is expanded into 12 16-bit
 * complex samples. Big-endian here means that where data from a 14-bit element
 * is split across multiple bytes, the most significant bits are stored in the
 * output byte with lowest address, and remaining bits are stored in the high
 * bits of the next output byte.
 *
 * A phase-compensation factor, stored in `*scale`, is used to scale values
 * after decompression in the case that `scale` is non-NULL.
 *
 * @param[in]     n_prb     The number of input resource blocks.
 * @param[in]     src       Points to the input compressed block sequence.
 * @param[out]    dst       Points to the complex output sequence.
 * @param[in]     scale     Phase compensation term to use, or NULL.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_block_float_decompr_14bit(
    uint32_t n_prb, const armral_compressed_data_14bit *src,
    armral_cmplx_int16_t *dst, const armral_cmplx_int16_t *scale);

/** @} end of BlockFloatCompr group */

/**
 * @ingroup groupUpPhy
 */
/**
 * @addtogroup crc CRC
 * @{
 * \brief Computes a Cyclic Redundancy Check (CRC) of an input buffer
 * using carry-less multiplication and Barret reduction.
 *
 * <pre>
 *  CRC24A polynomial = x^24 + x^23 + x^18 + x^17 + x^14 + x^11 + x^10 + x^7 +
 *                      x^6 + x^5 + x^4 + x^3 + x + 1
 *  CRC24B polynomial = x^24 + x^23 + x^6 + x^5 + x + 1
 *  CRC24C polynomial = x^24 + x^23 + x^21 + x^20 + x^17 + x^15 + x^13 + x^12 +
 *                      x^8 + x^4 + x^2 + x + 1
 *  CRC16 polynomial  = x^16 + x^12 + x^5 + 1
 *  CRC11 polynomial  = x^11 + x^10 + x^9 + x^5 + 1
 *  CRC6 polynomial   = x^6 + x^5 + 1
 * </pre>
 *
 * The input buffer is assumed to be padded to at least 8 bytes. If the input
 * size is greater than 8 bytes, then padding to a multiple of 16 bytes
 * (128 bits) is assumed.
 *
 * Both little-endian and big-endian orderings are provided, using the `le` and
 * `be` suffixes, respectively.
 */
/**
 * Computes the CRC24 of an input buffer using the `CRC24A` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_a_le(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC24 of an input buffer using the `CRC24A` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_a_be(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC24 of an input buffer using the `CRC24B` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_b_le(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC24 of an input buffer using the `CRC24B` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_b_be(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC24 of an input buffer using the `CRC24C` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_c_le(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC24 of an input buffer using the `CRC24C` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc24     The computed 24-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc24_c_be(uint32_t size, const uint64_t *input,
                                uint64_t *crc24);

/**
 * Computes the CRC16 of an input buffer using the `CRC16` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc16     The computed 16-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc16_le(uint32_t size, const uint64_t *input,
                              uint64_t *crc16);

/**
 * Computes the CRC16 of an input buffer using the `CRC16` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc16     The computed CRC on 16 bit.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc16_be(uint32_t size, const uint64_t *input,
                              uint64_t *crc16);

/**
 * Computes the CRC11 of an input buffer using the `CRC11` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc11     The computed 11-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc11_le(uint32_t size, const uint64_t *input,
                              uint64_t *crc11);

/**
 * Computes the CRC11 of an input buffer using the `CRC11` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc11     The computed CRC on 11 bit.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc11_be(uint32_t size, const uint64_t *input,
                              uint64_t *crc11);

/**
 * Computes the CRC6 of an input buffer using the `CRC6` polynomial.
 * Blocks of 64 bits are interpreted using little-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc6     The computed 6-bit CRC result.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc6_le(uint32_t size, const uint64_t *input,
                             uint64_t *crc6);

/**
 * Computes the CRC6 of an input buffer using the `CRC6` polynomial.
 * Blocks of 64 bits are interpreted using big-endian ordering.
 *
 * @param[in]     size      The number of bytes of the given buffer.
 * @param[in]     input     Points to the input byte sequence.
 * @param[out]    crc6     The computed CRC on 6 bit.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_crc6_be(uint32_t size, const uint64_t *input,
                             uint64_t *crc6);

/** @} end of CRC group */

/**
 * @ingroup groupUpPhy
 */
/**
 * @addtogroup polar Polar encoding
 * @{
 * \brief In uplink, Polar codes are used to encode the Uplink Control
 * Information (UCI) over the Physical Uplink Control Channel (PUCCH) and
 * Physical Uplink Shared Channel (PUSCH). In downlink, Polar codes are used to
 * encode the Downlink Control Information (DCI) over the Physical Downlink
 * Control Channel (PDCCH).
 *
 * By construction, Polar codes only allow code lengths that are powers of two
 * (`N` = `2^n`). The number of input information bits, `K`, can take any
 * arbitrary value up to the maximum value of `N` (`K` <= `N`). In particular,
 * 5G NR restricts the usage of Polar codes length from `N` = 32 bits to `N` =
 * 1024 bits. For `N` < 32, other types of channel coding are performed.
 *
 * Given the input sequence vector `[u]  = [u(0), u(1), ..., u(N-1)]`, if index
 * `i` is included in the `frozen` bits set, then `u(i) = 0`. The input
 * information bits are stored in the remaining entries.
 * `[d]  = [d(0), d(1), ..., d(N-1)]` is the vector of output encoded bits.
 * `[G_N]` is the channel transformation matrix (`N-by-N`), obtained by
 * recursively applying the Kronecker product from the basic kernel `G_2 = |1 0;
 * 1 1|` to the order `n = log2(N)`.
 *
 * The output after encoding, `[d]`, is obtained by `[d] = [u]*[G_N]`.
 *
 * For more information, refer to the 3GPP Technical Specification (TS) 38.212
 * V16.0.0 (2019-12).
 */

/**
 * Computes the `frozen` bits mask used for encoding and decoding a Polar code.
 *
 * The mask is formatted as an array of `uint8_t` elements, where each byte
 * element describes the corresponding bit index in the Polar-encoded message.
 * After \link armral_polar_subchannel_interleave \endlink, the value of each
 * bit in the interleaved message is set based on the corresponding byte index
 * of the `frozen` mask. The exact behavior of possible values in the `frozen`
 * mask is described by `armral_polar_frozen_bit_type`.
 *
 * The `armral_polar_frozen_mask` function takes both the number of information
 * bits and the number of parity bits separately, because the number of parity
 * bits does not depend exactly on `K` or `E`, but also depends on if you are
 * coding for the uplink or downlink. The downlink always has zero parity bits.
 *
 * The values of the input parameters must satisfy `K + n_pc < N` and satisfy
 * `K + n_pc < E`. The possible values of `n_pc` and `n_pc_wm` are described in
 * section 6.3.1.3.1 of the 3GPP Technical Specification (TS) 38.212: `n_pc`
 * must be either 0 or 3, `n_pc_wm` must be either 0 or 1, and `n_pc >= n_pc_wm`
 * must also be true.
 *
 * @param[in]  n        The Polar code length in bits, must be a power of 2.
 * @param[in]  e        The encoded code length in bits, after rate-matching
 *                      (either shortening, puncturing or repetition).
 * @param[in]  k        The number of information bits in the encoded message,
 *                      the sum of the message and CRC bits (`K = A + L`).
 * @param[in]  n_pc     The number of parity bits in the encoded message.
 * @param[in]  n_pc_wm  The number of row-weight-selected parity bits in the
 *                      encoded message. Must be either zero or one.
 * @param[out] frozen   The output `frozen` mask, length `n` bytes. Elements
 *                      corresponding to `frozen` bits are set to all ones,
 *                      everything else set to zero.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_frozen_mask(uint32_t n, uint32_t e, uint32_t k,
                                       uint32_t n_pc, uint32_t n_pc_wm,
                                       uint8_t *frozen);

/**
 * The `armral_polar_subschannel_interleave` function performs subchannel
 * allocation. To calculate the `u` bit array, as specified in section 5.3.1.2
 * of the 3GPP Technical Specification (TS) 38.212, the function interleaves the
 * supplied input bit array `c` into a larger output bit array. `c` interleaves
 * into positions where the `frozen` mask indicates an information bit is
 * present.
 *
 * For a particular underlying Polar code of length `N` bits (`N` must be a
 * power of two between 32 and 1024 inclusive), the `frozen` mask must be an
 * array of length `N` bytes. By the nature of Polar coding, `K′ ≤ N` must be
 * true.
 *
 * @param[in]  n       The Polar code size `N`.
 * @param[in]  kplus   The number of information bits plus the number of parity
 *                     bits: `K′ = K + n_pc`.
 * @param[in]  frozen  Points to the `frozen` bits mask given by \link
 *                     armral_polar_frozen_mask \endlink .
 * @param[in]  c       The input codeword, of length `K` bits.
 * @param[out] u       The output codeword including `frozen` and parity bits,
 *                     of length `N` bits.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_subchannel_interleave(uint32_t n, uint32_t kplus,
                                                 const uint8_t *frozen,
                                                 const uint8_t *c, uint8_t *u);

/**
 * The `armral_polar_subchannel_deinterleave` function performs the inverse of
 * subchannel allocation. To calculate the `c` bit array, as specified in
 * section 5.3.1.2 of the 3GPP Technical Specification (TS) 38.212, the function
 * deinterleaves the supplied input bit array `u` into a smaller output bit
 * array. Bits stored in `u` are taken from `c` at indices where the `frozen`
 * mask indicates an information bit is present. The bits at the remaining
 * `frozen` mask bit indices are ignored.
 *
 * For a particular underlying Polar code of length `N` bits (`N` must be a
 * power of two between 32 and 1024 inclusive), the `frozen` mask must be an
 * array of length `N` bytes. By the nature of Polar coding, `K ≤ N` must be
 * true.
 *
 * @param[in]  k       The number of information bits, not including the number
 *                     of parity bits.
 * @param[in]  frozen  Points to the `frozen` bits mask given by \link
 *                     armral_polar_frozen_mask \endlink .
 * @param[in]  u       The input decoded codeword, including `frozen` and parity
 *                     bits, of length `N` bits.
 * @param[out] c       The output codeword, of length `K` bits.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_subchannel_deinterleave(uint32_t k,
                                                   const uint8_t *frozen,
                                                   const uint8_t *u,
                                                   uint8_t *c);

/**
 * Encodes the specified sequence of `n` input bits using Polar encoding.
 *
 * @param[in]    n            The Polar code length in bits, where
 *                            `n` must be a power of 2.
 * @param[in]    p_u_seq_in   Points to the input sequence `[u]` of bits
 *                            `[u(0), u(1), ..., u(N-1)]`.
 * @param[out]   p_d_seq_out  Points to the output encoded sequence `[d]` of
 *                            bits `[d(0), d(1), ..., d(N-1)]`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_encode_block(uint32_t n, const uint8_t *p_u_seq_in,
                                        uint8_t *p_d_seq_out);

/**
 * Decodes `k` real information bits from a Polar-encoded message of length `n`,
 * given as input as a sequence of 8-bit log-likelihood ratios. The number of
 * information bits `k` itself is not needed for the `armral_polar_decode_block`
 * function itself, since computing the `frozen` bits mask is handled elsewhere
 * in \link armral_polar_frozen_mask \endlink.
 *
 * If `l=1`, the decoder uses a Successive Cancellation (SC) method. If `l>1`,
 * the decoder uses a Successive Cancellation List (SCL) method instead. `l`
 * candidate codewords are maintained and returned, sorted by worsening path
 * metric (in other words, the first returned value is the most likely to be
 * correct). Not all list sizes are supported. Unsupported values of `n` or `l`
 * will return `ARMRAL_ARGUMENT_ERROR`.
 *
 * @param[in]  n             The Polar code length in bits, must be a power of 2
 * @param[in]  frozen        Points to the `frozen` bits mask given by \link
 *                           armral_polar_frozen_mask \endlink .
 * @param[in]  l             The list size to be used in decoding.
 * @param[in]  p_llr_in      Points to the input sequence of LLR bytes.
 * @param[out] p_u_seq_out   Points to `l` decoded sequences, ordered by
 *                           decreasing path metric, each of length `n` bits.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_decode_block(uint32_t n, const uint8_t *frozen,
                                        uint32_t l, const int8_t *p_llr_in,
                                        uint8_t *p_u_seq_out);

/**
 * Matches the rate of the Polar encoded code block to the rate of the channel
 * using sub-block interleaving, bit selection, and channel interleaving. This
 * is as described in the 3GPP Technical Specification (TS) 38.212 section
 * 5.4.1.1.
 *
 * The code rate of the code block is defined by the ratio of the rate-matched
 * length `e` to the number of information bits in the message `k`. It is
 * assumed that `e` is strictly greater than `k`. Given a rate-matched length
 * and number of information bits, the code block length is determined as
 * described in section 5.3.1 of TS 38.212.
 *
 * @param[in]  n           The number of bits in the code block.
 * @param[in]  e           The number of bits in the rate-matched message.
 * @param[in]  k           The number of information bits in the code block.
 * @param[in]  p_d_seq_in  Points to `n` bits representing the Polar encoded
 *                         message.
 * @param[out] p_f_seq_out Points to `e` bits representing the rate-matched
 *                         message.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_rate_matching(uint32_t n, uint32_t e, uint32_t k,
                                         const uint8_t *p_d_seq_in,
                                         uint8_t *p_f_seq_out);

/**
 * Recovers the log-likelihood ratios (LLRs) from demodulation to match the
 * length of Polar code blocks using channel deinterleaving, bit recovery, and
 * sub-block deinterleaving. These operations are the inverse of channel
 * interleaving, bit selection, and sub-block interleaving used in Polar rate
 * matching, which is described in the 3GPP Technical Specification (TS)
 * 38.212 section 5.4.1.1.
 *
 * The size of the code block is given in section 5.3.1 of TS 38.212, and is
 * related to the ratio of the rate matched length `e` and information bits per
 * code block `k` according to clause 5.4.1 of TS 38.212.
 *
 * The code rate of the code block is the ratio of the rate-matched length `e`
 * to the number of information bits in the message `k`.
 *
 * @param[in]  e          The length of the rate-matched message. This is also
 *                        the number of LLRs in the demodulated message.
 * @param[in]  k          The number of information bits in the message to
 *                        recover. The ratio of `e/k` is the rate of the
 *                        transmitted code block. `e` is assumed to be strictly
 *                        greater than `k`.
 * @param[in]  n          The number of bits in the code block. Defined in
 *                        section 5.3.1 of TS 38.212.
 * @param[in]  p_llr_in   Points to `e` 8-bit LLRs, which are assumed to be the
 *                        output of demodulation.
 * @param[out] p_llr_out  Points to `n` 8-bit rate-recovered LLRs. This output
 *                        can be passed as input to Polar decoding.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_polar_rate_recovery(uint32_t e, uint32_t k, uint32_t n,
                                         const int8_t *p_llr_in,
                                         int8_t *p_llr_out);

/** @} end of Polar group */

/**
 * @ingroup groupLowPhy
 */
/**
 * @addtogroup fft Fast Fourier Transforms (FFT)
 * @{
 * \brief Computes the Discrete Fourier Transform (DFT) of a sequence (forwards
 * transform), or the inverse (backwards transform).
 *
 * FFT plans are represented by an opaque structure. To fill the plan
 * structure, define a pointer to the structure and call \link
 * armral_fft_create_plan_cf32 \endlink or \link armral_fft_create_plan_cs16
 * \endlink. For example:
 * <pre>
 * armral_fft_plan_t *plan;
 * armral_fft_create_plan_cf32(&plan, 32, ARMRAL_FFT_FORWARDS);
 * armral_fft_execute_cf32(plan, x, y);
 * armral_fft_destroy_plan_cf32(&plan);
 * </pre>
 */
/**
 * The opaque structure to an FFT plan. You must fill an FFT plan before you
 * use it. To fill an FFT plan, call \link armral_fft_create_plan_cf32 \endlink
 * or \link armral_fft_create_plan_cs16 \endlink.
 */
typedef struct armral_fft_plan_t armral_fft_plan_t;

/**
 * The direction of the FFT being computed. The direction is passed to \link
 * armral_fft_create_plan_cf32 \endlink and \link armral_fft_create_plan_cs16
 * \endlink.
 */
typedef enum {
  ARMRAL_FFT_FORWARDS = -1, ///< Compute a forwards (non-inverse) FFT.
  ARMRAL_FFT_BACKWARDS = 1, ///< Compute a backwards (inverse) FFT.
} armral_fft_direction_t;

/**
 * @brief         Creates a plan to solve a complex fp32 FFT.
 *
 * Fills the passed pointer with a pointer to the plan that is created. The plan
 * that is created can then be used to solve problems with specified size and
 * direction. It is efficient to create plans once and reuse them, rather than
 * creating a plan for every execute call. For some inputs, creating FFT plans
 * can incur a significant overhead.
 *
 * To avoid memory leaks, call \link armral_fft_destroy_plan_cf32 \endlink when
 * you no longer need this plan.
 *
 * @param[in,out] p  A pointer to the resulting plan pointer. On output `*p` is
 *                   a valid pointer, to be passed to
 *                   \link armral_fft_execute_cf32 \endlink.
 * @param[in] n      The problem size to be solved by this FFT plan.
 * @param[out] dir   The direction to be solved by this FFT plan.
 * @return     An \c armral_status value that indicates success or failure
 */
armral_status armral_fft_create_plan_cf32(armral_fft_plan_t **p, int n,
                                          armral_fft_direction_t dir);

/**
 * @brief         Performs a single FFT using the specified plan and arrays.
 *
 * Uses the plan created by \link armral_fft_create_plan_cf32 \endlink to
 * perform the configured FFT with the arrays that are specified.
 *
 * @param[in] p    A pointer to the FFT plan. The pointer is the value that is
 *                 filled in by an earlier call to
 *                 \link armral_fft_create_plan_cf32 \endlink.
 * @param[in] x    The input array for this FFT. The length must be the same as
 *                 the value of `n` that was previously passed to
 *                 \link armral_fft_create_plan_cf32 \endlink.
 * @param[out] y   The output array for this FFT. The length must be the same as
 *                 the value of `n` that was previously passed to
 *                 \link armral_fft_create_plan_cf32 \endlink.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fft_execute_cf32(const armral_fft_plan_t *p,
                                      const armral_cmplx_f32_t *x,
                                      armral_cmplx_f32_t *y);

/**
 * @brief         Destroys an FFT plan.
 *
 * The \link armral_fft_execute_cf32 \endlink function frees any associated
 * memory, and sets `*p = NULL`, for a plan that was previously created by
 * \link armral_fft_create_plan_cf32 \endlink.
 *
 * @param[in,out] p  A pointer to the FFT plan pointer. The pointer must be the
 *                   value that is returned by an earlier call to
 *                   \link armral_fft_create_plan_cf32 \endlink. On function
 *                   exit, the value that is pointed to is set to `NULL`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fft_destroy_plan_cf32(armral_fft_plan_t **p);

/**
 * @brief         Creates a plan to solve a complex int16 (Q0.15 format) FFT.
 *
 * Fills the passed pointer with a pointer to the plan that is created. The plan
 * that is created can then be used to solve problems with specified size and
 * direction. It is efficient to create plans once and reuse them, rather than
 * creating a plan for every execute call. For some inputs, creating FFT plans
 * can incur a significant overhead.
 *
 * To avoid memory leaks, call \link armral_fft_destroy_plan_cs16 \endlink when
 * you no longer need this plan.
 *
 * @param[in,out] p  A pointer to the resulting plan pointer. On output `*p` is
 *                   a valid pointer, to be passed to
 *                   \link armral_fft_execute_cs16 \endlink.
 * @param[in] n      The problem size to be solved by this FFT plan.
 * @param[out] dir   The direction to be solved by this FFT plan.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fft_create_plan_cs16(armral_fft_plan_t **p, int n,
                                          armral_fft_direction_t dir);

/**
 * @brief         Performs a single FFT using the specified plan and arrays.
 *
 * Uses the plan created by \link armral_fft_create_plan_cs16 \endlink to
 * perform the configured FFT with the arrays that are specified.
 *
 * @param[in] p    A pointer to the FFT plan. The pointer is the value that is
 *                 filled in by an earlier call to
 *                 \link armral_fft_create_plan_cs16 \endlink.
 * @param[in] x    The input array for this FFT. The length must be the same as
 *                 the value of `n` that was previously passed to
 *                 \link armral_fft_create_plan_cs16 \endlink.
 * @param[out] y   The output array for this FFT. The length must be the same as
 *                 the value of `n` that was previously passed to
 *                 \link armral_fft_create_plan_cs16 \endlink.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fft_execute_cs16(const armral_fft_plan_t *p,
                                      const armral_cmplx_int16_t *x,
                                      armral_cmplx_int16_t *y);

/**
 * @brief         Destroys an FFT plan.
 *
 * The \link armral_fft_execute_cs16 \endlink function frees any associated
 * memory, and sets `*p = NULL`, for a plan that was previously created by
 * \link armral_fft_create_plan_cs16 \endlink.
 *
 * @param[in,out] p  A pointer to the FFT plan pointer. The pointer must be the
 *                   value that is returned by an earlier call to
 *                   \link armral_fft_create_plan_cs16 \endlink. On function
 *                   exit, the value that is pointed to is set to `NULL`.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_fft_destroy_plan_cs16(armral_fft_plan_t **p);

/** @} end fft */

/**
 * @ingroup groupUpPhy
 */
/**
 * @addtogroup ldpc Low-Density Parity Check (LDPC)
 * @{
 * \brief Performs encoding and decoding of data using Low-density Parity Check
 * (LDPC) methods. The implementation is described in the 3GPP Technical
 * Specification (TS) 38.212, in sections 5.2.2 and 5.3.2.
 *
 * Encoding of a single block is supported. Depending on the rate matching
 * applied to a signal, one of two base graphs are used when creating an LDPC
 * encoding. Concepts of rate matching are not included, but the implementation
 * provided does take the graph as input to be able to perform different
 * encoding operations.
 *
 * A base graph is described by a sparse matrix, in which each non-zero entry
 * indicates the presence of a shifted identity matrix. The size of the matrix
 * is denoted by `z` and depends on the size of the message to encode. `z` is
 * referred to as the lifting size, and a lifting size belongs to a particular
 * lifting set (indices from 0 to 7). The amount each identity matrix is shifted
 * by depends on the lifting set index.
 */
/**
 * Identifies the base graph to use in LDPC encoding and decoding. The base
 * graphs are defined in tables 5.3.2-2 and 5.3.2-3 in the 3GPP Technical
 * Specification (TS) 38.212.
 */
typedef enum {
  LDPC_BASE_GRAPH_1, ///< Identifier for LDPC base graph 1.
  LDPC_BASE_GRAPH_2  ///< Identifier for LDPC base graph 2.
} armral_ldpc_graph_t;

/**
 * \brief Data structure required to store the data in a Low Density Parity
 * Check (LDPC) base graph. The data of a base graph is stored in Compressed
 * Sparse Row (CSR) format.
 */
typedef struct {
  ///
  /// The number of rows in the base graph.
  uint32_t nrows;

  /// The number of columns in the base graph which are associated with message
  /// bits. Punctured columns are included.
  uint32_t nmessage_bits;

  /// The number of block columns that are in the codeword. `ncodeword_bits` is
  /// the number of columns in the base graph minus the two punctured columns.
  uint32_t ncodeword_bits;

  /// The indices of the start of a row in the base graph, which you can use to
  /// index into the `col_inds` array to get the column indices of the non-zero
  /// entries in a row of the base graph.
  const uint32_t *row_start_inds;

  /// The indices of the non-zero columns in the base graph. Each of the entries
  /// in a row are stored contiguously. The start of a row is identified by
  /// indices stored in the `row_start_inds` array. For example, the start of
  /// row with index (zero-based) `2` is at index `row_start_inds[2]`.
  const uint32_t *col_inds;

  /// The shifts applied to the identity matrix to give the matrix at each
  /// non-zero column in the base graph. The shifts for all lifting sets are
  /// stored in this array. All shifts for one lifting set are stored before the
  /// next lifting set. This means that the shifts for lifting set with index
  /// (zero-based) `3`, and row with index `5` is at index
  /// `(row_start_inds[5] + 3) * 8`, where `8` is the number of lifting
  /// sets.
  const uint32_t *shifts;
} armral_ldpc_base_graph_t;

/**
 * Uses the identifier of a base graph to get the data structure that describes
 * a base graph.
 *
 * @param[in] bg Enum identifier of the base graph to get.
 * @return A pointer to an LDPC base graph.
 */
const armral_ldpc_base_graph_t *
armral_ldpc_get_base_graph(armral_ldpc_graph_t bg);

/**
 * Performs encoding using LDPC as layed out in the 3GPP Technical Specification
 * (TS) 38.212. Encoding is performed for a single code block.
 *
 * The length of the code block is determined from the lifting size `z` and
 * base graph. For example, for base graph 1 the length of a code block is
 * `68 * z` bits, and for base graph 2 the length of the code block is `52 * z`
 * bits.  The output from the encoding begins at the third column of the base
 * graph.  The first two columns are punctured, as per section 5.3.2 of TS
 * 38.212. The number of encoded bits returned from this function is `66 * z`
 * for base graph 1, and `50 * z` for base graph 2. The values of `z` are
 * limited to those in table 5.3.2-1 in TS 38.212.
 *
 * The number of information bits in a code block is determined by the lifting
 * size and base graph. For base graph 1 the number of information bits per
 * code block is `22 * z`. For base graph 2 the number of information bits per
 * code block is `10 * z`. It is assumed that the correct number of input bits
 * is passed into this routine.
 *
 * @param[in] data_in         The information bits to encode. It is assumed
 *                            that the number of bits stored in `data_in` fits
 *                            into a single code block. The number of
 *                            information bits is assumed to be `22 * z` for
 *                            base graph 1, and `10 * z` for base graph 2.
 * @param[in] bg              Identifier for the base graph to use for
 *                            encoding. TS 38.212 defines two base graphs in
 *                            table 5.3.2-2 and 5.3.2-3. The base graph, in
 *                            combination with the lifting size `z`, determines
 *                            the block size and the graph to use for encoding
 *                            the block.
 * @param[in] z               The lifting size. Valid values of the lifting
 *                            size are described in table 5.3.2-1 in TS 38.212.
 * @param[out] data_out       The codeword to be transmitted. `data_out` has the
 *                            first two columns for the base graphs punctured,
 *                            and contains the information and calculated
 *                            parity bits after encoding.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_ldpc_encode_block(const uint8_t *data_in,
                                       armral_ldpc_graph_t bg, uint32_t z,
                                       uint8_t *data_out);

/**
 * Performs decoding of LDPC using a layered min-sum algorithm. This is an
 * iterative algorithm which takes 8-bit log-likelihood ratios (LLRs) and
 * calculates the most likely codeword by calculating updates using information
 * available from the parity checks in the LDPC graph. LLRs are updated after
 * evaluating checks in a 'layer', where a layer is assumed to contain the same
 * number of checks as the lifting size `z`. There are 46 layers in base graph
 * 1, and 42 layers in base graph 2. Decoding is performed for a single code
 * block.
 *
 * @param[in]  llrs     The initial LLRs to use in the decoding. This is
 *                      typically the output after demodulation and rate
 *                      recovery.
 * @param[in]  bg       The type of base graph to use for the decoding.
 * @param[in]  z        The lifting size. Valid values of the lifting size are
 *                      described in table 5.3.2-1 in TS 38.212.
 * @param[in]  num_its  The maximum number of iterations of the LDPC decoder to
 *                      run. The algorithm may terminate after fewer iterations
 *                      if the current candidate codeword passes all the parity
 *                      checks.
 * @param[out] data_out The decoded bits. These are of length `68 * z` for base
 *                      graph 1 and `52 * z` for base graph 2. It is assumed
 *                      that the array `data_out` is able to store this many
 *                      bits.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_ldpc_decode_block(const int8_t *llrs,
                                       armral_ldpc_graph_t bg, uint32_t z,
                                       uint32_t num_its, uint8_t *data_out);

/**
 * Matches the rate of the code block encoded with LDPC code to the rate of the
 * channel using bit selection and bit interleaving. This is as described in
 * the 3GPP Technical Specification (TS) 38.212 section 5.4.2.
 *
 * The input to the rate matching is assumed to be the output from LDPC
 * encoding for a single code block. The output from rate matching is to be
 * passed to demodulation.
 *
 * The code rate for a given code block is the ratio of rate matched length `e`
 * to the number of information bits per code block. The number of information
 * bits is assumed to be `22 * z` for base graph 1, and `10 * z` for base graph
 * 2, where `z` is the lifting size. It is assumed that `e` is strictly greater
 * than the number of information bits in a code block. `e` must also be a
 * multiple of the modulation order (i.e. the number of bits per modulation
 * symbol).
 *
 * @param[in]  bg   The type of base graph for which rate matching is to be
 *                  performed.
 * @param[in]  z    The lifting size. Valid values of the lifting size are
 *                  described in table 5.3.2-1 in TS 38.212.
 * @param[in]  e    The number of bits in the rate-matched message. This is
 *                  assumed to be a multiple of the number of bits per
 *                  modulation symbol.
 * @param[in]  rv   Redundancy version used in rate matching. Must be in the
 *                  set `{0, 1, 2, 3}`. The effect of choosing different
 *                  redundancy versions is described in table 5.4.2.1-2 of TS
 *                  38.212.
 * @param[in]  mod  The type of modulation to perform. Required to perform
 *                  bit-interleaving, as described in section 5.4.2 of TS
 *                  38.212.
 * @param[in]  src  Input array. This is assumed to be the output of LDPC
 *                  encoding. This contains `66 * z` bits in the case that base
 *                  graph 1 is used, and `50 * z` bits in the case that base
 *                  graph 2 is used.
 * @param[out] dst  Contains `e` bits of data, which is the rate-matched data
 *                  ready to be passed to modulation.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_ldpc_rate_matching(armral_ldpc_graph_t bg, uint32_t z,
                                        uint32_t e, uint32_t rv,
                                        armral_modulation_type mod,
                                        const uint8_t *src, uint8_t *dst);

/**
 * Recovers the log-likelihood ratios (LLRs) from demodulation to match the
 * length of an LDPC code block. This is the inverse of the operations for rate
 * matching for LDPC described in the 3GPP Technical Specification (TS) 38.212
 * section 5.4.2. The input array is of length `e` bytes, where `e` is the
 * rate-matched length of the code block. It is assumed that `e` is a multiple
 * of the modulation order (i.e. the number of bits per modulation symbol).
 *
 * The size of the code block is determined using the base graph and lifting
 * size `z`. For base graph 1, the code block is of length `68 * z`. For base
 * graph 2, the code block is of length `52 * z`. The output of the rate
 * recovery will be of length `66 * z` for base graph 1, and `50 * z` for base
 * graph 2, as it is assumed that the first two information columns are
 * punctured.
 *
 * The rate of the code block is the ratio of the rate matched length `e` and
 * the number of information bits in the code block. The number of information
 * bits in the code block is `22 * z` for base graph 1, and `10 * z` for base
 * graph 2.
 *
 * The output array also serves as an input array. It contains the current
 * approximation to LLRs. The LLRs calculated from the rate-recovery are summed
 * to existing LLRs in the output array.
 *
 * @param[in]  bg      The type of base graph for which rate recovery is to be
 *                     performed.
 * @param[in]  z       The lifting size. Valid values of the lifting size are
 *                     described in table 5.3.2-1 in TS 38.212.
 * @param[in]  e       The number of LLRs in the demodulated message. Assumed
 *                     to be a multiple of the number of bits per modulation
 *                     symbol.
 * @param[in]  rv      Redundancy version used in rate recovery. Must be in the
 *                     set `{0, 1, 2, 3}`. The effect of choosing different
 *                     redundancy versions is described in table 5.4.2.1-2 of
 *                     TS 38.212.
 * @param[in]  mod     The type of modulation which was performed. Required to
 *                     perform bit-deinterleaving as the inverse of the
 *                     bit-interleaving described in section 5.4.2 of TS
 *                     38.212.
 * @param[in]  src     Input array of a total of `e` 8-bit LLRs. This is the
 *                     output after demodulation.
 * @param[in,out] dst  On entry, contains the current approximation to LLRs.
 *                     If no approximation of the LLRs is known, all entries
 *                     must be set to zero. The array has length `66 * z` for
 *                     base graph 1, and `50 * z` for base graph 2. On exit,
 *                     updated rate-recovered 8-bit LLRs, which are ready to be
 *                     passed to decoding.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_ldpc_rate_recovery(armral_ldpc_graph_t bg, uint32_t z,
                                        uint32_t e, uint32_t rv,
                                        armral_modulation_type mod,
                                        const int8_t *src, int8_t *dst);
/** @} end ldpc */

/**
 * @ingroup groupUpPhy
 */
/**
 * @addtogroup turbo LTE Turbo
 * @{
 * \brief Performs encoding and decoding of data using LTE Turbo methods. The
 * encoding scheme is defined in section 5.1.3.2 of the 3GPP Technical
 * Specification (TS) 36.212 "Multiplexing and channel coding". The decoder
 * implements a maximum a posteriori (MAP) algorithm and returns a hard decision
 * (either 0 or 1) for each output bit. The encoding and decoding are performed
 * for a single code block.
 */
/**
 * This routine implements the LTE Turbo encoding scheme described in 3GPP
 * Technical Specification (TS) 36.212 "Multiplexing and channel coding". It
 * takes as input an array \c src of length \c k bits, where \c k must be one
 * of the values defined in TS 36.212 Table 5.1.3-3. The outputs of the
 * encoding are written into the three arrays \c dst0, \c dst1, and \c dst2,
 * each of which contains \c k+4 bits of output. The encoding is performed for
 * a single code block.
 *
 * @param[in]  src    Input data of length \c k bits.
 * @param[in]  k      Length of the input code block in bits.
 * @param[out] dst0   The systematic portion of the output of length \c k+4
 *                    bits. If \c k+4 is not on a byte boundary, the most
 *                    significant bits of the final byte in this array contain
 *                    the systematic bits.
 * @param[out] dst1   The parity portion of the output of length \c k+4 bits.
 *                    If \c k+4 is not on a byte boundary, the most
 *                    significant bits of the final byte in this array contain
 *                    the parity bits.
 * @param[out] dst2   The interleaved portion of the output of length \c k+4
 *                    bits. If \c k+4 is not on a byte boundary, the most
 *                    significant bits of the final byte in this array contain
 *                    the interleaved bits.

 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_turbo_encode_block(const uint8_t *src, uint32_t k,
                                        uint8_t *dst0, uint8_t *dst1,
                                        uint8_t *dst2);

/**
 * This routine implements a maximum a posteriori (MAP) algorithm to decode the
 * output of the LTE Turbo encoding scheme described in 3GPP Technical
 * Specification (TS) 36.212 "Multiplexing and channel coding". It takes as
 * input three arrays \c sys, \c par and \c itl, each  of length \c k+4 bits
 * where \c k must be one of the values defined in TS 36.212 Table 5.1.3-3.
 * These three arrays contain the log-likelihood ratios (LLRs) of the
 * systematic, parity and interleaved parity bits. The decoding is performed
 * for a single code block.
 *
 * The output is written into the array \c dst, which must contain enough bytes
 * to store \c k bits. These are hard outputs (that is, either 0 or 1); the
 * routine does not return LLRs.
 *
 * The routine takes a parameter \c max_iter, which specifies the
 * maximum number of iterations that the decoder will perform. The
 * algorithm will terminate in fewer iterations if there is no change
 * in the computed LLRs between consecutive iterations.
 *
 * @param[in] sys        The systematic portion of the input of length \c k+4
 *                       bytes representing 8-bit log-likelihood ratios.
 * @param[in] par        The parity portion of the input of length \c k+4 bytes
 *                       representing 8-bit log-likelihood ratios.
 * @param[in] itl        The interleaved portion of the input of length \c k+4
 *                       representing 8-bit log-likelihood ratios.
 * @param[in]  k         Length of the output code block in bits.
 * @param[out] dst       Decoded output data of length \c k bits.
 * @param[in]  max_iter  Maximum number of decoding iterations to perform.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_turbo_decode_block(const int8_t *sys, const int8_t *par,
                                        const int8_t *itl, uint32_t k,
                                        uint8_t *dst, uint32_t max_iter);
/** @} end turbo */

/**
 * @ingroup groupMatrix
 */
/**
 * @addtogroup svd SVD decomposition of single complex matrix
 * @{
 * \brief The Singular Value Decomposition (SVD)
 *  is used for selecting orthogonal user equipments
 *  pairing in mMIMO channels.
 */
/**
 *
 * This algorithm performs the Singular Value Decomposition (SVD)
 * of an `M-by-N` single complex matrix `A`, where `M ≥ N` and `A` is stored in
 * column-major order. The SVD of `A` is a two-sided decomposition in the form
 * `A = U  Σ  V^H`, with `U` an `M-by-M` single complex orthogonal matrix.
 * Note that  we only store the first `N` columns of `U` because there
 * are at most `N` non-zero singular values. `V` is an `N-by-N`
 * single complex orthogonal matrix, and `Σ` is an `M-by-N`
 * real matrix. Entries `Σ_{i, i}`, `i < n` contain the singular
 * values, and other entries in `Σ` are zero. We only store
 * the singular values, not the full matrix `Σ`. The singular
 * values `Σ_{i,i}` are stored in vector `s` for `0 ≤ i < N`.
 * The matrices `U` and `V^H` are implicitly used in the
 * algorithm, unless parameter `vect` is specified to be true,
 * in which case the left and right singular vectors (respectively)
 * are stored in `U` and `V^H` in column-major order. This means that
 * singular vectors are stored contiguously in `U`, and are
 * non-contiguous in `V^H`. Note that it is `V^H` that is returned,
 * not `V`.
 *
 * There are different algorithms for an efficient SVD.
 * The most appropriate is automatically selected
 * depending on the size of the input matrix.
 *
 * @param[in] vect     If true, both the singular values and
 *                     the singular vectors are computed, else
 *                     only the singular values are computed.
 * @param[in] m        The number of rows (`M`) in matrix `A`.
 * @param[in] n        The number of columns (`N`) in matrix `A`.
 * @param[in,out] a    On entry contains the `M-by-N` matrix
 *                     on which to perform the SVD. On exit contains
 *                     the Householder reflectors used to
 *                     perform the bidiagonalization of `A`.
 * @param[out] s       The vector of singular values.
 * @param[out] u       The left singular vectors, if required.
 *                     If `vect` is true, `u` is populated with
 *                     the left singular vectors in the SVD.
 *                     A memory of `M-by-N` is assumed to have
 *                     been allocated before the call to this method.
 * @param[out] vt      The right singular vectors, if required.
 *                     If `vect` is true, `vt` is populated with
 *                     the right singular vectors in the SVD.
 *                     A memory of `N-by-N` is assumed to have been
 *                     allocated before the call to this method.
 * @return     An \c armral_status value that indicates success or failure.
 */
armral_status armral_svd_cf32(bool vect, int m, int n, armral_cmplx_f32_t *a,
                              float *s, armral_cmplx_f32_t *u,
                              armral_cmplx_f32_t *vt);
/** @} end svd */

#ifdef __cplusplus
}
#endif
