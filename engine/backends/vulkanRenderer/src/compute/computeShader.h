#ifndef __INCLUDE_GUARD_vulkanRendererBackend_computeShader_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_computeShader_h__
#include "iMath.h"
#include "shader.h"
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class ComputeShader : public Shader
	{
	private: // Members:
		iMath::Uint3 m_blockSize;

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		~ComputeShader();

		iMath::Uint3 GetBlockSize() const;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_computeShader_h__