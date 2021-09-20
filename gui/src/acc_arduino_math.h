/* date = September 20th 2021 7:46 pm */

#ifndef ACC_ARDUINO_MATH_H
#define ACC_ARDUINO_MATH_H

union v2
{
    struct
    {
        f32 x;
        f32 y;
    };
    f32 e[2];
};

union v4
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 e[4];
};

union rect_v2
{
    struct
    {
        v2 min;
        v2 max;
    };
    f32 e[2];
};


#endif //ACC_ARDUINO_MATH_H
