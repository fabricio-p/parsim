#ifndef PARSIM_UTIL_H
#define PARSIM_UTIL_H
#include <iostream>
#include <raymath.h>

static inline Vector2 operator+(Vector2 a, Vector2 b) {
    return Vector2Add(a, b);
}

static inline Vector2 operator-(Vector2 a, Vector2 b) {
    return Vector2Subtract(a, b);
}

static inline Vector2 operator+=(Vector2 &a, Vector2 b) {
    a = a + b;
    return a;
}

static inline Vector2 operator*(Vector2 a, float b) {
    return Vector2Scale(a, b);
}

static inline Vector2 operator/(Vector2 a, float b) {
    return a * (1.f / b);
}

static inline float abs(Vector2 a) {
    return Vector2Length(a);
}

static inline std::ostream& operator<<(std::ostream& o, Vector2 vec) {
    return o<<"{x: "<<vec.x<<", y: "<<vec.y<<"}";
}

static inline float deg2rad(float deg) {
    return M_PIf * deg / 180.f;
}

#define DUMP_FR(expr)                                               \
    std::cerr<<"["<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"] "\
             <<(#expr)<<" = "<<(expr)<<std::endl;

#endif /* PARSIM_UTIL_H */

#ifdef DUMP
#undef DUMP
#endif /* DUMP */

#ifdef DEBUGGING
#define DUMP(expr) DUMP_FR(expr)
#else
#define DUMP(expr)
#endif