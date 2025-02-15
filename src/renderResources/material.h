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
	struct VertexInputDescriptions;
	struct VulkanContext;



	/// <summary>
	/// Material creation is expensive.
	/// It is recommended to create all materials at the start of the application.
	/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
	/// Each Material must be used with a ShaderProperties which is customized for the Material.
	/// </summary>
	class Material : public Shader
	{
	public: // Enums:
		enum class Type
		{
			shadow,
			shading,
			skybox
		};
		enum class RenderQueue
		{
			shadow = 0,
			opaque = 1000,
			transparent = 2000,
			skybox = 3000
		};

	private: // Members:
		Type m_type;
		RenderQueue m_renderQueue;
		std::unique_ptr<VertexInputDescriptions> m_pVertexInputDescriptions;
		std::vector<VkBuffer> m_meshBuffers;
		std::vector<VkDeviceSize> m_meshOffsets;

	public: // Methods:
		// Constructors/Destructor:
		Material(VulkanContext* pContext, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		~Material();

		// Getters:
		Type GetType() const;
		RenderQueue GetRenderQueue() const;
		const VertexInputDescriptions* const GetVertexInputDescriptions() const;
		const VkBuffer* const GetMeshBuffers(Mesh* pMesh);
		const VkDeviceSize* const GetMeshOffsets(Mesh* pMesh);
	};
}



#endif // __INCLUDE_GUARD_material_h__