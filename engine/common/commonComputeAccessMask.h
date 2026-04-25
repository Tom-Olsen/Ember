#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>



namespace emberCommon
{
	enum class ComputeAccessFlag : uint16_t
	{
		none = 0,
		uniformRead = 1 << 0,
		shaderRead = 1 << 1,
		shaderWrite = 1 << 2,
		sampledRead = 1 << 3,
		storageRead = 1 << 4,
		storageWrite = 1 << 5,
		memoryRead = 1 << 6,
		memoryWrite = 1 << 7,
		accelerationStructureRead = 1 << 8,
		descriptorBufferRead = 1 << 9
	};

    // Operator overloading:
	inline constexpr ComputeAccessFlag operator|(ComputeAccessFlag lhs, ComputeAccessFlag rhs)
	{
		using T = std::underlying_type_t<ComputeAccessFlag>;
		return static_cast<ComputeAccessFlag>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}
	inline constexpr ComputeAccessFlag operator&(ComputeAccessFlag lhs, ComputeAccessFlag rhs)
	{
		using T = std::underlying_type_t<ComputeAccessFlag>;
		return static_cast<ComputeAccessFlag>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}
	inline constexpr ComputeAccessFlag& operator|=(ComputeAccessFlag& lhs, ComputeAccessFlag rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}



    // Helper methods:
	inline constexpr bool HasFlag(ComputeAccessFlag flags, ComputeAccessFlag flag)
	{
		return (flags & flag) != ComputeAccessFlag::none;
	}
	inline std::string ComputeAccessFlagsToString(ComputeAccessFlag accessFlags)
	{
		if (accessFlags == ComputeAccessFlag::none)
			return "none";

		std::string result;
		auto appendFlag = [&result, accessFlags](ComputeAccessFlag flag, std::string_view name)
		{
			if (!HasFlag(accessFlags, flag))
				return;
			if (!result.empty())
				result += "|";
			result += name;
		};

		appendFlag(ComputeAccessFlag::uniformRead, "uniformRead");
		appendFlag(ComputeAccessFlag::shaderRead, "shaderRead");
		appendFlag(ComputeAccessFlag::shaderWrite, "shaderWrite");
		appendFlag(ComputeAccessFlag::sampledRead, "sampledRead");
		appendFlag(ComputeAccessFlag::storageRead, "storageRead");
		appendFlag(ComputeAccessFlag::storageWrite, "storageWrite");
		appendFlag(ComputeAccessFlag::memoryRead, "memoryRead");
		appendFlag(ComputeAccessFlag::memoryWrite, "memoryWrite");
		appendFlag(ComputeAccessFlag::accelerationStructureRead, "accelerationStructureRead");
		appendFlag(ComputeAccessFlag::descriptorBufferRead, "descriptorBufferRead");
		return result.empty() ? "unknown" : result;
	}
}