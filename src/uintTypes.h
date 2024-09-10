#ifndef __INCLUDE_GUARD_uintTypes_h__
#define __INCLUDE_GUARD_uintTypes_h__
#include <string>
#include <iostream>
#include "glm/glm.hpp"



// using aliases for glm unsigned int types
using UInt2 = glm::uvec2;
using UInt3 = glm::uvec3;
using UInt4 = glm::uvec4;
using UInt2x2 = glm::umat2x2;
using UInt2x3 = glm::umat3x2;
using UInt2x4 = glm::umat4x2;
using UInt3x2 = glm::umat2x3;
using UInt3x3 = glm::umat3x3;
using UInt3x4 = glm::umat4x3;
using UInt4x2 = glm::umat2x4;
using UInt4x3 = glm::umat3x4;
using UInt4x4 = glm::umat4x4;



// to_string conversion for glm unsigned int types
inline std::string to_string(const UInt2& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}
inline std::string to_string(const UInt3& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}
inline std::string to_string(const UInt4& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
}

inline std::string to_string(const UInt2x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")";
}
inline std::string to_string(const UInt3x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")";
}
inline std::string to_string(const UInt4x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ")";
}

inline std::string to_string(const UInt2x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")";
}
inline std::string to_string(const UInt3x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")";
}
inline std::string to_string(const UInt4x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ")";
}

inline std::string to_string(const UInt2x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")";
}
inline std::string to_string(const UInt3x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")";
}
inline std::string to_string(const UInt4x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ", " + std::to_string(mat[3][3]) + ")";
}



// ostream operator overloads for glm unsigned int types
inline std::ostream& operator<<(std::ostream& os, const UInt2& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt3& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt4& vec)
{
	os << to_string(vec);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const UInt2x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt3x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt4x2& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const UInt2x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt3x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt4x3& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const UInt2x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt3x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const UInt4x4& mat)
{
	os << to_string(mat);
	return os;
}



#endif // __INCLUDE_GUARD_uintTypes_h__