#ifndef __INCLUDE_GUARD_shadowPushConstant_h__
#define __INCLUDE_GUARD_shadowPushConstant_h__
#include "mathf.h"
#include <string>



/// <summary>
/// Size limit for push constants is 128 bytes.
/// Only used for small data that is updated every frame.
/// </summary>
struct ShadowPushConstant
{
public: // Members:
	alignas(4) int shadowMapIndex;
	alignas(16) Float4x4 localToClipMatrix;

public: // Methods:
	ShadowPushConstant(int shadowMapIndex, const Float4x4& localToClipMatrix);
	std::string ToString();
};



#endif // __INCLUDE_GUARD_shadowPushConstant_h__