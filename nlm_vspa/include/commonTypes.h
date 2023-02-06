#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_


#include <stdint.h>
#include <vspa/intrinsics.h>
#include "vspa.h"

typedef __fx16                  fixed16_t;    //!< 16-bit half precision fixed point data.
typedef __fp16                  float16_t;    //!< 16-bit half precision floating point data.
typedef float                   float32_t;    //!< 32-bit single precision floating point data.
typedef double                  float64_t;    //!< 64-bit double precision floating point data.

typedef vspa2_complex_fixed16  cfixed16_t;    //!< Complex 16-bit half precision fixed point data.
typedef vspa2_complex_float16  cfloat16_t;    //!< Complex 16-bit half precision floating point data.
typedef vspa_complex_float32   cfloat32_t;    //!< Complex 32-bit single precision floating point data.
typedef vspa_complex_float64   cfloat64_t;    //!< Complex 64-bit double precision floating point data

// For linear arrays pBase can be NULL and size 0
typedef struct {
	cfixed16_t *pIn; 	// Input data address.
	cfixed16_t *pBase;	// Circular buffer base address.
	int16_t		size;	// Size of circular buffer in half-words
} CBUF_CHFX_T;

#endif //_COMMON_TYPES_H_
