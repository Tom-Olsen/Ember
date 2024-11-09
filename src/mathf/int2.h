#pragma once
#ifndef __INCLUDE_GUARD_int2_h__
#define __INCLUDE_GUARD_int2_h__
#include <string>



struct Int2
{
public:
	// Members:
	int x, y;

	// Constructors:
	Int2();
	Int2(int xy);
	Int2(int x, int y);
	Int2(const Int2& xy);

	// Math operations:

	// Static math operations:

	// Access:
	int& operator[](int index);
	int operator[](int index) const;

	// Assignment:
	Int2& operator=(const Int2& other);
	Int2& operator=(Int2&& other) noexcept;

	// Addition:
	Int2 operator+(const Int2& other) const;
	Int2& operator+=(const Int2& other);

	// Substraction:
	Int2 operator-(const Int2& other) const;
	Int2& operator-=(const Int2& other);
	Int2 operator-() const;

	// Multiplication:
	Int2 operator*(const Int2& other) const;
	Int2& operator*=(const Int2& other);
	Int2 operator*(int scalar) const;
	Int2& operator*=(int scalar);

	// Comparison:
	bool operator==(const Int2& other) const;
	bool operator!=(const Int2& other) const;

	// Friend functions:
	friend Int2 operator*(int a, const Int2& b);

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Int2& value);

	// Static members:
	static Int2 zero;
	static Int2 one;
	static Int2 right;	// +x
	static Int2 left;	// -x
	static Int2 up;		// +y
	static Int2 down;	// -y
};



#endif // __INCLUDE_GUARD_int2_h__