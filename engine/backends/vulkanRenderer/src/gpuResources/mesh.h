#ifndef __INCLUDE_GUARD_vulkanRendererBackend_mesh_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_mesh_h__
#include "emberMath.h"
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
	/// Mesh class for storing vertex positions, normals, tangents, colors, uvs, and triangles.
	/// Mesh class takes ownership of vectors and takes care of cleanup.
	/// Normalization of normals and tangets must be done manually if they are not computed with ComputeNormals()/ComputeTangents().
	/// Mesh class does not guarantee validity of the mesh data.
	/// </summary>
	class Mesh
	{
	private: // Members:
		std::string m_name;
		bool m_isLoaded = false;
		bool m_verticesUpdated = false;
		bool m_indicesUpdated = false;
		uint32_t m_vertexCount = 0;
		uint32_t m_triangleCount = 0;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<IndexBuffer> m_indexBuffer;
		std::vector<Float3> m_positions;
		std::vector<Float3> m_normals;
		std::vector<Float3> m_tangents;
		std::vector<Float4> m_colors;
		std::vector<Float4> m_uvs;
		std::vector<Uint3> m_triangles;

	public: // Methods:
		Mesh(const std::string& name = "");
		Mesh(const Mesh& other) = default;
		Mesh& operator=(const Mesh& other) = default;
		Mesh(Mesh&& other) noexcept = default;
		Mesh& operator=(Mesh&& other) noexcept = default;
		~Mesh();

		// Load/Unload:
		void Load();
		void Unload();

		// Setters:
		void SetName(const std::string& name);
		void SetPositions(const std::vector<Float3>& positions);
		void SetNormals(const std::vector<Float3>& normals);
		void SetTangents(const std::vector<Float3>& tangents);
		void SetColors(const std::vector<Float4>& colors);
		void SetUniformColor(const Float4& color);
		void SetUVs(const std::vector<Float4>& uvs);
		void SetTriangles(const std::vector<Uint3>& triangles);

		// Movers:
		void MovePositions(std::vector<Float3>& positions);
		void MoveNormals(std::vector<Float3>& normals);
		void MoveTangents(std::vector<Float3>& tangents);
		void MoveColors(std::vector<Float4>& colors);
		void MoveUVs(std::vector<Float4>& uvs);
		void MoveTriangles(std::vector<Uint3>& triangles);

		// Getters:
		const std::string& GetName() const;
		uint32_t GetVertexCount() const;
		uint32_t GetTriangleCount() const;
		std::vector<Float3>& GetPositions();
		std::vector<Float3>& GetNormals();
		std::vector<Float3>& GetTangents();
		std::vector<Float4>& GetColors();
		std::vector<Float4>& GetUVs();
		std::vector<Uint3>& GetTriangles();
		uint32_t* GetTrianglesUnrolled();
		uint32_t GetSizeOfPositions() const;
		uint32_t GetSizeOfNormals() const;
		uint32_t GetSizeOfTangents() const;
		uint32_t GetSizeOfColors() const;
		uint32_t GetSizeOfUVs() const;
		uint32_t GetVertexBufferElementSize() const;
		uint64_t GetVertexBufferSize() const;
		uint64_t GetSizeOfTriangles() const;
		VkDeviceSize GetPositionsOffset() const;
		VkDeviceSize GetNormalsOffset() const;
		VkDeviceSize GetTangentsOffset() const;
		VkDeviceSize GetColorsOffset() const;
		VkDeviceSize GetUVsOffset() const;
		VertexBuffer* GetVertexBuffer();
		IndexBuffer* GetIndexBuffer();
		bool IsLoaded();
		Mesh* GetCopy(const std::string& newName = "");
		static VkIndexType GetIndexType();

	private: // Methods:
		void UpdateVertexBuffer();
		void UpdateIndexBuffer();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_mesh_h__