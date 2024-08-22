#ifndef __INCLUDE_GUARD_floatTypes_h__
#define __INCLUDE_GUARD_floatTypes_h__
#include <string>
#include <iostream>
#include "glm/glm.hpp"



using Float2 = glm::vec2;
using Float3 = glm::vec3;
using Float4 = glm::vec4;
using Float2x2 = glm::mat2x2;
using Float2x3 = glm::mat3x2;
using Float2x4 = glm::mat4x2;
using Float3x2 = glm::mat2x3;
using Float3x3 = glm::mat3x3;
using Float3x4 = glm::mat4x3;
using Float4x2 = glm::mat2x4;
using Float4x3 = glm::mat3x4;
using Float4x4 = glm::mat4x4;



inline std::string to_string(const Float2& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}
inline std::string to_string(const Float3& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}
inline std::string to_string(const Float4& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
}

inline std::string to_string(const Float2x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")";
}
inline std::string to_string(const Float3x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")";
}
inline std::string to_string(const Float4x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ")";
}

inline std::string to_string(const Float2x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")";
}
inline std::string to_string(const Float3x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")";
}
inline std::string to_string(const Float4x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ")";
}

inline std::string to_string(const Float2x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")";
}
inline std::string to_string(const Float3x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")";
}
inline std::string to_string(const Float4x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ", " + std::to_string(mat[3][3]) + ")";
}



inline std::ostream& operator<<(std::ostream& os, const Float2& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float3& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float4& vec)
{
	os << to_string(vec);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Float2x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float3x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float4x2& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Float2x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float3x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float4x3& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Float2x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float3x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Float4x4& mat)
{
	os << to_string(mat);
	return os;
}
#endif // __INCLUDE_GUARD_floatTypes_h__