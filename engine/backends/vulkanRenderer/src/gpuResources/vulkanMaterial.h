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
	class Pipeline;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public Shader, public emberBackendInterface::IMaterial
	{
	public: // Enums:
		enum class PipelineType : uint8_t
		{
			gizmo,
			shadow,
			forward,
			present
		};

	private: // Members:
		emberCommon::RenderMode m_renderMode;
		int32_t m_renderQueue;          // opaque=0-999, transparent=1000-1999, skybox=2000-...
		Material* m_pShadowMaterial;    // the shadow material used alongside this material.
		std::vector<std::unique_ptr<Pipeline>> m_pGizmoPipelines;
		std::vector<std::unique_ptr<Pipeline>> m_pShadowPipelines;
		std::vector<std::unique_ptr<Pipeline>> m_pForwardPipelines;
		std::vector<std::unique_ptr<Pipeline>> m_pPresentPipelines;

	public: // Methods:
		// Factories/Destructor:
		static Material CreateForward(const std::string& name, emberCommon::RenderMode renderMode, int32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv);
		static Material CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other) noexcept;
		Material& operator=(Material&& other) noexcept;

		// Setters:
		void SetRenderQueue(int32_t renderQueue) override;
		void SetRenderMode(emberCommon::RenderMode renderMode) override;
		void SetShadowMaterial(emberBackendInterface::IMaterial* pShadowMaterial) override;

		// Getters:
		const std::string& GetName() const override;
		int32_t GetRenderQueue() const override;
		emberCommon::RenderMode GetRenderMode() const override;
		Material* GetShadowMaterial() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		const Pipeline* GetPipeline(const Mesh* pMesh, PipelineType pipelineType) const;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		Material(const std::string& name);

		// Pipeline indexing:
		bool HasPipeline(PipelineType pipelineType) const;
		size_t GetPipelineIndex(const Mesh* pMesh, PipelineType pipelineType) const;
	};
}