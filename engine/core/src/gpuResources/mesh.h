#pragma once
#include "commonMeshGeometry.h"
#include "commonSubmesh.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class IMesh;
}


//// Set zero values if vectors not set:
//if (m_normals.size() != GetVertexCount())
//	m_normals.resize(GetVertexCount(), Float3::up);
//if (m_tangents.size() != GetVertexCount())
//	m_tangents.resize(GetVertexCount(), Float3::right);
//if (m_colors.size() != GetVertexCount())
//	m_colors.resize(GetVertexCount(), Float4::white);
//if (m_uvs.size() != GetVertexCount())
//	m_uvs.resize(GetVertexCount(), Float4::zero);

namespace emberCore
{
	class EMBER_CORE_API Mesh
	{
		// Friends:
		friend class Renderer;

	private: // Members:
		std::string m_name;
		std::vector<Float3> m_positions;
		std::vector<Float3> m_normals;
		std::vector<Float3> m_tangents;
		std::vector<Float4> m_colors;
		std::vector<Float4> m_uvs;
		std::vector<Uint3> m_triangles;
		std::vector<emberCommon::Submesh> m_submeshes = { emberCommon::Submesh() };
        Bounds m_bounds;
		std::unique_ptr<emberBackendInterface::IMesh> m_pIMesh;

	public: // Methods:
		// Constructors/Destructor:
		Mesh();
		Mesh(const std::string& name);
		Mesh(emberBackendInterface::IMesh* pIMesh);
		~Mesh();

		// Non-copyable:
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		// Movable:
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other);

		// Setters:
		void SetName(const std::string& name);

		// Geometry: (validates and replaces entire mesh data)
		void SetGeometry(const emberCommon::MeshGeometry& geometry);
		void MoveGeometry(emberCommon::MeshGeometry&& geometry);

		// Optional vertex attributes: (added attributes must match the vertex count, adding an existing attribute is ignored)
		void AddNormals(const std::vector<Float3>& normals);
		void AddTangents(const std::vector<Float3>& tangents);
		void AddColors(const std::vector<Float4>& colors);
		void AddColor(const Float4& color);
		void AddUVs(const std::vector<Float4>& uvs);
		void RemoveNormals();
		void RemoveTangents();
		void RemoveColors();
		void RemoveUVs();

		// In-place vertex attribute edits: (changes are uploaded after the modifier returns)
		void ModifyPositions(const std::function<void(std::span<Float3>)>& modifier);
		void ModifyNormals(const std::function<void(std::span<Float3>)>& modifier);
		void ModifyTangents(const std::function<void(std::span<Float3>)>& modifier);
		void ModifyColors(const std::function<void(std::span<Float4>)>& modifier);
		void ModifyUVs(const std::function<void(std::span<Float4>)>& modifier);

		// Getters:
		const std::string& GetName() const;
		uint32_t GetVertexCount() const;
		uint32_t GetTriangleCount() const;
		uint32_t GetSubmeshCount() const;
		const std::vector<Float3>& GetPositions() const;
		const std::vector<Float3>& GetNormals() const;
		const std::vector<Float3>& GetTangents() const;
		const std::vector<Float4>& GetColors() const;
		const std::vector<Float4>& GetUVs() const;
		const std::vector<Uint3>& GetTriangles() const;
		const emberCommon::Submesh& GetSubmesh(uint32_t submeshIndex) const;
		const std::vector<emberCommon::Submesh>& GetSubmeshes() const;
		emberCommon::MeshGeometry GetGeometry() const;
        Bounds GetBounds() const;
		RayMeshHit IntersectRay(const Ray& localRay) const;
		Mesh GetCopy(const std::string& newName = "") const;

		// Mesh transformations (changes *this):
		Mesh& Translate(const Float3& translation) &;
		Mesh  Translate(const Float3& translation) &&;
		Mesh& Rotate(const Float3x3& rotation) &;
		Mesh  Rotate(const Float3x3& rotation) &&;
		Mesh& Rotate(const Float4x4& rotation) &;
		Mesh  Rotate(const Float4x4& rotation) &&;
		Mesh& Scale(const Float3& scale) &;
		Mesh  Scale(const Float3& scale) &&;
		Mesh& Scale(float scale) &;
		Mesh  Scale(float scale) &&;
		Mesh& Transform(const Float4x4& transformationMatrix) &;
		Mesh  Transform(const Float4x4& transformationMatrix) &&;
		Mesh& Subdivide() &;
		Mesh  Subdivide() &&;
		Mesh& Spherify(float factor, float radius) &;
		Mesh  Spherify(float factor, float radius) &&;
		Mesh& InvertFaces() &;
		Mesh  InvertFaces() &&;
		Mesh& RescaleUVs(const Float4& scale, const Float4& offset) &;
		Mesh  RescaleUVs(const Float4& scale, const Float4& offset) &&;
		Mesh& Mirror(const Float3& planePosition, const Float3& planeNormal) &;
		Mesh  Mirror(const Float3& planePosition, const Float3& planeNormal) &&;
		void ComputeNormals();
		void ComputeTangents();

		// Mesh operations:
		// Submeshes of all meshes are kept, in order, so each stays addressable in the merged mesh.
		static Mesh Merge(const std::vector<Mesh>& meshes, const std::string& name = "");

	private: // Methods:
		// Backend access:
		emberBackendInterface::IMesh* const GetInterfaceHandle() const;
		// Validation:
		static void ValidateGeometry(const emberCommon::MeshGeometry& geometry);
		static void ValidateAttributeCount(size_t attributeCount, size_t vertexCount, const std::string& attributeName);
		// GPU upload/Bounds:
		void RegisterUpdate();
		void UpdateBounds();
		// Mesh modifiers:
		void ApplyTranslate(const Float3& translation);
		void ApplyRotate(const Float3x3& rotation);
		void ApplyRotate(const Float4x4& rotation);
		void ApplyScale(const Float3& scale);
		void ApplyScale(float scale);
		void ApplyTransform(const Float4x4& transformationMatrix);
		void ApplySubdivide();
		void ApplySpherify(float factor, float radius);
		void ApplyInvertFaces();
		void ApplyRescaleUVs(const Float4& scale, const Float4& offset);
		void ApplyMirror(const Float3& planePosition, const Float3& planeNormal);
	};
}