#pragma once
#ifndef __INCLUDE_GUARD_pushConstant_h__
#define __INCLUDE_GUARD_pushConstant_h__
#include "glmTypes.h"



// Size limit for push constants is 128 bytes.
// Should only be used for small data that is frequently updated.
// IMO don't use it at all, use uniform buffers instead, as they are more flexible,
// and push constants don't scale well, as they are per command buffer.
// (e.g. agregation of multiple draw calls into one (batching) wont work!)
struct PushConstantObject
{
	alignas(16) Float4x4 transform;	// 64 bytes
	alignas(16) Float4x4 projView;	// 64 bytes

	PushConstantObject()
	{
		transform = Float4x4(1.0f);
		projView = Float4x4(1.0f);
	}

	PushConstantObject(Float4x4 transform, Float4x4 projView)
	{
		this->transform = transform;
		this->projView = projView;
	}
};



#endif // __INCLUDE_GUARD_pushConstant_h__