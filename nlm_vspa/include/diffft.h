// ===========================================================================
//! @file            diffft.h
//! @brief           DIF FFT library interface definitions.
//! @copyright       Copyright 2020 NXP
//! @ingroup         GROUP_FFT_C GROUP_FFT_N
//!
//! The diffft.h header defines the DIF FFT library application programming
//! interface.
//!
// ===========================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */

#ifndef __DIFFFT_H__
#define __DIFFFT_H__

#include <stddef.h>

// -----------------------------------------------------------------------------
//! @defgroup		GROUP_FFT_C FFT Library with circular buffer support
//! @brief          FFT function library
//!
//! This library contains function prototypes for radix 2 FFTs
//! The FFT Library provides the following functions:
//!   - 64 pt DIF FFT HFX_SFL:
//!      - fftDIF64_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 64pt DIF FFT
//!   - 64 pt DIF IFFT HFX_SFL:
//!      - ifftDIF64_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 64pt DIF IFFT
//!   - 128 pt DIF FFT HFX_SFL:
//!      - fftDIF128_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 128pt DIF FFT
//!   - 128 pt DIF IFFT HFX_SFL:
//!      - ifftDIF128_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 128pt DIF IFFT
//!   - 256 pt DIF FFT HFX_SFL:
//!      - fftDIF256_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 256pt DIF FFT
//!   - 256 pt DIF IFFT HFX_SFL:
//!      - ifftDIF256_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 256pt DIF IFFT
//!   - 512 pt DIF FFT HFX_SFL:
//!      - fftDIF512_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 512pt DIF FFT
//!   - 512 pt DIF IFFT HFX_SFL:
//!      - ifftDIF512_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 512pt DIF IFFT
//!   - 1024 pt DIF FFT HFX_SFL:
//!      - fftDIF1024_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 1024pt DIF FFT
//!   - 1024 pt DIF IFFT HFX_SFL:
//!      - ifftDIF1024_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 1024pt DIF IFFT
//!   - 2048 pt DIF FFT HFX_SFL:
//!      - fftDIF2048_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 2048pt DIF FFT
//!   - 2048 pt DIF IFFT HFX_SFL:
//!      - ifftDIF2048_hfx_sfl(): 16-bit fixed point in, 32-bit floating point out 2048pt DIF IFFT
//!   - 64 pt DIF FFT HFX_HFX:
//!      - fftDIF64_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 64pt DIF FFT
//!   - 64 pt DIF IFFT HFX_HFX:
//!      - ifftDIF64_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 64pt DIF IFFT
//!   - 128 pt DIF FFT HFX_HFX:
//!      - fftDIF128_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 128pt DIF FFT
//!   - 128 pt DIF IFFT HFX_HFX:
//!      - ifftDIF128_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 128pt DIF IFFT
//!   - 256 pt DIF FFT HFX_HFX:
//!      - fftDIF256_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 256pt DIF FFT
//!   - 256 pt DIF IFFT HFX_HFX:
//!      - ifftDIF256_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 256pt DIF IFFT
//!   - 512 pt DIF FFT HFX_HFX:
//!      - fftDIF512_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 512pt DIF FFT
//!   - 512 pt DIF IFFT HFX_HFX:
//!      - ifftDIF512_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 512pt DIF IFFT
//!   - 1024 pt DIF FFT HFX_HFX:
//!      - fftDIF1024_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1024pt DIF FFT
//!   - 1024 pt DIF IFFT HFX_HFX:
//!      - ifftDIF1024_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1024pt DIF IFFT
//!   - 2048 pt DIF FFT HFX_HFX:
//!      - fftDIF2048_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 2048pt DIF FFT
//!   - 2048 pt DIF IFFT HFX_HFX:
//!      - ifftDIF2048_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 2048pt DIF IFFT
//!   - 64 pt DIF FFT HFX_HFL:
//!      - fftDIF64_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 64pt DIF FFT
//!   - 64 pt DIF IFFT HFX_HFL:
//!      - ifftDIF64_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 64pt DIF IFFT
//!   - 128 pt DIF FFT HFX_HFL:
//!      - fftDIF128_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 128pt DIF FFT
//!   - 128 pt DIF IFFT HFX_HFL:
//!      - ifftDIF128_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 128pt DIF IFFT
//!   - 256 pt DIF FFT HFX_HFL:
//!      - fftDIF256_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 256pt DIF FFT
//!   - 256 pt DIF IFFT HFX_HFL:
//!      - ifftDIF256_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 256pt DIF IFFT
//!   - 512 pt DIF FFT HFX_HFL:
//!      - fftDIF512_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 512pt DIF FFT
//!   - 512 pt DIF IFFT HFX_HFL:
//!      - ifftDIF512_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 512pt DIF IFFT
//!   - 1024 pt DIF FFT HFX_HFL:
//!      - fftDIF1024_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 1024pt DIF FFT
//!   - 1024 pt DIF IFFT HFX_HFL:
//!      - ifftDIF1024_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 1024pt DIF IFFT
//!   - 2048 pt DIF FFT HFX_HFL:
//!      - fftDIF2048_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 2048pt DIF FFT
//!   - 2048 pt DIF IFFT HFX_HFL:
//!      - ifftDIF2048_hfx_hfl(): 16-bit fixed point in, 16-bit floating point out 2048pt DIF IFFT
//!
//! @{
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// variable
// -----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//! @brief           64 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF64_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           64 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF64_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF128_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF128_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF256_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF256_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF512_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF512_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF1024_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF1024_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt SP FFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF2048_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt SP IFFT using decimation in frequency approach for 16 bit fixed point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF2048_hfx_sfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat32_t*      	pOut,   // Output buffer pointer for holding single floating point complex values.
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);


// ---------------------------------------------------------------------------
//! @brief           64 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF64_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           64 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF64_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF128_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF128_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF256_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF256_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF512_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF512_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half fixed point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF1024_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF1024_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF2048_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF2048_hfx_hfx(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfixed16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);


// ---------------------------------------------------------------------------
//! @brief           64 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF64_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           64 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 64 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF64_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF128_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF128_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF256_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           256 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 256 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF256_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF512_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF512_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF1024_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF1024_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX FFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIF2048_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX IFFT using decimation in frequency approach for 16 bit fixed point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   		Input buffer address
//! @param[out]      pOut  		Output buffer address
//! @param[in]       pBuff 		Base of circular input buffer
//! @param[in]       cbuffSize  Size of circular buffer in half-word units
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIF2048_hfx_hfl(
	cfixed16_t const* pIn,	// Input buffer pointer for holding half fixed point complex values
	cfloat16_t*      	pOut,   // Output buffer pointer for holding half floating point complex values
	cfixed16_t const* pBuff,	// Base of circular buffer
	size_t const cbuffSize		// Size of circular buffer in half-words
	);

// ------------------------------------------------------------------------------------------
//! @brief           4096 pt F24 FFT using decimation in frequency approach for 16 bit 
//!					 fixed point input data, 16 bit fixed point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the 
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 1705
//! @stack         	 0
//!
//! This function calculates 4096 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x). There are four variants. One with default 
//!	output scaling by 4096. The other three variants are with 64, 128 and 256 scaling
//!	
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch must be in different memory banks as compared to input and  
//! 				 output buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// --------------------------------------------------------------------------------------------	
extern void fft_DIF_4096_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );
	
//Scaling by 64 APIs	
extern void fft_DIF_4096_sc64_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );		

//Scaling by 128 APIs	
extern void fft_DIF_4096_sc128_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );	

//Scaling by 256 APIs	
extern void fft_DIF_4096_sc256_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );		
// ------------------------------------------------------------------------------------------
//! @brief           4096 pt F24 IFFT using decimation in frequency approach for 16 bit 
//!					 fixed point input data, 16 bit fixed point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the 
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 1702
//! @stack         	 0
//!
//! This function calculates 4096 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x). There are four variants. One with default 
//!	output scaling by 4096. The other three variants are with 64, 128 and 256 scaling
//!	
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch must be in different memory banks as compared to input and  
//! 				 output buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// --------------------------------------------------------------------------------------------		
extern void ifft_DIF_4096_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );
	
//Scaling by 64 APIs	
extern void ifft_DIF_4096_sc64_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );	

//Scaling by 128 APIs	
extern void ifft_DIF_4096_sc128_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );

//Scaling by 256 APIs	
extern void ifft_DIF_4096_sc256_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );	
// ------------------------------------------------------------------------------------------
//! @brief           4096 pt F24 FFT using decimation in frequency approach for 16 bit 
//!					 floating point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the 
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 1704
//! @stack         	 0
//!
//! This function calculates 4096 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x). There are three variants. One with default 
//!	output scaling by 4096. The other two variants are with 128 and 256 scaling
//!	
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch must be in different memory banks as compared to input and  
//! 				 output buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// --------------------------------------------------------------------------------------------		
extern void fft_DIF_4096_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
		cfloat16_t*         pInBuffBase,
		uint32_t            inputBuffSize
   );

// ------------------------------------------------------------------------------------------
//! @brief           4096 pt F24 IFFT using decimation in frequency approach for 16 bit 
//!					 floating point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the 
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 1701
//! @stack         	 0
//!
//! This function calculates 4096 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x). There are three variants. One with default 
//!	output scaling by 4096. The other two variants are with 128 and 256 scaling
//!	
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch must be in different memory banks as compared to input and  
//! 				 output buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// --------------------------------------------------------------------------------------------		   
extern void ifft_DIF_4096_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
		cfloat16_t*         pInBuffBase,
		uint32_t            inputBuffSize
   );

//! @} GROUP_FFT_C

// -----------------------------------------------------------------------------
//! @defgroup		GROUP_FFT_N FFT Library with circular buffer support
//! @brief          FFT function library
//!
//! This library contains function prototypes for radix 2 FFTs 
//! The FFT Library provides the following functions:
//!   - 8192 pt DIF FFT HFX_HFX:
//!      - fft_DIF_8192_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF FFT with scaling by 8192
//!   - 8192 pt DIF FFT SC64 HFX_HFX:
//!      - fft_DIF_8192_sc64_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF FFT with scaling by 64
//!   - 8192 pt DIF FFT SC128 HFX_HFX:
//!      - fft_DIF_8192_sc128_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF FFT with scaling by 128
//!   - 8192 pt DIF IFFT HFX_HFX:
//!      - ifft_DIF_8192_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF IFFT with scaling by 8192
//!   - 8192 pt DIF IFFT SC64 HFX_HFX:
//!      - ifft_DIF_8192_sc64_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF IFFT with scaling by 64
//!   - 8192 pt DIF IFFT SC128 HFX_HFX:
//!      - ifft_DIF_8192_sc128_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 8192pt DIF IFFT with scaling by 128
//!   - 8192 pt DIF FFT HFL_HFL:
//!      - fft_DIF_8192_hfl_hfl(): 16-bit half float in, 16-bit half float out 8192pt DIF FFT
//!   - 8192 pt DIF IFFT HFL_HFL:
//!      - ifft_DIF_8192_hfl_hfl(): 16-bit half float in, 16-bit half float out 8192pt DIF IFFT
//!
//!
//!   - 8192 pt split DIF FFT HFX_HFX for stage 1 to 11:
//!      - fft_DIF_8192_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF FFT
//!   - 8192 pt split DIF FFT HFX_HFX for stage 12 to 13:
//!      - fft_DIF_8192_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF FFT
//!
//!   - 8192 pt split DIF FFT HFX_HFX for stage 1 to 11 with output scaling by 64:
//!      - fft_DIF_8192_sc64_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF FFT
//!   - 8192 pt split DIF FFT HFX_HFX for stage 12 to 13 with output scaling by 64:
//!      - fft_DIF_8192_sc64_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF FFT
//!
//!   - 8192 pt split DIF FFT HFX_HFX for stage 1 to 11 with output scaling by 128:
//!      - fft_DIF_8192_sc128_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF FFT
//!   - 8192 pt split DIF FFT HFX_HFX for stage 12 to 13 with output scaling by 128:
//!      - fft_DIF_8192_sc128_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF FFT
//!
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 1 to 11:
//!      - ifft_DIF_8192_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF IFFT
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 12 to 13:
//!      - ifft_DIF_8192_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF IFFT
//!
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 1 to 11 with output scaling by 64:
//!      - ifft_DIF_8192_sc64_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF IFFT
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 12 to 13 with output scaling by 64:
//!      - ifft_DIF_8192_sc64_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF IFFT
//!
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 1 to 11 with output scaling by 128:
//!      - ifft_DIF_8192_sc128_split_1_11_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 1 to 11 stages of 8192pt DIF IFFT
//!   - 8192 pt split DIF IFFT HFX_HFX for stage 12 to 13 with output scaling by 128:
//!      - ifft_DIF_8192_sc128_split_12_13_hfx_hfx(): 16-bit fixed point in, 16-bit fixed point out 12 to 13 stages of 8192pt DIF IFFT
//!
//!   - 8192 pt split DIF FFT HFL_HFL for stage 1 to 11:
//!      - fft_DIF_8192_split_1_11_hfl_hfl(): 16-bit half float in, 16-bit half float out 1 to 11 stages of 8192pt DIF FFT
//!   - 8192 pt split DIF FFT HFL_HFL for stage 12 to 13:
//!      - fft_DIF_8192_split_12_13_hfl_hfl(): 16-bit half float in, 16-bit half float out 12 to 13 stages of 8192pt DIF FFT
//!
//!   - 8192 pt split DIF IFFT HFL_HFL for stage 1 to 11:
//!      - ifft_DIF_8192_split_1_11_hfl_hfl(): 16-bit half float in, 16-bit half float out 1 to 11 stages of 8192pt DIF IFFT
//!   - 8192 pt split DIF IFFT HFL_HFL for stage 12 to 13:
//!      - ifft_DIF_8192_split_12_13_hfl_hfl(): 16-bit half float in, 16-bit half float out 12 to 13 stages of 8192pt DIF IFFT
//!
//! @{
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// variable
// -----------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 FFT using decimation in frequency approach for 16 bit 
//!					 fixed point input data, 16 bit fixed point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the 
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 3496
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x). There are three variants. One with default 
//!	output scaling by 8192. The other two variants are with 64 and 128 scaling
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// --------------------------------------------------------------------------------------------
extern void fft_DIF_8192_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );

extern void fft_DIF_8192_sc64_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
    );
	
extern void fft_DIF_8192_sc128_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
    );	

// ---------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 Split FFT with two APIs for stage (1 to 11) and   
//!					 stage (12 to 13) using decimation in frequency approach for  
//!					 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 2984 & 542 respectively for two kernels
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using two functions using decimation in frequency 
//! (DIF approach). The two functions must be called in sequence and can be across cores.
//! There are three variants. One with default output scaling by 8192. 
//!	The other two variants are with 64 and 128 scaling.
//!
//! @attention       Input buffer to first stage can be unaligned buffer but input to 
//! 				 second stage must be aligned 
//! @attention       Input to second stage must be output of first stage
//! @attention       Inputs to first stage must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
//Default 8192 scaling APIs
extern void fft_DIF_8192_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   
   
extern void fft_DIF_8192_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );

//Scaling by 64 APIs
extern void fft_DIF_8192_sc64_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   

extern void fft_DIF_8192_sc64_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );   

//Scaling by 128 APIs
extern void fft_DIF_8192_sc128_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   

extern void fft_DIF_8192_sc128_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );   

// -------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 IFFT using decimation in frequency approach for 16 bit 
//!					 fixed point input data, 16 bit fixed point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 3494
//! @stack         	 0
//!
//! This function calculates 8192 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x). There are three variants. One with default 
//!	output scaling by 8192. The other two variants are with 64 and 128 scaling
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
extern void ifft_DIF_8192_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,
		cfixed16_t* const   pIn,
		cfixed16_t*         pInBuffBase,
		uint32_t            inputBuffSize
    );

extern void ifft_DIF_8192_sc64_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
    );
	
extern void ifft_DIF_8192_sc128_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
    );	
	

// -------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 Split IFFT with two APIs for stage (1 to 11) and   
//!					 stage (12 to 13) using decimation in frequency approach for  
//!					 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 2982 & 542 respectively for two kernels
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using two functions using decimation in frequency 
//! (DIF approach). The two functions must be called in sequence and can be across cores.
//! There are three variants. One with default output scaling by 8192. 
//!	The other two variants are with 64 and 128 scaling.
//!
//! @attention       Input buffer to first stage can be unaligned buffer but input to 
//! 				 second stage must be aligned 
//! @attention       Input to second stage must be output of first stage
//! @attention       Inputs to first stage must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
//Default 8192 scaling APIs
extern void ifft_DIF_8192_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   
   
extern void ifft_DIF_8192_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );

//Scaling by 64 APIs
extern void ifft_DIF_8192_sc64_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   

extern void ifft_DIF_8192_sc64_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );   

//Scaling by 128 APIs
extern void ifft_DIF_8192_sc128_split_1_11_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn,
        cfixed16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   

extern void ifft_DIF_8192_sc128_split_12_13_hfx_hfx(
		cfixed16_t*         pOut,
		cfixed16_t*			pScratch,		
		cfixed16_t* const   pIn
   );   


// -------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 FFT using decimation in frequency approach for 16 bit  
//!					 floating point input data, 16 bit floating point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 3491
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
extern void fft_DIF_8192_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
		cfloat16_t*         pInBuffBase,
		uint32_t            inputBuffSize
   );
   
// ---------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 Split FFT with two APIs for stage (1 to 11) and   
//!					 stage (12 to 13) using decimation in frequency approach for  
//!					 16 bit floating point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 2979 & 537 respectively for two kernels
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using two functions using decimation in frequency 
//! (DIF approach). The two functions must be called in sequence and can be across cores.
//!
//! @attention       Input buffer to first stage can be unaligned buffer but input to 
//! 				 second stage must be aligned 
//! @attention       Input to second stage must be output of first stage
//! @attention       Inputs to first stage must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
extern void fft_DIF_8192_split_1_11_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
        cfloat16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   
   
extern void fft_DIF_8192_split_12_13_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn
   );   

// --------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 IFFT using decimation in frequency approach for 16 bit 
//!					 floating point input data, 16 bit floating point output data 
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 3489
//! @stack         	 0
//!
//! This function calculates 8192 pt IFFT using decimation in frequency (DIF approach)
//! Equivalent MATLAB command: y = N*ifft(x)
//!
//! @attention       Inputs must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
extern void ifft_DIF_8192_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
        cfloat16_t*         pInBuffBase,
        uint32_t            inputBuffSize
    );
	
// -------------------------------------------------------------------------------------------
//! @brief           8192 pt F24 Split IFFT with two APIs for stage (1 to 11) and   
//!					 stage (12 to 13) using decimation in frequency approach for  
//!					 16 bit floating point input data, 16 bit floating point output data
//!
//! @param[in]       pIn   			Input buffer address pointing to address within the
//!									circular buffer which is 2 half-words aligned
//! @param[out]      pOut  			Output buffer address (DMEM vector-aligned )
//! @param[in]       pScratch 		Scratch buffer
//!	@param[in]		 pInBuffBase	Pointer to DMEM aligned circular buffer base address
//!	@param[in]		 inputBuffSize	Length of circular buffer size in bytes. Length should be 
//!									in multiples of 128 bytes 
//! @return          Void.
//! @cycle         	 2977 & 537 respectively for two kernels
//! @stack         	 0
//!
//! This function calculates 8192 pt FFT using two functions using decimation in frequency 
//! (DIF approach). The two functions must be called in sequence and can be across cores.
//!
//! @attention       Input buffer to first stage can be unaligned buffer but input to 
//! 				 second stage must be aligned 
//! @attention       Input to second stage must be output of first stage
//! @attention       Inputs to first stage must be in linear order
//! @attention       Outputs are in bit-reversed order
//! @attention       The output buffer must be vector-aligned.
//! @attention       The scratch and the output buffer must be placed in two 
//! 				 different memory banks.
//! @attention       To save memory the input and scratch buffer could be same buffer.
//!	@attention		 For linear FFT/IFFT operation, the circular buffer base and the input  
//!					 address(pIn) shall be the same or at max offset within the same DMEM line.
// ---------------------------------------------------------------------------------------------
extern void ifft_DIF_8192_split_1_11_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn,
        cfloat16_t*         pInBuffBase,
        uint32_t            inputBuffSize
   );   
   
extern void ifft_DIF_8192_split_12_13_hfl_hfl(
		cfloat16_t*         pOut,
		cfloat16_t*			pScratch,		
		cfloat16_t* const   pIn
   );	
	
//! @} GROUP_FFT_N	
	
#endif // __DIFFFT_H__
