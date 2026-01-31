#pragma once
#include "emberMath.h"
#include "iMesh.h"
#include "vulkanIndexType.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class IndexBuffer;
	class VertexBuffer;



	/// <summary>
	/// Mesh class does not guarantee validity of the mesh data. It only provides GPU resources for rendering.
	/// </summary>
	class VULKAN_RENDERER_API Mesh : public emberBackendInterface::IMesh
	{
	public: // Enums/Structs:
		//enum class MeshType
		//{
		//	staticMesh,
		//	dynamicMesh
		//};
		//enum class MemoryLayout
		//{
		//	interleaved,
		//	separate
		//};
		struct Vertex
		{
			Float3 position;
			Float3 normal;
			Float3 tangent;
			Float4 color;
			Float4 uv;
		};
		//enum UpdateFlagsBits : uint32_t
		//{
		//	none = 0,
		//	positions = 1 << 0,
		//	normals = 1 << 1,
		//	tangents = 1 << 2,
		//	colors = 1 << 3,
		//	uvs = 1 << 4,
		//	triangles = 1 << 5,
		//	vertices = positions | normals | tangents | colors | uvs,
		//	all = positions | normals | tangents | colors | uvs | triangles
		//};
		//typedef uint32_t UpdateFlags;

	private: // Structs:
		template<typename BufferType>
		struct BufferPair
		{
			std::unique_ptr<BufferType> pBufferA;
			std::unique_ptr<BufferType> pBufferB;
			bool inUseA = false;
			bool inUseB = false;
    		uint64_t generationA = 0;
    		uint64_t generationB = 0;
			BufferType* GetActiveBuffer()
			{
				if (generationA > generationB)
				{
					inUseA = true;
					inUseB = false;
					return pBufferA.get();
				}
				else
				{
					inUseA = false;
					inUseB = true;
					return pBufferB.get();
				}
			}
		}

	private: // Members:
		std::string m_name;
		//MeshType m_meshType = MeshType::static;	// Ember::ToDo: implement static meshes.
		VkIndexType m_vkIndexType = VK_INDEX_TYPE_UINT16;
		//MemoryLayout m_memoryLayout = MemoryLayout::interleaved; // Ember::ToDo: implement interleaved meshes.
		std::vector<std::unique_ptr<BufferPair>> m_pVertexBuffers;
		std::vector<std::unique_ptr<BufferPair>> m_pIndexBuffers;

	public: // Methods:
		// Constructors/Destructor:
		Mesh(const std::string& name = "");
		~Mesh();

		// Non-copyable:
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;

		// Movable:
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

		// Setters:
		void SetName(const std::string& name) override;
		//void SetMeshType(MeshType type);
		void SetMemoryLayout(MemoryLayout layout) override;

		// Getters:
		const std::string& GetName() const override;
		//MeshType GetMeshType() const;
		IndexType GetIndexType() const override;
		MemoryLayout GetMemoryLayout() const override;
		emberBackendInterface::IMesh* GetCopy(const std::string& newName) override;

		// Update GPU buffers:
		void UpdateVertexBuffer(std::vector<Float3>* positions, std::vector<Float3>* normals, std::vector<Float3>* tangents, std::vector<Float4>* colors, std::vector<Float4>* uvs) override;
		void UpdateIndexBuffer(std::vector<Uint3>* triangles) override;

		// Backend only:
		VertexBuffer* GetVertexBuffer();
		IndexBuffer* GetIndexBuffer();
	};
}