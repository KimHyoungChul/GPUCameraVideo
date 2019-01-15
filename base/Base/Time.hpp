//
// Created by 刘轩 on 2019/1/10.
//

#ifndef GPUCAMERAVIDEO_TIME_H
#define GPUCAMERAVIDEO_TIME_H

#include <stdio.h>
#include <math.h>
#include <chrono>

namespace GCVBase{

    enum TimeFlags{
        TimeFlag_Init = 1UL<<0, //初始化时间
        TimeFlag_Indefinite = 1UL<<1,
        TimeFlag_NegativeInfinity = 1UL<<2, //负无穷
        TimeFlag_PositiveInfinity = 1UL<<3, //正无穷

        TimeFlags_Inited = TimeFlag_PositiveInfinity | TimeFlag_NegativeInfinity | TimeFlag_Indefinite
    };

    enum TimeRounding { //时间值取整
        RoundHalfAwayFromZero = 1,
        RoundTowardZero = 2,
        RoundAwayFromZero = 3,
        RoundTowardPositiveInfinity = 4,
        RoundTowardNegativeInfinity = 5,

        Default = RoundHalfAwayFromZero
    };

    union Time {
        struct {
            int64_t value; //从开始到当前帧经过的时间
            int32_t fps; //帧率
            TimeFlags flags; //时间标志位
        };

    public:
        Time(int64_t nValue , int32_t nfps, TimeFlags nFlags){
            value = nValue;
            fps = nfps;
            flags = nFlags;
        }

        static const Time &Zero() {
            static const Time zero(0, 1, TimeFlag_Init);
            return zero;
        }

        static const Time &Init(){
            static const Time invalid(0, 0, TimeFlag_Init);
            return invalid;
        }

        static const Time &Indefinite() {
            static const Time invalid(0, 0, TimeFlag_Indefinite);
            return invalid;
        }

        static const Time &NegativeInfinity() {
            static const Time invalid(0, 0, TimeFlag_NegativeInfinity);
            return invalid;
        }

        static const Time &PositiveInfinity() {
            static const Time invalid(0, 0, TimeFlag_PositiveInfinity);
            return invalid;
        }

        double nowseconds() const { //到目前过去多少秒
            if (flags & TimeFlag_Indefinite || isValid()){
                return NAN;
            }

            if (flags & TimeFlag_NegativeInfinity){
                return -DBL_MAX;
            }

            if (flags & TimeFlag_PositiveInfinity){
                return DBL_MAX;
            }

            if (!fps){
                return 0;
            }

            return (value / (double)fps);
        }

        bool isValid() const {
            return flags == 0;
        }

        bool isIndefinite() const {
            return (flags & TimeFlag_Indefinite) != 0;
        }

        bool isNegativeInfinity() const {
            return (flags & TimeFlag_NegativeInfinity) != 0;
        }

        bool inited() const {
            return (flags & TimeFlags_Inited) != 0;
        }

        Time convert(const Time &time) const {
            Time outTime = time.toTimeRound(fps == 0 ? time.fps : fps, Default);
            if (*this == Zero()){
                outTime = time;
            }

            return outTime;
        }

        Time toTimeRound(int32_t nfps, TimeRounding roundingMethod) const {
            double nSeconds = nowseconds();
            double tmpValue = nSeconds * nfps;
            Time outTime = Time::Zero();
            outTime.fps = nfps;
            switch (roundingMethod) {
                case RoundAwayFromZero:
                    outTime.value = static_cast<int64_t>(floor(tmpValue)); //floor(x)返回的是小于或等于x的最大整数(向下取整)
                    break;
                case RoundTowardNegativeInfinity:
                    outTime.flags = TimeFlag_NegativeInfinity;
                    break;
                case RoundTowardPositiveInfinity:
                    outTime.flags = TimeFlag_PositiveInfinity;
                    break;
                case RoundTowardZero:
                    outTime.value = static_cast<int64_t>(ceil(tmpValue)); //eil(x)返回的是大于x的最小整数(向上取整)
                    break;
                case RoundHalfAwayFromZero:
                default:
                    outTime.value = static_cast<int64_t>(round(tmpValue)); //round(x)返回四舍五入整数值
                    break;
            }
            return outTime;
        }

        bool operator >(const Time &time) const{
            return nowseconds() > time.nowseconds();
        };

        bool operator <(const Time &time) const{
            return time.nowseconds() > nowseconds();
        };

        Time operator -(const Time &time) const{
            if (inited()){
                return *this;
            }

            if (time.inited()){
                return time;
            }

            Time tmpTime = convert(time);
            return Time(value - tmpTime.value, tmpTime.fps, flags);
        };

        bool operator == (const Time &time) const{

            if (inited()){
                return time.inited() && flags == time.flags;
            }

            double sec = nowseconds();
            double destSec = time.nowseconds();
            return sec == destSec;
        };
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
