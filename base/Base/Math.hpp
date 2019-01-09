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

        float area() const {
            return width * height;
        }

        bool operator == (const Vec2 & vec2) const {
            return (x == vec2.x && y == vec2.y);
        }

    };

    typedef Vec2 Size;

}
#endif //GPUCAMERAVIDEO_MATH_H
