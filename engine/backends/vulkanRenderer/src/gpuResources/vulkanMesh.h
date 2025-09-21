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
	/// Mesh class for storing vertex positions, normals, tangents, colors, uvs, and triangles.
	/// Mesh class takes ownership of vectors and takes care of cleanup.
	/// Normalization of normals and tangets must be done manually if they are not computed with ComputeNormals()/ComputeTangents().
	/// Mesh class does not guarantee validity of the mesh data.
	/// </summary>
	class VULKAN_RENDERER_API Mesh : public emberBackendInterface::IMesh
	{
	private: // Members:
		std::string m_name;
		bool m_isLoaded = false;
		bool m_verticesUpdated = false;
		bool m_indicesUpdated = false;
		uint32_t m_vertexCount = 0;
		uint32_t m_triangleCount = 0;
		std::unique_ptr<VertexBuffer> m_pVertexBuffer;
		std::unique_ptr<IndexBuffer> m_pIndexBuffer;
		std::vector<Float3> m_positions;
		std::vector<Float3> m_normals;
		std::vector<Float3> m_tangents;
		std::vector<Float4> m_colors;
		std::vector<Float4> m_uvs;
		std::vector<Uint3> m_triangles;

	public: // Constructors/Destructor:
		Mesh(const std::string& name = "");
		~Mesh();

	public: // Non-copyable:
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;

	public: // Movable:
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

	public: // Setters:
		void SetName(const std::string& name) override;
		void SetPositions(const std::vector<Float3>& positions) override;
		void SetNormals(const std::vector<Float3>& normals) override;
		void SetTangents(const std::vector<Float3>& tangents) override;
		void SetColors(const std::vector<Float4>& colors) override;
		void SetUniformColor(const Float4& color) override;
		void SetUVs(const std::vector<Float4>& uvs) override;
		void SetTriangles(const std::vector<Uint3>& triangles) override;

	public: // Movers:
		void MovePositions(std::vector<Float3>& positions) override;
		void MoveNormals(std::vector<Float3>& normals) override;
		void MoveTangents(std::vector<Float3>& tangents) override;
		void MoveColors(std::vector<Float4>& colors) override;
		void MoveUVs(std::vector<Float4>& uvs) override;
		void MoveTriangles(std::vector<Uint3>& triangles) override;

	public: // Getters:
		const std::string& GetName() const override;
		uint32_t GetVertexCount() const override;
		uint32_t GetTriangleCount() const override;
		std::vector<Float3>& GetPositions() override;
		std::vector<Float3>& GetNormals() override;
		std::vector<Float3>& GetTangents() override;
		std::vector<Float4>& GetColors() override;
		std::vector<Float4>& GetUVs() override;
		std::vector<Uint3>& GetTriangles() override;

	public: // Mesh transformations (changes *this):
		void ComputeNormals() override;
		void ComputeTangents() override;

	public: // Backend getters:
		uint32_t* GetTrianglesUnrolled();
		uint32_t GetSizeOfPositions() const;
		uint32_t GetSizeOfNormals() const;
		uint32_t GetSizeOfTangents() const;
		uint32_t GetSizeOfColors() const;
		uint32_t GetSizeOfUVs() const;
		uint32_t GetVertexBufferElementSize() const;
		uint64_t GetVertexBufferSize() const;
		uint64_t GetSizeOfTriangles() const;
		VertexBuffer* GetVertexBuffer();
		IndexBuffer* GetIndexBuffer();
		uint64_t GetPositionsOffset() const;
		uint64_t GetNormalsOffset() const;
		uint64_t GetTangentsOffset() const;
		uint64_t GetColorsOffset() const;
		uint64_t GetUVsOffset() const;
		static IndexType GetIndexType();
		bool IsLoaded();

	private: // Methods:
		void Load();
		void Unload();
		void UpdateVertexBuffer();
		void UpdateIndexBuffer();
	};
}