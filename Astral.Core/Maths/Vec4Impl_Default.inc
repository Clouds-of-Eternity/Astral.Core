inline Vec4()
{
    X = 0.0f;
    Y = 0.0f;
    Z = 0.0f;
    W = 0.0f;
}
inline Vec4(float unit)
{
    X = unit;
    Y = unit;
    Z = unit;
    W = unit;
}
inline Vec4(float x, float y, float z, float w)
{
    X = x;
    Y = y;
    Z = z;
    W = w;
}
inline Vec4(Vec2 vec2, float z, float w)
{
    X = vec2.X;
    Y = vec2.Y;
    Z = z;
    W = w;
}
inline Vec4(Vec3 vec3, float w)
{
    X = vec3.X;
    Y = vec3.Y;
    Z = vec3.Z;
    W = w;
}
inline Vec4(vec4 cvec)
{
    X = cvec.X;
    Y = cvec.Y;
    Z = cvec.Z;
    W = cvec.W;
}
inline Vec4 operator+(const Vec4 other) const
{
    Vec4 result = *this;
    result.X += other.X;
    result.Y += other.Y;
    result.Z += other.Z;
    result.W += other.W;
    return result;
}
inline void operator+=(const Vec4 other)
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    W += other.W;
}
inline Vec4 operator-(const Vec4 other) const
{
    Vec4 result = *this;
    result.X -= other.X;
    result.Y -= other.Y;
    result.Z -= other.Z;
    result.W -= other.W;
    return result;
}
inline void operator-=(const Vec4 other)
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    W -= other.W;
}
inline Vec4 operator*(const Vec4 other) const
{
    Vec4 result;
    result.X = X * other.X;
    result.Y = Y * other.Y;
    result.Z = Z * other.Z;
    result.W = W * other.W;
    return result;
}
inline void operator*=(const Vec4 other)
{
    X *= other.X;
    Y *= other.Y;
    Z *= other.Z;
    W *= other.W;
}
inline Vec4 operator*(float other)
{
    return Vec4(X * other, Y * other, Z * other, W * other);
}
inline void operator*=(float other)
{
    X *= other;
    Y *= other;
    Z *= other;
    W *= other;
}
inline Vec4 operator/(const Vec4 other) const
{
    Vec4 result;
    result.X = X / other.X;
    result.Y = Y / other.Y;
    result.Z = Z / other.Z;
    result.W = W / other.W;
    return result;
}
inline void operator/=(const Vec4 other)
{
    X /= other.X;
    Y /= other.Y;
    Z /= other.Z;
    W /= other.W;
}
inline Vec4 operator/(float other)
{
    return Vec4(X / other, Y / other, Z / other, W/ other);
}
inline void operator/=(float other)
{
    X /= other;
    Y /= other;
    Z /= other;
    W /= other;
}
inline Vec4 operator -() const
{
    return Vec4(-X, -Y, -Z, -W);
}
inline i32 FindIndexOf(float component) const
{
    if (X == component) return 0;
    if (Y == component) return 1;
    if (Z == component) return 2;
    if (W == component) return 3;
    return -1;
}

inline bool operator==(Vec4 other) const
{
    return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
}
inline bool operator!=(Vec4 other) const
{
    return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
}
inline bool operator<(Vec4 other) const
{
    return X < other.X && Y < other.Y && Z < other.Z && W < other.W;
}
inline bool operator<=(Vec4 other) const
{
    return X <= other.X && Y <= other.Y && Z <= other.Z && W <= other.W;
}
inline bool operator>(Vec4 other) const
{
    return X > other.X && Y > other.Y && Z > other.Z && W > other.W;
}
inline bool operator>=(Vec4 other) const
{
    return X >= other.X && Y >= other.Y && Z >= other.Z && W >= other.W;
}

static inline Vec4 Lerp4(const Vec4 A, const Vec4 B, Vec4 amount)
{
    Vec4 result;
    result.X = A.X + (B.X - A.X) * amount.X;
    result.Y = A.Y + (B.Y - A.Y) * amount.Y;
    result.Z = A.Z + (B.Z - A.Z) * amount.Z;
    result.W = A.W + (B.W - A.W) * amount.W;
    return result;
}
static inline Vec4 Lerp(const Vec4 A, const Vec4 B, float amount)
{
    Vec4 result;
    result.X = A.X + (B.X - A.X) * amount;
    result.Y = A.Y + (B.Y - A.Y) * amount;
    result.Z = A.Z + (B.Z - A.Z) * amount;
    result.W = A.W + (B.W - A.W) * amount;
    return result;
}
static inline Vec4 Abs(Vec4 value)
{
    return Vec4(fabsf(value.X), fabsf(value.Y), fabsf(value.Z), fabsf(value.W));
}
static inline float Dot(const Vec4 A, const Vec4 B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}
static inline float MinComponent(Vec4 value)
{
    return fminf(fminf(value.X, value.Y), fminf(value.Z, value.W));
}
static inline float MaxComponent(Vec4 value)
{
    return fmaxf(fmaxf(value.X, value.Y), fmaxf(value.Z, value.W));
}
static inline Vec4 Min(const Vec4 A, const Vec4 B)
{
    return Vec4(fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z), fminf(A.W, B.W));
}
static inline Vec4 Max(const Vec4 A, const Vec4 B)
{
    return Vec4(fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z), fmaxf(A.W, B.W));
}