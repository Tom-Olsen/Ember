#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
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
	
	protected: // Constructor (only for derived classes)
		Shader() = default;

	public: // Methods:
		virtual ~Shader();

		// Non-copyable:
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Movable:
		Shader(Shader&& other) noexcept = default;
		Shader& operator=(Shader&& other) noexcept = default;

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