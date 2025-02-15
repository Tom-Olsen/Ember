#ifndef __INCLUDE_GUARD_computeShader_h__
#define __INCLUDE_GUARD_computeShader_h__
#include "shader.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct VulkanContext;



	class ComputeShader : public Shader
	{
	private: // Members:

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(VulkanContext* pContext, const std::string& name, const std::filesystem::path& computeSpv);
		~ComputeShader();
	};
}



#endif // __INCLUDE_GUARD_computeShader_h__