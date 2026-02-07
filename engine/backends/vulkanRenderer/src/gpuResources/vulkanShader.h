#pragma once
#include "shaderReflection.h"
#include "vulkanRendererExport.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Pipeline;



	/// <summary>
	/// Polymorphic parent class for Material and ComputeShader.
	/// </summary>
	class VULKAN_RENDERER_API Shader
	{
	protected: // Members:
		std::string m_name;
		std::vector<std::unique_ptr<Pipeline>> m_pPipelines;
		emberSpirvReflect::ShaderReflection m_shaderReflection;
	
	protected:  // Methods:
		// Constructor:
		Shader();

	public: // Methods:
		// Destructor:
		virtual ~Shader();

		// Non-copyable:
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Movable:
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		//const Pipeline* const GetPipeline() const;
		const emberSpirvReflect::ShaderReflection& const GetShaderReflection() const;

		// Debugging:
		void PrintShaderInfo() const;
	};
}