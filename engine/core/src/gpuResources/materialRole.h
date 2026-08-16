#pragma once
#include "emberCoreExport.h"
#include <cstdint>
#include <string>



namespace emberCore
{
	enum class MaterialRole : uint8_t
	{
		none = 0,
		accessible = 1 << 0,
		deletable = 1 << 1,
		immutable = 1 << 2,
		defaultRole = accessible | deletable
	};

	// Operator overloading:
	EMBER_CORE_API MaterialRole operator|(MaterialRole lhs, MaterialRole rhs);
	EMBER_CORE_API MaterialRole operator&(MaterialRole lhs, MaterialRole rhs);
	EMBER_CORE_API MaterialRole operator^(MaterialRole lhs, MaterialRole rhs);
	EMBER_CORE_API MaterialRole operator~(MaterialRole role);
	EMBER_CORE_API MaterialRole& operator|=(MaterialRole& lhs, MaterialRole rhs);
	EMBER_CORE_API MaterialRole& operator&=(MaterialRole& lhs, MaterialRole rhs);
	EMBER_CORE_API MaterialRole& operator^=(MaterialRole& lhs, MaterialRole rhs);
	[[nodiscard]] EMBER_CORE_API bool HasMaterialRole(MaterialRole roles, MaterialRole role);
	
	// Debugging:
	[[nodiscard]] EMBER_CORE_API std::string MaterialRoleToString(MaterialRole roles);
}