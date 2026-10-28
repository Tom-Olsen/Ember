#ifndef __INCLUDE_GUARD_instanceData_h__
#define __INCLUDE_GUARD_instanceData_h__



#ifdef __HLSL_VERSION
struct InstanceData
{
    float4x4 localToWorldMatrix; // Per-instance TRS matrix relative to the group transform.
    float4 color;
};
#endif



#ifdef __cplusplus
#include "float4.h"
#include "float4x4.h"
#include <cstddef>
#include <type_traits>



namespace emberCommon
{
	struct InstanceData
	{
	public: // Members:
		emberMath::Float4x4 localToWorldMatrix; // Per-instance TRS matrix relative to the group transform.
		emberMath::Float4 color;
	};

	static_assert(std::is_standard_layout_v<InstanceData>);
	static_assert(offsetof(InstanceData, localToWorldMatrix) == 0);
	static_assert(offsetof(InstanceData, color) == 64);
	static_assert(sizeof(InstanceData) == 80);
}
#endif



#endif // __INCLUDE_GUARD_instanceData_h__