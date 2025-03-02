#ifndef __INCLUDE_GUARD_shadowPushConstant_h__
#define __INCLUDE_GUARD_shadowPushConstant_h__
#include "emberMath.h"
#include <string>



namespace emberEngine
{
	/// <summary>
	/// Size limit for push constants is 128 bytes.
	/// Only used for small data that is updated every frame.
	/// </summary>
	struct ShadowPushConstant
	{
		alignas(4) uint32_t instanceCount;
		alignas(4) int shadowMapIndex;
		alignas(16) Float4x4 localToWorldMatrix;
		alignas(16) Float4x4 worldToClipMatrix;

		ShadowPushConstant(uint32_t instanceCount, int shadowMapIndex, const Float4x4& localToWorldMatrix, const Float4x4& worldToClipMatrix);
		std::string ToString();
	};
}



#endif // __INCLUDE_GUARD_shadowPushConstant_h__