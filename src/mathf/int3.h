#pragma once
#ifndef __INCLUDE_GUARD_int3_h__
#define __INCLUDE_GUARD_int3_h__
#include "int2.h"
#include <string>



struct Int3
{
public:
	// Members:
	int x, y, z;

	// Constructors:
	Int3();
	Int3(int xyz);
	Int3(int x, int y);
	Int3(int x, int y, int z);
	Int3(const Int2& xy);
	Int3(const Int2& xy, int z);
	Int3(const Int3& xyz);

	// Math operations:

	// Static math operations:

	// Access:
	int& operator[](int index);
	int operator[](int index) const;

	// Assignment:
	Int3& operator=(const Int3& other);
	Int3& operator=(Int3&& other) noexcept;

	// Addition:
	Int3 operator+(const Int3& other) const;
	Int3& operator+=(const Int3& other);

	// Substraction:
	Int3 operator-(const Int3& other) const;
	Int3& operator-=(const Int3& other);
	Int3 operator-() const;

	// Multiplication:
	Int3 operator*(const Int3& other) const;
	Int3& operator*=(const Int3& other);
	Int3 operator*(int scalar) const;
	Int3& operator*=(int scalar);

	// Comparison:
	bool operator==(const Int3& other) const;
	bool operator!=(const Int3& other) const;

	// Friend functions:
	friend Int3 operator*(int a, const Int3& b);

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Int3& value);

	// Static members:
	static Int3 zero;
	static Int3 one;
	static Int3 right;		// +x = ( 1, 0, 0).
	static Int3 left;		// -x = (-1, 0, 0).
	static Int3 up;			// +y = ( 0, 1, 0).
	static Int3 down;		// -y = ( 0,-1, 0).
	static Int3 forward;	// +z = ( 0, 0, 1).
	static Int3 backward;	// -z = ( 0, 0,-1).
};



#endif // __INCLUDE_GUARD_int3_h__