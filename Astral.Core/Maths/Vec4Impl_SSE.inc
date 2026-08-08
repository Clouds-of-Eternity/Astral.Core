Vec4()
{
    asM128 = _mm_set1_ps(0.0f);
}
Vec4(float unit)
{
    asM128 = _mm_set1_ps(unit);
}
Vec4(float x, float y, float z, float w)
{
    asM128 = _mm_set_ps(w, z, y, x);
}
Vec4(Vec2 vec2, float z, float w)
{
    asM128 = _mm_set_ps(w, z, vec2.Y, vec2.X);
}
Vec4(Vec3 vec3, float w)
{
    asM128 = _mm_set_ps(w, vec3.Z, vec3.Y, vec3.X);
}

Vec4(vec4 cvec)
{
    asM128 = _mm_set_ps(cvec.W, cvec.Z, cvec.Y, cvec.X);
}
Vec4(__m128 m128)
{
    asM128 = m128;
}

// addition operators
inline Vec4 operator+(const Vec4 other) const
{
    Vec4 result;
    result.asM128 = _mm_add_ps(this->asM128, other.asM128);
    return result;
}
inline void operator+=(const Vec4 other)
{
    this->asM128 = _mm_add_ps(this->asM128, other.asM128);
}

// subtraction operators
inline Vec4 operator-(const Vec4 other) const
{
    Vec4 result;
    result.asM128 = _mm_sub_ps(this->asM128, other.asM128);
    return result;
}
inline void operator-=(const Vec4 other)
{
    this->asM128 = _mm_sub_ps(this->asM128, other.asM128);
}

// multiplication operators
inline Vec4 operator*(const Vec4 other) const
{
    Vec4 result;
    result.asM128 = _mm_mul_ps(this->asM128, other.asM128);
    return result;
}
inline void operator*=(const Vec4 other)
{
    asM128 = _mm_mul_ps(this->asM128, other.asM128);
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

// division operators
inline Vec4 operator/(const Vec4 other) const
{
    Vec4 result;
    result.asM128 = _mm_div_ps(this->asM128, other.asM128);
    return result;
}
inline void operator/=(const Vec4 other)
{
    asM128 = _mm_div_ps(this->asM128, other.asM128);
}
inline Vec4 operator/(float other) const
{
    __m128 val = _mm_set_ps1(other);
    val = _mm_div_ps(this->asM128, val);
    return Vec4(val);
}
inline void operator/=(float other)
{
    __m128 val = _mm_set_ps1(other);
    asM128 = _mm_div_ps(this->asM128, val);
}

inline Vec4 operator-() const
{
    return Vec4(_mm_mul_ps(asM128, _mm_set1_ps(-1.0f)));
}

inline i32 FindIndexOf(float component) const
{
    const __m128 component4 = _mm_set1_ps(component);
    __m128 equals = _mm_cmpeq_ps(asM128, component4);
    __m128 equalsSign = _mm_and_ps(equals, _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f));
    __m128 hadd0 = _mm_hadd_ps(equalsSign, equalsSign);
    __m128 hadd1 = _mm_hadd_ps(hadd0, hadd0);

    return (i32)_mm_cvtss_f32(hadd1) - 1;
}

static inline Vec4 Lerp4(const Vec4 A, const Vec4 B, Vec4 amount)
{
    __m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amount.asM128);
    __m128 mul = _mm_mul_ps(A.asM128, sub);
    __m128 fma = _mm_add_ps(_mm_mul_ps(B.asM128, amount.asM128), mul);
    return Vec4(fma);
}
static inline Vec4 Lerp(const Vec4 A, const Vec4 B, float amount)
{
    return Lerp4(A, B, Vec4(amount));
}

static inline Vec4 Abs(Vec4 value)
{
    return Vec4(_mm_andnot_ps(_mm_set1_ps(-0.0f), value.asM128));
}
static inline float Dot(const Vec4 A, const Vec4 B)
{
    __m128 mult = _mm_mul_ps(A.asM128, B.asM128);
    __m128 hadd0 = _mm_hadd_ps(mult, mult);
    __m128 hadd1 = _mm_hadd_ps(hadd0, hadd0);
    return _mm_cvtss_f32(hadd1);
    //__m128 dotProduct = _mm_dp_ps(A.asM128, B.asM128);
    //return _mm_cvtss_f32(dotProduct);
}
static inline float MinComponent(const Vec4 value)
{
    __m128 result = value.asM128;
    result = _mm_min_ps(result, _mm_shuffle_ps(result, result, _MM_SHUFFLE(2, 1, 0, 3)));
    result = _mm_min_ps(result, _mm_shuffle_ps(result, result, _MM_SHUFFLE(1, 0, 3, 2)));

    return _mm_cvtss_f32(result);
}
static inline float MaxComponent(const Vec4 value)
{
    __m128 result = value.asM128;
    result = _mm_max_ps(result, _mm_shuffle_ps(result, result, _MM_SHUFFLE(2, 1, 0, 3)));
    result = _mm_max_ps(result, _mm_shuffle_ps(result, result, _MM_SHUFFLE(1, 0, 3, 2)));

    return _mm_cvtss_f32(result);
}
static inline Vec4 Min(const Vec4 A, const Vec4 B)
{
    return Vec4(_mm_min_ps(A.asM128, B.asM128));
}
static inline Vec4 Max(const Vec4 A, const Vec4 B)
{
    return Vec4(_mm_max_ps(A.asM128, B.asM128));
}
inline bool operator==(Vec4 other) const
{
    __m128 compareResult = _mm_cmpeq_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111; //all elements equal
}
inline bool operator!=(Vec4 other) const
{
    __m128 compareResult = _mm_cmpneq_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111; //all elements not equal
}
inline bool operator<(Vec4 other) const
{
    __m128 compareResult = _mm_cmplt_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111; //all elements true
}
inline bool operator<=(Vec4 other) const
{
    __m128 compareResult = _mm_cmple_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111;
}
inline bool operator>(Vec4 other) const
{
    __m128 compareResult = _mm_cmpgt_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111;
}
inline bool operator>=(Vec4 other) const
{
    __m128 compareResult = _mm_cmpge_ps(asM128, other.asM128);
    return _mm_movemask_ps(compareResult) == 0b1111;
}