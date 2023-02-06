// ===========================================================================
//! @file            ditfft.h
//! @brief           DIT FFT library interface definitions.
//! @copyright       Copyright 2017 NXP
//! @ingroup         GROUP_FFT
//!
//! The ditfft.h header defines the DIT FFT library application programming
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
#ifndef __DITFFT_H__
#define __DITFFT_H__

#include <stddef.h>
#include "commonTypes.h"
// -----------------------------------------------------------------------------
//! @defgroup		GROUP_FFT FFT Library
//! @brief          FFT function library
//!
//! This library contains function prototypes for radix 2 FFTs
//! The FFT Library provides the following functions:
//!   - 128 pt DIT FFT HFL:
//!      - fftDIT128_hfl(): 16-bit floating point in, 16-bit fixed-point out 128pt DIT FFT
//!   - 128 pt DIT IFFT HFL:
//!      - ifftDIT128_hfl(): 16-bit floating point in, 16-bit fixed-point out 128pt DIT IFFT
//!   - 128 pt DIT FFT SFL:
//!      - fftDIT128_sfl(): 16-bit floating point in, 32-bit floating-point out 128pt DIT FFT
//!   - 128 pt DIT IFFT SFL:
//!      - ifftDIT128_sfl(): 16-bit floating point in, 32-bit floating-point intermediate, 16-bit fixed-point out 128pt DIT IFFT
//!   - 512 pt DIT FFT HFL:
//!      - fftDIT512_hfl(): 16-bit floating point in, 16-bit fixed-point out 512pt DIT FFT
//!   - 512 pt DIT IFFT HFL:
//!      - ifftDIT512_hfl(): 16-bit floating point in, 16-bit fixed-point out 512pt DIT IFFT
//!   - 512 pt DIT FFT SFL:
//!      - fftDIT512_sfl(): 16-bit floating point in, 32-bit floating-point out 512pt DIT FFT
//!   - 512 pt DIT IFFT SFL:
//!      - ifftDIT512_sfl(): 16-bit floating point in, 32-bit floating-point intermediate, 16-bit fixed-point out 512pt DIT IFFT
//!   - 512 pt DIT FFT SFL SFL:
//!      - fftDIT512_sflsfl(): 32-bit floating point in, 32-bit floating-point out 512pt DIT FFT
//!   - 512 pt DIT IFFT SFL SFL:
//!      - ifftDIT512_sflsfl(): 32-bit floating point in, 32-bit floating-point out 512pt DIT IFFT
//!   - 1024 pt DIT FFT HFL:
//!      - fftDIT1024_hfl(): 16-bit floating point in, 16-bit fixed-point out 1024pt DIT FFT
//!   - 1024 pt DIT IFFT HFL:
//!      - ifftDIT1024_hfl(): 16-bit floating point in, 16-bit fixed-point out 1024pt DIT IFFT
//!   - 1024 pt DIT FFT SFL:
//!      - fftDIT1024_sfl(): 16-bit floating point in, 32-bit floating-point out 1024pt DIT FFT
//!   - 1024 pt DIT IFFT SFL:
//!      - ifftDIT1024_sfl(): 16-bit floating point in, 32-bit floating-point intermediate, 16-bit fixed-point out 1024pt DIT IFFT
//!   - 2048 pt DIT FFT HFL:
//!      - fftDIT2048_hfl(): 16-bit floating point in, 16-bit fixed-point out 2048pt DIT FFT
//!   - 2048 pt DIT IFFT HFL:
//!      - ifftDIT2048_hfl(): 16-bit floating point in, 16-bit fixed-point out 2048pt DIT IFFT
//!   - 2048 pt DIT FFT SFL:
//!      - fftDIT2048_sfl(): 16-bit floating point in, 32-bit floating-point out 2048pt DIT FFT
//!   - 2048 pt DIT IFFT SFL:
//!      - ifftDIT2048_sfl(): 16-bit floating point in, 32-bit floating-point intermediate, 16-bit fixed-point out 2048pt DIT IFFT
//!
//! @{
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// variable
// -----------------------------------------------------------------------------
extern uint32_t ifftDITScratchBuffer[4096] _VSPA_VECTOR_ALIGN;

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT128_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT128_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);


// ---------------------------------------------------------------------------
//! @brief           128 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 128 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT128_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding SP floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           128 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point intermediate data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! Additionally, this function uses scratch memory of size 256 words at ifftDITScratchBuffer
//! This function calculates 128 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT128_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT512_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT512_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed complex values.
	);
extern void ifftDIT512_hfx_hfx(
    cfixed16_t const* pIn,    
    cfixed16_t*       pOut
    );

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT512_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding SP floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point intermediate data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! Additionally, this function uses scratch memory of size 1024 words at ifftDITScratchBuffer
//! This function calculates 512 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT512_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt SFL FFT using decimation in time approach for 32 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT512_sflsfl(
	cfloat32_t const* pIn,	    // Input buffer pointer for holding SP floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding SP floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           512 pt SFL IFFT using decimation in time approach for 32 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 512 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = 512*ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT512_sflsfl(
	cfloat32_t const* pIn,	    // Input buffer pointer for holding SP floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding SP floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT1024_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT1024_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 1024 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT1024_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding half floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           1024 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point intermediate data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! Additionally, this function uses scratch memory of size 2048 words at ifftDITScratchBuffer
//! This function calculates 1024 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT1024_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT2048_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT2048_hfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);
// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX FFT using decimation in time approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack           0
//!
//! This function calculates 2048 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = (1/N)*fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT2048_hfx_hfx(
    cfixed16_t const* pIn,      // Input buffer pointer for holding half fixed point complex values
    cfixed16_t*         pOut    // Output buffer pointer for holding half fixed point complex values.
    );
// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX IFFT using decimation in time approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack           0
//!
//! This function calculates 2048 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT2048_hfx_hfx(
    cfixed16_t const* pIn,      // Input buffer pointer for holding half fixed point complex values
    cfixed16_t*         pOut    // Output buffer pointer for holding half fixed point complex values.
    );
// ---------------------------------------------------------------------------
//! @brief           CUSTOM 2048 pt HFX IFFT using decimation in time approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack           0
//!
//! This function calculates 2048 pt IFFT using decimation in time (DIT approach)
//! Note: this function only applies 1/16 scaling instead of the normal 1/2048
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT2048_hfx_hfx_scal_16(
    cfixed16_t const* pIn,      // Input buffer pointer for holding half fixed point complex values
    cfixed16_t*         pOut    // Output buffer pointer for holding half fixed point complex values.
    );
// ---------------------------------------------------------------------------
//! @brief           CUSTOM 2048 pt HFX IFFT using decimation in time approach for 16 bit fixed point input data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack           0
//!
//! This function calculates 2048 pt IFFT using decimation in time (DIT approach)
//! Note: this function only applies 1/128 scaling instead of the normal 1/2048
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT2048_hfx_hfx_scal_128(
    cfixed16_t const* pIn,      // Input buffer pointer for holding half fixed point complex values
    cfixed16_t*         pOut    // Output buffer pointer for holding half fixed point complex values.
    );
// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX FFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! This function calculates 2048 pt FFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = fft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void fftDIT2048_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half floating point complex values
	cfloat32_t*      	pOut   	// Output buffer pointer for holding SP floating point complex values.
	);

// ---------------------------------------------------------------------------
//! @brief           2048 pt HFX IFFT using decimation in time approach for 16 bit floating point input data, 32 bit floating point intermediate data, 16 bit fixed point output data
//!
//! @param[in]       pIn   Input buffer address
//! @param[out]      pOut  Output buffer address
//! @return          Void.
//! @cycle
//! @stack         	 0
//!
//! Additionally, this function uses scratch memory of size 4096 words at ifftDITScratchBuffer
//! This function calculates 2048 pt IFFT using decimation in time (DIT approach)
//! Equivalent MATLAB command: y = ifft(x)
//!
//! @attention       This function can operate in-place.
//! @attention       The output buffer must be vector-aligned.
// ---------------------------------------------------------------------------
extern void ifftDIT2048_sfl(
	cfloat16_t const* pIn,	    // Input buffer pointer for holding half precision complex values
	cfixed16_t*      	pOut   	// Output buffer pointer for holding half fixed point complex values.
	);


//! @} GROUP_FFT

#endif // __DITFFT_H__
