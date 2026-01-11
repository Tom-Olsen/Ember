#pragma once
#include "iMaterial.h"
#include "commonMaterialType.h"
#include "commonRenderQueue.h"
#include "vulkanRendererExport.h"
#include "vulkanShader.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>



// Forward declarations:
typedef struct VkBuffer_T* VkBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class VULKAN_RENDERER_API Material : public Shader, public emberBackendInterface::IMaterial
	{
	private: // Members:
		emberCommon::MaterialType m_type;
		uint32_t m_renderQueue;	// shadow=0, opaque=1000, transparent=2000, skybox=3000
		std::unique_ptr<VertexInputDescriptions> m_pVertexInputDescriptions;

	public: // Methods:
		// Constructors/Destructor:
		Material(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		Material(uint32_t shadowMapResolution);	// special constructor for shadowMaterial.
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other);
		Material& operator=(Material&& other);

		// Getters:
		const std::string& GetName() const override;
		emberCommon::MaterialType GetType() const override;
		uint32_t GetRenderQueue() const override;
		const VertexInputDescriptions* const GetVertexInputDescriptions() const;

		// Debugging:
		void Print() const override;
	};
}