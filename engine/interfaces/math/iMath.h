#ifndef __INCLUDE_GUARD_iMath_h__
#define __INCLUDE_GUARD_iMath_h__
#include <array>
#include <math.h>
#include <sstream>



namespace iMath
{
	using Int2 = std::array<int, 2>;
	using Int3 = std::array<int, 3>;
	using Int4 = std::array<int, 4>;
	using Uint2 = std::array<uint32_t, 2>;
	using Uint3 = std::array<uint32_t, 3>;
	using Uint4 = std::array<uint32_t, 4>;
	using Float2 = std::array<float, 2>;
	using Float3 = std::array<float, 3>;
	using Float4 = std::array<float, 4>;
	using Float2x2 = std::array<float, 4>;  // column major: 0 3 6   [0+3*0] [0+3*1] [0+3*2]
	using Float3x3 = std::array<float, 9>;  // column major: 1 4 7 = [1+3*0] [1+3*1] [1+3*2]
	using Float4x4 = std::array<float, 16>; // column major: 2 5 8   [2+3*0] [2+3*1] [2+3*2]



	// Constexpr default values:
    constexpr float pi =  3.14159265358979323846f;
    constexpr float pi2 = 0.50f * pi;

	constexpr Uint3 Uint3Zero = Uint3{ 0, 0, 0 };
	constexpr Uint3 Uint3One = Uint3{ 1, 1, 1 };

	constexpr Float2 Float2Zero    = Float2{  0.0f,  0.0f };
	constexpr Float2 Float2One     = Float2{  1.0f,  1.0f };
	constexpr Float2 Float2Right   = Float2{  1.0f,  0.0f };
	constexpr Float2 Float2Left    = Float2{ -1.0f,  0.0f };
	constexpr Float2 Float2Forward = Float2{  0.0f,  1.0f };
	constexpr Float2 Float2Back    = Float2{  0.0f, -1.0f };

	constexpr Float3 Float3Zero    = Float3{  0.0f,  0.0f,  0.0f };
	constexpr Float3 Float3One     = Float3{  1.0f,  1.0f,  1.0f };
	constexpr Float3 Float3Right   = Float3{  1.0f,  0.0f,  0.0f };
	constexpr Float3 Float3Left    = Float3{ -1.0f,  0.0f,  0.0f };
	constexpr Float3 Float3Forward = Float3{  0.0f,  1.0f,  0.0f };
	constexpr Float3 Float3Back    = Float3{  0.0f, -1.0f,  0.0f };
	constexpr Float3 Float3Up      = Float3{  0.0f,  0.0f,  1.0f };
	constexpr Float3 Float3Down    = Float3{  0.0f,  0.0f, -1.0f };

	constexpr Float4 Float4Zero    = Float4{  0.0f,  0.0f,  0.0f,  0.0f };
	constexpr Float4 Float4One     = Float4{  1.0f,  1.0f,  1.0f,  1.0f };
	constexpr Float4 Float4Right   = Float4{  1.0f,  0.0f,  0.0f,  0.0f };
	constexpr Float4 Float4Left    = Float4{ -1.0f,  0.0f,  0.0f,  0.0f };
	constexpr Float4 Float4Forward = Float4{  0.0f,  1.0f,  0.0f,  0.0f };
	constexpr Float4 Float4Back    = Float4{  0.0f, -1.0f,  0.0f,  0.0f };
	constexpr Float4 Float4Up      = Float4{  0.0f,  0.0f,  1.0f,  0.0f };
	constexpr Float4 Float4Down    = Float4{  0.0f,  0.0f, -1.0f,  0.0f };
	constexpr Float4 Float4In      = Float4{  0.0f,  0.0f,  0.0f,  1.0f };
	constexpr Float4 Float4Out     = Float4{  0.0f,  0.0f,  0.0f, -1.0f };

	constexpr Float3x3 Float3x3Identity = Float3x3
    { 1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f };
	constexpr Float4x4 Float4x4Identity = Float4x4
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };



    // Basic operations:
    inline Float4x4 Multiply(const Float4x4& a, const Float4x4& b)
    {
		Float4x4 result;
		for (uint32_t i = 0; i < 4; i++)
			for (uint32_t j = 0; j < 4; j++)
				for (uint32_t k = 0; k < 4; k++)
					result[i + 4 * j] += a[i + 4* k] * b[k + 4 * j];
		return result;
    }
	inline Float4x4 Float4x4RotateX(float angle)
	{
		float c = std::cos(angle);
		float s = std::sin(angle);
		return Float4x4
		{1.0f, 0.0f, 0.0f, 0.0f,
		 0.0f,    c,   -s, 0.0f,
		 0.0f,    s,    c, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f};
	}
	inline Float4x4 Float4x4RotateY(float angle)
	{
		float c = std::cos(angle);
		float s = std::sin(angle);
		return Float4x4
		{   c, 0.0f,    s, 0.0f,
		 0.0f, 1.0f, 0.0f, 0.0f,
		   -s, 0.0f,    c, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f};
	}
	inline Float4x4 Float4x4RotateZ(float angle)
	{
		float c = std::cos(angle);
		float s = std::sin(angle);
		return Float4x4
		{   c,   -s, 0.0f, 0.0f,
		    s,    c, 0.0f, 0.0f,
		 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 1.0f};
	}
	
    

    // ToString methods:
	inline std::string ToStringInt2(Int2 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ")";
		return oss.str();
	}
	inline std::string ToStringInt3(Int3 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ")";
		return oss.str();
	}
	inline std::string ToStringInt4(Int4 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ")";
		return oss.str();
	}

	inline std::string ToStringUint2(Uint2 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ")";
		return oss.str();
	}
	inline std::string ToStringUint3(Uint3 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ")";
		return oss.str();
	}
	inline std::string ToStringUint4(Uint4 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ")";
		return oss.str();
	}

	inline std::string ToStringFloat2(Float2 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ")";
		return oss.str();
	}
	inline std::string ToStringFloat3(Float3 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ")";
		return oss.str();
	}
	inline std::string ToStringFloat4(Float4 data)
	{
		std::ostringstream oss;
		oss << "(" << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ")";
		return oss.str();
	}

	inline std::string ToStringFloat2x2(Float2x2 data)
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[2] << ")\n";
		oss << "(" << data[1] << ", " << data[3] << ")";
		return oss.str();
	}
	inline std::string ToStringFloat3x3(Float3x3 data)
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[3] << ", " << data[6] << ")\n";
		oss << "(" << data[1] << ", " << data[4] << ", " << data[7] << ")\n";
		oss << "(" << data[2] << ", " << data[5] << ", " << data[8] << ")";
		return oss.str();
	}
	inline std::string ToStringFloat4x4(Float4x4 data)
	{
		std::ostringstream oss;
		oss << "\n";
		oss << "(" << data[0] << ", " << data[4] << ", " << data[ 8] << ", " << data[12] << ")\n";
		oss << "(" << data[1] << ", " << data[5] << ", " << data[ 9] << ", " << data[13] << ")\n";
		oss << "(" << data[2] << ", " << data[6] << ", " << data[10] << ", " << data[14] << ")\n";
		oss << "(" << data[3] << ", " << data[7] << ", " << data[11] << ", " << data[15] << ")";
		return oss.str();
	}
}



#endif // __INCLUDE_GUARD_iMath_h__