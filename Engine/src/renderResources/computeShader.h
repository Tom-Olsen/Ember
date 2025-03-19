#ifndef __INCLUDE_GUARD_computeShader_h__
#define __INCLUDE_GUARD_computeShader_h__
#include "emberMath.h"
#include "shader.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	class ComputeShader : public Shader
	{
	private: // Members:
		Uint3 m_blockSize;

	public: // Methods:
		// Constructors/Destructor:
		ComputeShader(const std::string& name, const std::filesystem::path& computeSpv);
		~ComputeShader();

		Uint3 GetBlockSize() const;
	};
}



#endif // __INCLUDE_GUARD_computeShader_h__