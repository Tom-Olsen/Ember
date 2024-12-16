#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



struct DescriptorBoundResources;
class Mesh;
class Pipeline;
struct UniformBufferBlock;
struct VertexInputDescriptions;
struct VulkanContext;



/// <summary>
/// Material creation is expensive.
/// It is recommended to create all materials at the start of the application.
/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
/// Each material must be used with a MaterialProperties which is customized for the Material.
/// </summary>
class Material
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
	std::string m_name;
	RenderQueue m_renderQueue;
	std::unique_ptr<Pipeline> m_pPipeline;
	std::unique_ptr<DescriptorBoundResources> m_pDescriptorBoundResources;
	std::unique_ptr<VertexInputDescriptions> m_pVertexInputDescriptions;
	std::vector<VkBuffer> m_meshBuffers;
	std::vector<VkDeviceSize> m_meshOffsets;
	VulkanContext* m_pContext;

public: // Methods:
	// Constructors/Destructor:
	Material(VulkanContext* pContext, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
	~Material();

	// Getters:
	Type GetType() const;
	const std::string& GetName() const;
	RenderQueue GetRenderQueue() const;
	const Pipeline* const GetPipeline() const;
	const DescriptorBoundResources* const GetDescriptorBoundResources() const;
	const VertexInputDescriptions* const GetVertexInputDescriptions() const;
	const VkBuffer* const GetMeshBuffers(Mesh* pMesh);
	const VkDeviceSize* const GetMeshOffsets(Mesh* pMesh);
	VulkanContext* const GetContext() const;

	// Debugging:
	void PrintBindings() const;
	void PrintUniformBuffers() const;

private: // Methods:
	static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
};



#endif // __INCLUDE_GUARD_material_h__