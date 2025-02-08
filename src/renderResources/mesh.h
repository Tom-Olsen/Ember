#ifndef __INCLUDE_GUARD_mesh_h__
#define __INCLUDE_GUARD_mesh_h__
#include "mathf.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class VmaBuffer;
	struct VulkanContext;



	/// <summary>
	/// Mesh class for storing vertex positions, normals, tangents, colors, uvs, and triangles.
	/// Mesh class takes ownership of vectors and takes care of cleanup.
	/// Normalization of normals and tangets must be done manually if they are not computed with ComputeNormals()/ComputeTangents().
	/// Mesh class does not guarantee validity of the mesh data.
	/// Static meshes should be stored as pointers in the static MeshManager class.
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
		std::unique_ptr<VmaBuffer> m_vertexBuffer;
		std::unique_ptr<VmaBuffer> m_indexBuffer;
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
		void Load(VulkanContext* pContext);
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
		std::string GetName() const;
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
		uint64_t GetVertexBufferSize() const;
		uint64_t GetSizeOfTriangles() const;
		VkDeviceSize GetPositionsOffset() const;
		VkDeviceSize GetNormalsOffset() const;
		VkDeviceSize GetTangentsOffset() const;
		VkDeviceSize GetColorsOffset() const;
		VkDeviceSize GetUVsOffset() const;
		VmaBuffer* GetVertexBuffer(VulkanContext* pContext);
		VmaBuffer* GetIndexBuffer(VulkanContext* pContext);
		bool IsLoaded();
		Mesh* GetCopy(const std::string& newName = "");

		// Mesh transformations (changes *this!):
		Mesh* Translate(const Float3& translation);
		Mesh* Rotate(const Float3x3& rotation);
		Mesh* Rotate(const Float4x4& rotation);
		Mesh* Scale(const Float3& scale);
		Mesh* Scale(float scale);
		Mesh* Transform(const Float4x4& transformationMatrix);
		Mesh* Subdivide();
		Mesh* Spherify(float factor, float radius);
		Mesh* InvertFaces();
		Mesh* RescaleUVs(const Float4& scale, const Float4& offset);
		void ComputeNormals();
		void ComputeTangents();

		// Static methods:
		static VkIndexType GetIndexType();
		static Mesh* Merge(std::vector<Mesh*>& meshes, const std::string& name = "");

		// Debugging:
		std::string ToString();

	private: // Methods:
		void UpdateVertexBuffer(VulkanContext* pContext);
		void UpdateIndexBuffer(VulkanContext* pContext);
	};
}



#endif // __INCLUDE_GUARD_mesh_h__