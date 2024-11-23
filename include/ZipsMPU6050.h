// Zips Baja 2025 MPU6050 controller interface
// Author: Zack Young

#pragma once

#ifndef USING_ACCEL
#define USING_ACCEL 1
#endif
#ifndef USING_GYRO
#define USING_GYRO 1
#endif

#include <stdint.h>
#include <Vec3f.h>
#include <haw/MPU6050.h>

namespace uazips
{

    class MPU6050
    {
    private:
        union
        {
            struct
            {
                float ax, ay, az;
            };
            Vec3f AccelVec;
        };
        union
        {
            struct
            {
                float gr, gy, gp;
            };
            Vec3f GyroVec;
        };

        uint8_t I2CAddress;
        mpu6050_t MPUWrapper;

        Vec3f accel_offset;

    public:
        inline MPU6050(uint8_t Address) : I2CAddress(Address)
        {
            MPUWrapper = mpu6050_init(i2c_default, Address);
            if (mpu6050_begin(&MPUWrapper))
            {
                mpu6050_set_scale(&MPUWrapper, MPU6050_SCALE_2000DPS);
                mpu6050_set_range(&MPUWrapper, MPU6050_RANGE_16G);
                mpu6050_set_temperature_measuring(&MPUWrapper, false);
                mpu6050_set_accelerometer_measuring(&MPUWrapper,
#if USING_ACCEL
                true
#else
                false
#endif
                );
                mpu6050_set_gyroscope_measuring(&MPUWrapper,
#if USING_GYRO
                true
#else
                false
#endif
                );
                mpu6050_set_int_free_fall(&MPUWrapper, false);
                mpu6050_set_int_motion(&MPUWrapper, false);
                mpu6050_set_int_zero_motion(&MPUWrapper, false);
                mpu6050_set_motion_detection_threshold(&MPUWrapper, 2);
                mpu6050_set_motion_detection_duration(&MPUWrapper, 5);
                mpu6050_set_zero_motion_detection_threshold(&MPUWrapper, 4);
                mpu6050_set_zero_motion_detection_duration(&MPUWrapper, 2);
            }
        }

        /*
        * Calibrates the accelerometer to zero it to its best of ability.
        * @return The offset necessary to zero the vector.
        */
        const Vec3f& CalibrateAcceleration(size_t Iterations)
        {
            accel_offset = Vec3f();
            Vec3f Total;
            for (int i = 0; i < Iterations; i++)
            {
                Update();
                Total += GetAcceleration();
            }
            accel_offset = Total / (float)Iterations;
            return accel_offset;
        }
        
        void Update()
        {
            mpu6050_event(&MPUWrapper);
#if USING_ACCEL
            mpu6050_vectorf_t* Accel = mpu6050_get_accelerometer(&MPUWrapper);
            ax = Accel->x;
            ay = Accel->y;
            az = Accel->z - 1.f;
#endif
#if USING_GYRO
            mpu6050_vectorf_t* Gyro = mpu6050_get_gyroscope(&MPUWrapper);
            gr = Gyro->x;
            gp = Gyro->y;
            gy = Gyro->z;
#endif
        }

    public:
        inline Vec3f GetAcceleration() const
        {
            return AccelVec - accel_offset;
        }
        inline Vec3f GetAccelerationNoOffset() const
        {
            return AccelVec;
        }
        inline Vec3f GetGyroscope() const
        {
            return GyroVec;
        }
        inline uint8_t GetAddress() const
        {
            return I2CAddress;
        }

    };

}
