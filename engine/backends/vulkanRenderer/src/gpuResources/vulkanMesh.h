#pragma once
#include "emberMath.h"
#include "iMesh.h"
#include "vulkanIndexType.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <vector>



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
		enum class MeshType
		{
			static,
			dynamic
		};
		enum class IndexType
		{
			uint16,
			uint32
		};
		enum class MemoryLayout
		{
			interleaved,
			separate
		};
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
		struct PendingVertexUpload
		{
			uint64_t generation;
			uint32_t remainingFramesMask;	// one bit per frame in flight.
			StagingBuffer stagingBuffer;
			uint64_t lastSignalValue;		// for semaphore synchronization.
		};

	private: // Members:
		std::string m_name;
		MeshType m_meshType = MeshType::static;
		IndexType m_indexType = IndexType::uint32;
		MemoryLayout m_memoryLayout = MemoryLayout::interleaved;
		std::vector<std::unique_ptr<VertexBuffer>> m_pVertexBuffers;
		std::vector<std::unique_ptr<IndexBuffer>> m_pIndexBuffers;

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
		void SetMeshType(MeshType type);
		void SetIndexType(IndexType type) override;
		void SetMemoryLayout(MemoryLayout layout) override;

		// Getters:
		const std::string& GetName() const override;
		MeshType GetMeshType() const;
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