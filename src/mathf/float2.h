#ifndef __INCLUDE_GUARD_float2_h__
#define __INCLUDE_GUARD_float2_h__
#include <string>



struct Float3;
struct Float4;



struct Float2
{
private:
	constexpr static float s_epsilon = 1e-8f;
public:
	// Members:
	float x, y;

	// Constructors:
	Float2();
	Float2(float xy);
	Float2(float x, float y);
	Float2(const Float2& xy);
	Float2(const Float3& xy);
	Float2(const Float4& xy);
	static Float2 Direction(float radians);

	// Math operations:
	Float2 Abs() const;
	float Length2() const;
	float Length() const;
	float AngleDegrees() const;
	float AngleRadians() const;
	Float2 Normalize() const;
	Float2 Rotate(float radians) const;
	Float2 Rotate90() const;
	Float2 Rotate270() const;
	bool IsEpsilonZero() const;

	// Static math operations:
	static float Dot(const Float2& a, const Float2& b);
	static float Cross(const Float2& a, const Float2& b);
	static float Distance2(const Float2& a, const Float2& b);
	static float Distance(const Float2& a, const Float2& b);
	static float AngleDegrees(const Float2& a, const Float2& b);
	static float AngleRadians(const Float2& a, const Float2& b);
	static Float2 Min(const Float2& a, const Float2& b);
	static Float2 Max(const Float2& a, const Float2& b);
	static Float2 Clamp(const Float2& value, const Float2& min, const Float2& max);

	// Access:
	float& operator[](int index);
	float operator[](int index) const;

	// Assignment:
	Float2& operator=(const Float2& other);
	Float2& operator=(Float2&& other) noexcept;

	// Addition:
	Float2 operator+(const Float2& other) const;
	Float2& operator+=(const Float2& other);

	// Substraction:
	Float2 operator-(const Float2& other) const;
	Float2& operator-=(const Float2& other);
	Float2 operator-() const;

	// Multiplication:
	Float2 operator*(const Float2& other) const;
	Float2& operator*=(const Float2& other);
	Float2 operator*(float scalar) const;
	Float2& operator*=(float scalar);

	// Division:
	Float2 operator/(const Float2& other) const;
	Float2& operator/=(const Float2& other);
	Float2 operator/(float scalar) const;
	Float2& operator/=(float scalar);

	// Comparison:
	bool IsEpsilonEqual(const Float2& other) const;
	bool operator==(const Float2& other) const;
	bool operator!=(const Float2& other) const;

	// Friend functions:
	friend Float2 operator*(float a, const Float2& b);

	// Conversion:
	Float2 ToDegrees() const;
	Float2 ToRadians() const;

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Float2& value);

	// Static members:
	static Float2 zero;
	static Float2 one;
	static Float2 right;	// +x = ( 1, 0).
	static Float2 left;		// -x = (-1, 0).
	static Float2 up;		// +y = ( 0, 1).
	static Float2 down;		// -y = ( 0,-1).
};

// Friend functions:
Float2 operator/(float scalar, const Float2& vector);



#endif // __INCLUDE_GUARD_float2_h__