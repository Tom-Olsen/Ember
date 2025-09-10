#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanShadowPushConstant_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanShadowPushConstant_h__
#include "iMath.h"
#include <string>



namespace vulkanRendererBackend
{
	/// <summary>
	/// instanceCount encoded in localToWorldMatrix entry [3,0].
	/// shadowMapIndex encoded in localToWorldMatrix entry [3,1].
	/// when extracting data in shader don't forget to reset these values to 0 in the localToWorldMatrix.
	/// </summary>
	struct ShadowPushConstant
	{
	private:
		alignas(16) iMath::Float4x4 localToWorldMatrix;
		alignas(16) iMath::Float4x4 worldToClipMatrix;

	public:
		ShadowPushConstant(int instanceCount, int shadowMapIndex, const iMath::Float4x4& localToWorldMatrix, const iMath::Float4x4& worldToClipMatrix);
		std::string ToString();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanShadowPushConstant_h__