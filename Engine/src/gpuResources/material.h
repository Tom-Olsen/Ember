#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include "shader.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	class Mesh;
	namespace vulkanBackend
	{
		struct VertexInputDescriptions;
	}



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all Materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class Material : public Shader
	{
	public: // Enums:
		enum class Type
		{
			forwardOpaque,
			forwardTransparent,
			shadow,
			skybox,
			present
		};
		enum class Queue
		{
			opaque = 1000,
			skybox = 2000,
			transparent = 3000
		};

	private: // Members:
		Type m_type;
		uint32_t m_renderQueue;	// shadow=0, opaque=1000, transparent=2000, skybox=3000
		std::unique_ptr<vulkanBackend::VertexInputDescriptions> m_pVertexInputDescriptions;
		std::vector<VkBuffer> m_meshBuffers;
		std::vector<VkDeviceSize> m_meshOffsets;

	public: // Methods:
		// Constructors/Destructor:
		Material(Type type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		~Material();

		// Getters:
		Type GetType() const;
		uint32_t GetRenderQueue() const;
		const vulkanBackend::VertexInputDescriptions* const GetVertexInputDescriptions() const;
		const VkBuffer* const GetMeshBuffers(Mesh* pMesh);
		const VkDeviceSize* const GetMeshOffsets(Mesh* pMesh);
	};
}



#endif // __INCLUDE_GUARD_material_h__