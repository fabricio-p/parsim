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