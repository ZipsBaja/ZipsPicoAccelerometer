// Zips Baja 2025 Intrinsics Utilities
// Author: Zack Young

#pragma once

#ifdef __ARM_NEON
#define USING_INTRIN 1
#include <arm_neon.h>
typedef float32x2_t simd_float32x2_t;
typedef float32x4_t simd_float32x4_t;
typedef float64x2_t simd_float64x2_t;
struct simd_float64x4_t
{
    simd_float64x2_t Inl0;
    simd_float64x2_t Inl1;
};
#elif defined(__SSE2__)
#define USING_INTRIN 1
#include <immintrin.h>
typedef __m64   simd_float32x2_t;
typedef __m128  simd_float32x4_t;
typedef __m128d simd_float64x2_t;
typedef __m256d simd_float64x4_t;
#else
#include <math.h>
#define USING_INTRIN 0
#pragma message("Skipping intrinsics and using standard instructions.")
#endif

#if USING_INTRIN
#ifdef _MSC_VER
#define ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
#define ALIGNED(x) __attribute__((aligned(x)))
#else
#define ALIGNED(x)
#warning "Invalid compiler for alignment attribute"
#endif
#else
#define ALIGNED(x)
#endif