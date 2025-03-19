#ifndef __INCLUDE_GUARD_shader_h__
#define __INCLUDE_GUARD_shader_h__
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class Pipeline;
	struct DescriptorBoundResources;



	/// <summary>
	/// Polymorphic parent class for Material and ComputeShader.
	/// </summary>
	class Shader
	{
	protected: // Members:
		std::string m_name;
		std::unique_ptr<Pipeline> m_pPipeline;
		std::unique_ptr<DescriptorBoundResources> m_pDescriptorBoundResources;

	public: // Methods:
		virtual ~Shader();

		// Getters:
		const std::string& GetName() const;
		const Pipeline* const GetPipeline() const;
		const DescriptorBoundResources* const GetDescriptorBoundResources() const;

		// Debugging:
		void PrintShaderInfo() const;

	protected: // Methods:
		static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
	};
}



#endif // __INCLUDE_GUARD_shader_h__