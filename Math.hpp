#pragma once
#include <cmath>

constexpr float PI = 3.14159265f;
constexpr float RadToDeg = 180.0f / PI;
constexpr float DegToRad = PI / 180.0f;
constexpr float OneDivPI = 1.0f / PI;
constexpr float PIDiv2 = PI / 2.0f;
constexpr float TwoPI = PI * 2.0f;

template<typename T> inline T Max(const T a, const T b) { return a > b ? a : b; }
template<typename T> inline T Min(const T a, const T b) { return a < b ? a : b; }
template<typename T> inline T Clamp(const T x, const T a, const T b) { return Max(a, Min(b, x)); }

template<typename RealT>
inline RealT Lerp(const RealT from, const RealT to, const RealT t) noexcept {
	return from + (to - from) * t;
}

inline float rsqrt(float number) {
	long i;
	float x2, y;
	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( 1.5f - ( x2 * y * y ) );   // 1st iteration
	return y;
}

template<typename T>
struct Vector2
{
	union
	{
		struct { T x, y; };
		T arr[2];
	};

	static constexpr int NumElements = 2;
	static constexpr uint64 ElementSize = sizeof(T);
	using ElemType = T;
	
	Vector2()                   noexcept  : x(0), y(0) { }
	constexpr Vector2(T a, T b) noexcept  : x(a), y(b) { }
	T& operator[] (int index) { return arr[index]; }
	T operator[] (int index) const { return arr[index]; }
	
	float Length()	      const { return sqrtf(LengthSquared()); }
	float LengthSquared() const { return x * x + y * y; }

	static float Distance(const Vector2<T> a, const Vector2<T> b) {
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return sqrtf(diffx * diffx + diffy * diffy);
	}
        // distance squared
	static float DistanceSq(const Vector2<T> a, const Vector2<T> b) {
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return diffx * diffx + diffy * diffy;
	}

	void Normalized() const { *this /= Length(); }
	
	Vector2 Normalize(Vector2 other) { return other.Normalize(); }
	
	Vector2 operator-() { return Vector2(-x, -y); }
	Vector2 operator+(Vector2 other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator*(Vector2 other) const { return Vector2(x * other.x, y * other.y); }
	Vector2 operator/(Vector2 other) const { return Vector2(x / other.x, y / other.y); }
	Vector2 operator-(Vector2 other) const { return Vector2(x - other.x, y - other.y); }

	Vector2 operator+(T other) const { return Vector2(x + other, y + other); }
	Vector2 operator*(T other) const { return Vector2(x * other, y * other); }
	Vector2 operator/(T other) const { return Vector2(x / other, y / other); }
	Vector2 operator-(T other) const { return Vector2(x - other, y - other); }
	
	Vector2 operator += (Vector2 o) { x += o.x; y += o.y; return *this; }
	Vector2 operator *= (Vector2 o) { x *= o.x; y *= o.y; return *this; }
	Vector2 operator /= (Vector2 o) { x /= o.x; y /= o.y; return *this; }
	Vector2 operator -= (Vector2 o) { x -= o.x; y -= o.y; return *this; }

	Vector2 operator += (T o) { x += o; y += o; return *this; }
	Vector2 operator *= (T o) { x *= o; y *= o; return *this; }
	Vector2 operator /= (T o) { x /= o; y /= o; return *this; }
	Vector2 operator -= (T o) { x -= o; y -= o; return *this; }

	bool operator == (Vector2 b) const { return x == b.x && y == b.y; }
	bool operator != (Vector2 b) const { return x != b.x || y != b.y; }
	bool operator <  (Vector2 b) const { return x < b.x && y < b.y; }
	
	static constexpr Vector2 Zero()     { return Vector2( 0,  0); } 
	static constexpr Vector2 One()      { return Vector2( 1,  1); } 
	static constexpr Vector2 Up()       { return Vector2( 0,  1); } 
	static constexpr Vector2 Left()     { return Vector2(-1,  0); } 
	static constexpr Vector2 Down()     { return Vector2( 0, -1); } 
	static constexpr Vector2 Right()    { return Vector2( 1,  0); } 
};

template<typename T>
inline Vector2<T> MinT(const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Min(a.x, b.x), Min(a.y, b.y)); }
template<typename T>
inline Vector2<T> MaxT(const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Max(a.x, b.x), Max(a.y, b.y)); }

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2s = Vector2<short>;

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2s = Vector2<short>;
