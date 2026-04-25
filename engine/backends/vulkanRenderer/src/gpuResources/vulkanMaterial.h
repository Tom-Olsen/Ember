#pragma once
#include "iMaterial.h"
#include "commonPipelineState.h"
#include "commonRenderQueue.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



// Forward declarations:
typedef struct VkBuffer_T* VkBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public Shader, public emberBackendInterface::IMaterial
	{
	private: // Enums:
		enum class Type : uint8_t
		{
			forward,
			shadow,
			present
		};

	private: // Members:
		Type m_type;
		emberCommon::RenderMode m_renderMode;
		uint32_t m_renderQueue; // opaque=0-999, transparent=1000-1999, skybox=2000-...

	private: // Methods:
		// Constructor:
		Material(const std::string& name);

        // Pipeline indexing:
		size_t GetPipelineIndex(const Mesh* pMesh) const;

	public: // Methods:
		// Factories/Destructor:
		static Material CreateForward(const std::string& name, emberCommon::RenderMode renderMode, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateShadow(const std::string& name, uint32_t shadowMapResolution);
		static Material CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other) noexcept;
		Material& operator=(Material&& other) noexcept;

		// Setters:
		void SetRenderQueue(uint32_t renderQueue) override;
		void SetRenderMode(emberCommon::RenderMode renderMode) override;

		// Getters:
		const std::string& GetName() const override;
		uint32_t GetRenderQueue() const override;
		emberCommon::RenderMode GetRenderMode() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		const Pipeline* GetPipeline(const Mesh* pMesh) const;

		// Debugging:
		void Print() const override;
	};
}