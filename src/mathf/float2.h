#ifndef __INCLUDE_GUARD_float2_h__
#define __INCLUDE_GUARD_float2_h__
#include <string>



struct Float3;
struct Float4;



struct Float2
{
public:
	// Members:
	float x, y;

	// Constructors:
	Float2();
	Float2(float xy);
	Float2(float x, float y);
	Float2(const Float2& xy);
	explicit Float2(const Float3& xy);
	explicit Float2(const Float4& xy);
	static Float2 Direction(float angle);

	// Math operations:
	float LengthSq() const;
	float Length() const;
	float Angle() const;
	Float2 Normalize() const;
	Float2 Rotate(float angle) const;
	bool IsEpsilonZero() const;

	// Static math operations:
	static Float2 Abs(const Float2& a);
	static Float2 Round(const Float2& value, int decimals = 0);
	static float Dot(const Float2& a, const Float2& b);
	static float Cross(const Float2& a, const Float2& b);
	static float DistanceSq(const Float2& a, const Float2& b);
	static float Distance(const Float2& a, const Float2& b);
	static float Angle(const Float2& a, const Float2& b);
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
	friend Float2 operator*(const Float2& a, float b);

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Float2& value);

	// Static members:
	// Numbers:
	static Float2 zero;		// (0, 0).
	static Float2 one;		// (1, 1).
	static Float2 max;		// mathf::max * (1, 1).
	static Float2 min;		// mathf::min * (1, 1).

	// Directions:
	static Float2 right;	// +x = ( 1, 0).
	static Float2 left;		// -x = (-1, 0).
	static Float2 forward;	// +y = ( 0, 1).
	static Float2 back;		// -y = ( 0,-1).
};

// Friend functions:
Float2 operator/(float scalar, const Float2& vector);



#endif // __INCLUDE_GUARD_float2_h__