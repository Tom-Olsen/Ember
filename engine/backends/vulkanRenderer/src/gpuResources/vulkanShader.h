#pragma once
#include "shaderReflection.h"
#include "vulkanGpuResourceHandle.h"
#include "vulkanRendererExport.h"
#include <filesystem>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ShaderHandle;
	template<typename T>
	class GpuResourceRegistry;



	/// <summary>
	/// Polymorphic parent class for Material and ComputeShader.
	/// The core owns published shader interface pointers. The backend registry only provides stable,
	/// non-owning identity for internal borrowers when a shader value is moved.
	/// </summary>
	class VULKAN_RENDERER_API Shader
	{
		// Friends:
		friend class ShaderHandle;

	private: // Static members:
		static GpuResourceRegistry<Shader> s_resourceRegistry;

	protected: // Members:
		std::string m_debugName;
		emberSpirvReflect::ShaderReflection m_shaderReflection;
		std::vector<VkDescriptorSetLayout> m_vkDescriptorSetLayouts;
		VkPipelineLayout m_vkPipelineLayout;
		GpuResourceHandle m_registrationHandle;
	
	protected: // Methods:
		// Constructor:
		Shader(const std::string& debugName);

	public: // Methods:
		// Destructor:
		virtual ~Shader();

		// Non-copyable:
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		// Movable:
		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		// Create descriptor sets:
		void CreateDescriptorSetLayout();

		// Getters:
		const emberSpirvReflect::ShaderReflection& GetShaderReflection() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		const std::vector<VkDescriptorSetLayout>& GetVkDescriptorSetLayout() const;
		const std::string& GetDebugName() const;

		// Debugging:
		void PrintShaderInfo() const;

	private: // Methods:
		void Cleanup();
		void UnregisterResource();
		void RebindResource();
	};
}