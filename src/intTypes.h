#ifndef __INCLUDE_GUARD_intTypes_h__
#define __INCLUDE_GUARD_intTypes_h__
#include <string>
#include <iostream>
#include "glm/glm.hpp"



using Int2 = glm::ivec2;
using Int3 = glm::ivec3;
using Int4 = glm::ivec4;
using Int2x2 = glm::imat2x2;
using Int2x3 = glm::imat3x2;
using Int2x4 = glm::imat4x2;
using Int3x2 = glm::imat2x3;
using Int3x3 = glm::imat3x3;
using Int3x4 = glm::imat4x3;
using Int4x2 = glm::imat2x4;
using Int4x3 = glm::imat3x4;
using Int4x4 = glm::imat4x4;



inline std::string to_string(const Int2& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}
inline std::string to_string(const Int3& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}
inline std::string to_string(const Int4& vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
}

inline std::string to_string(const Int2x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")";
}
inline std::string to_string(const Int3x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")";
}
inline std::string to_string(const Int4x2& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ")";
}

inline std::string to_string(const Int2x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")";
}
inline std::string to_string(const Int3x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")";
}
inline std::string to_string(const Int4x3& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ")";
}

inline std::string to_string(const Int2x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")";
}
inline std::string to_string(const Int3x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")";
}
inline std::string to_string(const Int4x4& mat)
{
	return "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + ")" + "\n" +
		"(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + ")" + "\n" +
		"(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + ")" + "\n" +
		"(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ", " + std::to_string(mat[3][3]) + ")";
}



inline std::ostream& operator<<(std::ostream& os, const Int2& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int3& vec)
{
	os << to_string(vec);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int4& vec)
{
	os << to_string(vec);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Int2x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int3x2& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int4x2& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Int2x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int3x3& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int4x3& mat)
{
	os << to_string(mat);
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Int2x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int3x4& mat)
{
	os << to_string(mat);
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Int4x4& mat)
{
	os << to_string(mat);
	return os;
}



#endif // __INCLUDE_GUARD_intTypes_h__