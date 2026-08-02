#pragma once
#include "commonMaterialRenderState.h"
#include "iMaterial.h"
#include "commonPipelineState.h"
#include "vulkanPipelineKey.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;
	class Pipeline;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class (core), making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public Shader, public emberBackendInterface::IMaterial
	{
	private: // Members:
		emberCommon::MaterialRenderState m_defaultRenderState;
		Material* m_pShadowMaterial;    // the shadow material used alongside this material.
		std::unordered_map<PipelineKey, std::unique_ptr<Pipeline>, PipelineKey::Hasher> m_pipelines;

	public: // Methods:
		// Factories/Destructor:
        static Material CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
		static Material CreateForward(const std::string& name, emberCommon::RenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv);
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
		void SetDefaultRenderState(const emberCommon::MaterialRenderState& defaultRenderState) override;
		void SetShadowMaterial(emberBackendInterface::IMaterial* pShadowMaterial) override;

		// Getters:
		const std::string& GetName() const override;
		const emberCommon::MaterialRenderState& GetDefaultRenderState() const override;
		Material* GetShadowMaterial() const override;
		emberBackendInterface::IDescriptorSetBinding* GetShaderDescriptorSetBinding() const override;
		const Pipeline* GetPipeline(const Mesh* pMesh, PipelineType pipelineType, emberCommon::RenderMode renderMode) const;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Constructor:
		Material(const std::string& name);

		// Pipeline lookup:
		bool HasPipeline(PipelineType pipelineType) const;
	};
}