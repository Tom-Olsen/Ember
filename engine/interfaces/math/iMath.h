#ifndef __INCLUDE_GUARD_iMath_h__
#define __INCLUDE_GUARD_iMath_h__
#include <array>
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
	using Float2x2 = std::array<float, 4>;
	using Float3x3 = std::array<float, 9>;
	using Float4x4 = std::array<float, 16>;


	// Constexpr default values:
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