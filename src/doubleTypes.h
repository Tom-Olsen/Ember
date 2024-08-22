#ifndef __INCLUDE_GUARD_doubleTypes_h__
#define __INCLUDE_GUARD_doubleTypes_h__
#include <string>
#include <iostream>
#include "glm/glm.hpp"



using Double2 = glm::dvec2;
using Double3 = glm::dvec3;
using Double4 = glm::dvec4;
using Double2x2 = glm::dmat2x2;
using Double2x3 = glm::dmat3x2;
using Double2x4 = glm::dmat4x2;
using Double3x2 = glm::dmat2x3;
using Double3x3 = glm::dmat3x3;
using Double3x4 = glm::dmat4x3;
using Double4x2 = glm::dmat2x4;
using Double4x3 = glm::dmat3x4;
using Double4x4 = glm::dmat4x4;



inline std::string to_string(const Double2& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}
inline std::string to_string(const Double3& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}
inline std::string to_string(const Double4& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
}

inline std::string to_string(const Double2x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")";
}
inline std::string to_string(const Double3x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")";
}
inline std::string to_string(const Double4x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ")";
}

inline std::string to_string(const Double2x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")";
}
inline std::string to_string(const Double3x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")";
}
inline std::string to_string(const Double4x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ")";
}

inline std::string to_string(const Double2x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")";
}
inline std::string to_string(const Double3x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")";
}
inline std::string to_string(const Double4x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ", " + std::to_string(mat[3][3]) + ")";
}



inline std::ostream& operator<<(std::ostream& os, const Double2& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double3& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double4& vec)
{
	os << to_string(vec);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Double2x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double3x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double4x2& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Double2x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double3x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double4x3& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Double2x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double3x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Double4x4& mat)
{
	os << to_string(mat);
	return os;
}
#endif // __INCLUDE_GUARD_doubleTypes_h__