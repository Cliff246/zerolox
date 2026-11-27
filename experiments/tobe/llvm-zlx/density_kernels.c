typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

u32 add_three(u32 a, u32 b, u32 c)
{
    return a + b + c;
}

u32 constant_value(void)
{
    return 400;
}

u32 bit_mix(u32 a, u32 b)
{
    return (a ^ b) & 255;
}

u32 shift_mix(u32 a, u32 b)
{
    return (a << 3) ^ (b >> 5);
}

u32 sum_four(const u32 *p)
{
    return p[0] + p[1] + p[2] + p[3];
}

void copy_four(volatile u32 *dst, const volatile u32 *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

u32 load_mix(const volatile u8 *p, const volatile u16 *q)
{
    return p[1] + q[2];
}

int signed_byte(const volatile signed char *p)
{
    return p[1];
}

u32 equal32(u32 a, u32 b)
{
    return a == b;
}

u32 greater32(int a, int b)
{
    return a > b;
}

u32 choose32(u32 cond, u32 a, u32 b)
{
    return cond ? a : b;
}
