//
// Created by 刘轩 on 2019/1/8.
//

#ifndef GPUCAMERAVIDEO_MATH_H
#define GPUCAMERAVIDEO_MATH_H

namespace GCVBase{
    union Vec2{
        struct{
            float x;
            float y;
        };

        struct{
            float width;
            float height;
        };

        Vec2(float mx, float my){
            x = mx;
            y = my;
        }

        static const Vec2 & zero(){
            static const Vec2 zero(0,0);
            return zero;
        }

        bool isZero() const {
            return *this == zero();
        }

        float size() const {
            return width * height;
        }

        bool operator == (const Vec2 & vec2) const {
            return (x == vec2.x && y == vec2.y);
        }

    };

    typedef Vec2 Size;

    /*
     * 以下两个函数均来源于 LIUNX include/linux/kernel.h文件中的 ALIGN宏，主要用于内存对齐:
     */

    /*
     * 计算n以size为倍数的上界数（上进）：
     * 举个例子4k页面边界的例子，即size=4096： 如果n = 3888；那么以上界对齐，执行结果就是4096。
     * 如果n = 4096；结果是4096. 如果n = 4222； 则结果为8192.
     */
    static inline unsigned long alignment_up(unsigned long n,unsigned int size) {
        return ((n + size - 1) & (~(size - 1)));
    }

    /*
     * 计算n以size为倍数的下界数（丢弃）:
     * size=4096： 若n = 3888； 结果为0. 如果n = 4096；结果是4096. 如果n = 4222； 则结果为4096.
     */
    static inline unsigned long alignment_down(unsigned long n,unsigned int size) {
        return (n & (~(size - 1)));
    }

}
#endif //GPUCAMERAVIDEO_MATH_H
