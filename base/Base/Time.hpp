//
// Created by 刘轩 on 2019/1/10.
//

#ifndef GPUCAMERAVIDEO_TIME_H
#define GPUCAMERAVIDEO_TIME_H

#include <cstdio>
#include <cmath>
#include <chrono>

namespace GCVBase{

    union Time {
        struct {
            int64_t value; //从开始到当前帧经过的时间
            int32_t fps; //帧率
        };

    public:
        Time(int64_t nValue , int32_t nfps){
            value = nValue;
            fps = nfps;
        }

        static const Time &Zero() {
            static const Time zero(0, 1);
            return zero;
        }
    };

    typedef Time MediaTime;

    static inline std::chrono::system_clock::duration periods() {
        return std::chrono::system_clock::now().time_since_epoch();
    }

    static inline double currentTimeOfSeconds() {
        using namespace std::chrono;
        return duration_cast<seconds>(periods()).count();
    }

    static inline double currentTimeOfMilliseconds(){
        using namespace std::chrono;
        return duration_cast<milliseconds>(periods()).count();
    }

    static inline double currentTimeOfNanoseconds(){
        using namespace std::chrono;
        return duration_cast<nanoseconds>(periods()).count();
    }
}

#endif //GPUCAMERAVIDEO_TIME_H
