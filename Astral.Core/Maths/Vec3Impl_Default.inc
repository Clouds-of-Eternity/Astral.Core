inline Vec3 operator+(const Vec3 other) const 
{
    return Vec3(X + other.X, Y + other.Y, Z + other.Z);
}
inline Vec3 operator-(const Vec3 other) const 
{
    return Vec3(X - other.X, Y - other.Y, Z - other.Z);
}
inline Vec3 operator*(const Vec3 other) const
{
    return Vec3(X * other.X, Y * other.Y, Z * other.Z);
}
inline Vec3 operator/(const Vec3 other) const
{
    return Vec3(X / other.X, Y / other.Y, Z / other.Z);
}
inline Vec3 operator*(float amount) const
{
    return Vec3(X * amount, Y * amount, Z * amount);
}
inline Vec3 operator/(float amount) const
{
    float oneOver = 1.0f / amount;
    return Vec3(X * oneOver, Y * oneOver, Z * oneOver);
}
inline Vec3 operator-() const
{
    return Vec3(-X, -Y, -Z);
}
inline bool operator==(const Vec3 other) const
{
    return X == other.X && Y == other.Y && Z == other.Z;
}
inline bool operator !=(const Vec3 other) const
{
    return X != other.X || Y != other.Y || Z != other.Z;
}
inline bool operator<(Vec3 other) const
{
    return X < other.X && Y < other.Y && Z < other.Z;
}
inline bool operator<=(Vec3 other) const
{
    return X <= other.X && Y <= other.Y && Z <= other.Z;
}
inline bool operator>(Vec3 other) const
{
    return X > other.X && Y > other.Y && Z > other.Z;
}
inline bool operator>=(Vec3 other) const
{
    return X >= other.X && Y >= other.Y && Z >= other.Z;
}
static inline float Dot(const Vec3 A, const Vec3 B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}
inline float Length() const
{
    return sqrtf(Dot(*this, *this));
}
static inline Vec3 Min(const Vec3 A, const Vec3 B)
{
    return Vec3(fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z));
}
static inline Vec3 Max(const Vec3 A, const Vec3 B)
{
    return Vec3(fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z));
}
inline Vec3 Abs() const
{
    return Vec3(fabsf(X), fabsf(Y), fabsf(Z));
}
static inline Vec3 Lerp(const Vec3 A, const Vec3 B, const float amount)
{
    return Vec3(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount, A.Z + (B.Z - A.Z) * amount);
}