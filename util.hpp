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