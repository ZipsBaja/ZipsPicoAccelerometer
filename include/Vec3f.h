// Zips Baja 2025 Vector3 float
// Author: Zack Young

#pragma once

#include <IntrinUtil.h>

namespace uazips
{
    struct ALIGNED(16) Vec3f
    {
#if USING_INTRIN
        inline Vec3f(simd_float32x4_t Data) : Data(Data)
        {
        }
#endif
        inline Vec3f(float x, float y, float z) : x(x), y(y), z(z)
        {
        }

        inline Vec3f() : Vec3f(0.f, 0.f, 0.f)
        {
        }

        union
        {
            struct
            {
                float x, y, z;
            };
#if USING_INTRIN
            simd_float32x4_t Data;
#endif
        };

        inline Vec3f operator+(const Vec3f& Other) const
        {
#ifdef __ARM_NEON
            return vaddq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            return _mm_mul_ps(Data, Other.Data);
#else
            return Vec3f(x + Other.x, y + Other.y, z + Other.z);
#endif
        }

        inline Vec3f operator-(const Vec3f& Other) const
        {
#ifdef __ARM_NEON
            return vsubq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            return _mm_sub_ps(Data, Other.Data);
#else
            return Vec3f(x - Other.x, y - Other.y, z - Other.z);
#endif
        }

        inline Vec3f operator*(const Vec3f& Other) const
        {
#ifdef __ARM_NEON
            return vmulq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            return _mm_mul_ps(Data, Other.Data);
#else
            return Vec3f(x * Other.x, y * Other.y, z * Other.z);
#endif
        }

        inline Vec3f operator/(const Vec3f& Other) const
        {
#ifdef __ARM_NEON
            return vdivq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            return _mm_div_ps(Data, Other.Data);
#else
            return Vec3f(x / Other.x, y / Other.y, z / Other.z);
#endif
        }

        inline Vec3f& operator+=(const Vec3f& Other)
        {
#ifdef __ARM_NEON
            Data = vaddq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            Data = _mm_add_ps(Data, Other.Data);
#else
            x += Other.x;
            y += Other.y;
            z += Other.z;
#endif
            return *this;
        }

        inline Vec3f& operator-=(const Vec3f& Other)
        {
#ifdef __ARM_NEON
            Data = vsubq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            Data = _mm_sub_ps(Data, Other.Data);
#else
            x -= Other.x;
            y -= Other.y;
            z -= Other.z;
#endif
            return *this;
        }

        inline Vec3f& operator*=(const Vec3f& Other)
        {
#ifdef __ARM_NEON
            Data = vmulq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            Data = _mm_mul_ps(Data, Other.Data);
#else
            x *= Other.x;
            y *= Other.y;
            z *= Other.z;
#endif
            return *this;
        }

        inline Vec3f& operator/=(const Vec3f& Other)
        {
#ifdef __ARM_NEON
            Data = vdivq_f32(Data, Other.Data);
#elif defined(__SSE2__)
            Data = _mm_div_ps(Data, Other.Data);
#else
            x /= Other.x;
            y /= Other.y;
            z /= Other.z;
#endif
            return *this;
        }

        inline Vec3f operator*(float Scalar) const
        {
#ifdef __ARM_NEON
            return vmulq_n_f32(Data, Scalar);
#elif defined(__SSE2__)
            return _mm_mul_ps(Data, _mm_set1_ps(Scalar));
#else
            return Vec3f(x * Scalar, y * Scalar, z * Scalar);
#endif
        }

        inline Vec3f operator/(float Scalar) const
        {
#ifdef __ARM_NEON
            return vdivq_n_f32(Data, Scalar);
#elif defined(__SSE2__)
            return _mm_div_ps(Data, _mm_set1_ps(Scalar));
#else
            return Vec3f(x / Scalar, y / Scalar, z / Scalar);
#endif
        }

        inline Vec3f& operator*=(float Scalar)
        {
#ifdef __ARM_NEON
            Data = vmulq_n_f32(Data, Scalar);
#elif defined(__SSE2__)
            Data = _mm_mul_ps(Data, _mm_set1_ps(Scalar));
#else
            x *= Scalar;
            y *= Scalar;
            z *= Scalar;
#endif
            return *this;
        }

        inline Vec3f& operator/=(float Scalar)
        {
#ifdef __ARM_NEON
            Data = vdivq_n_f32(Data, Scalar);
#elif defined(__SSE2__)
            Data = _mm_div_ps(Data, _mm_set1_ps(Scalar));
#else
            x /= Scalar;
            y /= Scalar;
            z /= Scalar;
#endif
            return *this;
        }

        inline float Length() const
        {
#ifdef __ARM_NEON
            simd_float32x4_t sqr = vmulq_f32(Data, Data);
            simd_float32x4_t sum = vpadd_f32(vget_low_f32(sqr), vget_high_f32(sqr));
            return vget_lane_f32(vsqrtq_f32(vdupq_n_f32(vget_lane_f32(vpadd_f32(sum, sum), 0))), 0)
#elif defined(__SSE2__)
            return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(Data, Data, 0b01110001)));
#else
            return sqrtf(x*x + y*y + z*z);
#endif
        }

        inline Vec3f Normalized() const
        {
#ifdef __ARM_NEON
            return vmulq_f32(Data, vrsqrtsq_f32(Data, Data));
#elif defined(__SSE2__)
            return _mm_mul_ps(Data, _mm_rsqrt_ps(_mm_dp_ps(Data, Data, 0b01110111)));
#else
            float len = sqrtf(x*x + y*y + z*z);
            return Vec3f(x / len, y / len, z / len);
#endif  
        }       

        inline float Dot(const Vec3f& Other) const
        {
#ifdef __ARM_NEON
            simd_float32x4_t prod = vmulq(Data, Other.Data);
            simd_float32x4_t sum = vpadd_f32(vget_low_f32(prod), vget_high_f32(prod));
            return vget_lane_f32(vpadd_f32(sum, sum), 0);
#elif defined(__SSE2__)
            return _mm_cvtss_f32(_mm_dp_ps(Data, Other.Data, 0b01111111));
#else
            return x * Other.x + y * Other.y + z * Other.z;
#endif
        }

        inline Vec3f& operator++()
        {
#ifdef __ARM_NEON

#elif defined(__SSE2__)

#else
            x += 1.f;
            y += 1.f;
            z += 1.f;
#endif
            return *this;
        }
    };
}