#pragma once

#include <pico/time.h>

namespace uazips
{

    struct TimeHandler
    {
        uint64_t TimeWhenCreated;
        uint64_t TimeThen;
        uint64_t TimeNow;
        uint64_t DeltaTime;
        inline TimeHandler()
        {
            TimeWhenCreated = get_absolute_time();
            TimeThen = TimeWhenCreated;
            TimeNow = TimeWhenCreated;
        }

        inline void Update()
        {
            TimeNow = get_absolute_time();   
            DeltaTime = TimeNow - TimeThen;
            TimeThen = TimeNow;
        }

        inline uint64_t GetElapsed()
        {
            return TimeNow - TimeWhenCreated;
        }
    };

}