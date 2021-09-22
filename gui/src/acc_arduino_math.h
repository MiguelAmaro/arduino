/* date = September 20th 2021 7:46 pm */

#ifndef ACC_ARDUINO_MATH_H
#define ACC_ARDUINO_MATH_H

union v2f32
{
    struct
    {
        f32 x;
        f32 y;
    };
    f32 e[2];
};

union v2s32
{
    struct
    {
        s32 x;
        s32 y;
    };
    s32 e[2];
};

union v4f32
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

union rect_v2f32
{
    struct
    {
        v2f32 min;
        v2f32 max;
    };
    f32 e[2];
};

union rect_v2s32
{
    struct
    {
        v2s32 min;
        v2s32 max;
    };
    s32 e[4];
};


#endif //ACC_ARDUINO_MATH_H
