#include "materialRole.h"
#include <string_view>



namespace emberCore
{
	// Operator overloading:
	MaterialRole operator|(MaterialRole lhs, MaterialRole rhs)
	{
		return static_cast<MaterialRole>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
	}
	MaterialRole operator&(MaterialRole lhs, MaterialRole rhs)
	{
		return static_cast<MaterialRole>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}
	MaterialRole operator^(MaterialRole lhs, MaterialRole rhs)
	{
		return static_cast<MaterialRole>(static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
	}
	MaterialRole operator~(MaterialRole role)
	{
		return static_cast<MaterialRole>(~static_cast<uint8_t>(role));
	}
	MaterialRole& operator|=(MaterialRole& lhs, MaterialRole rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}
	MaterialRole& operator&=(MaterialRole& lhs, MaterialRole rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}
	MaterialRole& operator^=(MaterialRole& lhs, MaterialRole rhs)
	{
		lhs = lhs ^ rhs;
		return lhs;
	}
	bool HasMaterialRole(MaterialRole roles, MaterialRole role)
	{
		return (roles & role) == role;
	}



	// Debugging:
	std::string MaterialRoleToString(MaterialRole roles)
	{
		if (roles == MaterialRole::none)
			return "none";

		std::string result;
		auto appendRole = [&result, roles](MaterialRole role, std::string_view name)
		{
			if (!HasMaterialRole(roles, role))
				return;
			if (!result.empty())
				result += "|";
			result += name;
		};

		appendRole(MaterialRole::accessible, "accessible");
		appendRole(MaterialRole::deletable, "deletable");
		appendRole(MaterialRole::immutable, "immutable");
		return result.empty() ? "unknown" : result;
	}
}