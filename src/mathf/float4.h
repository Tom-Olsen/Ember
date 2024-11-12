#pragma once
#ifndef __INCLUDE_GUARD_float4_h__
#define __INCLUDE_GUARD_float4_h__
#include <string>



// Forward declarations:
struct Float2;
struct Float3;



struct Float4
{
private:
	constexpr static float epsilon = 1e-8f;
public:
	// Members:
	float x, y, z, w;

	// Constructors:
	Float4();
	Float4(float xyzw);
	Float4(float x, float y);
	Float4(float x, float y, float z);
	Float4(float x, float y, float z, float w);
	Float4(const Float2& xy);
	Float4(const Float2& xy, float z);
	Float4(const Float2& xy, float z, float w);
	Float4(const Float2& xy, Float2 zw);
	Float4(const Float3& xyz);
	Float4(const Float3& xyz, float w);
	Float4(const Float4& xyzw);
	static Float4 Direction(float theta, float phi);

	// Math operations:
	float Length2() const;
	float Length() const;
	bool IsEpsilonZero() const;

	// Static math operations:

	// Access:
	float& operator[](int index);
	float operator[](int index) const;
	Float3 xyz() const;

	// Assignment:
	Float4& operator=(const Float4& other);
	Float4& operator=(Float4&& other) noexcept;

	// Addition:
	Float4 operator+(const Float4& other) const;
	Float4& operator+=(const Float4& other);

	// Substraction:
	Float4 operator-(const Float4& other) const;
	Float4& operator-=(const Float4& other);
	Float4 operator-() const;

	// Multiplication:
	Float4 operator*(const Float4& other) const;
	Float4& operator*=(const Float4& other);
	Float4 operator*(float scalar) const;
	Float4& operator*=(float scalar);

	// Division:
	Float4 operator/(const Float4& other) const;
	Float4& operator/=(const Float4& other);
	Float4 operator/(float scalar) const;
	Float4& operator/=(float scalar);

	// Comparison:
	bool IsEpsilonEqual(const Float4& other) const;
	bool operator==(const Float4& other) const;
	bool operator!=(const Float4& other) const;

	// Friend functions:
	friend Float4 operator*(float a, const Float4& b);

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Float4& value);

	// Static members:
	static Float4 zero;
	static Float4 one;
	static Float4 right;	// +x = ( 1, 0, 0, 0).
	static Float4 left;		// -x = (-1, 0, 0, 0).
	static Float4 up;		// +y = ( 0, 1, 0, 0).
	static Float4 down;		// -y = ( 0,-1, 0, 0).
	static Float4 forward;	// +z = ( 0, 0, 1, 0).
	static Float4 backward;	// -z = ( 0, 0,-1, 0).
	static Float4 in;		// +w = ( 0, 0, 0, 1).
	static Float4 out;		// -w = ( 0, 0, 0,-1).
};



#endif // __INCLUDE_GUARD_float4_h__