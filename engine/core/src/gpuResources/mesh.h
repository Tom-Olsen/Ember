#pragma once
#include "assetLoader.h"
#include "emberMath.h"
#include <memory>
#include <string>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class IMesh;
}



namespace emberEngine
{
	class Mesh
	{
		// Friends:
		friend class Renderer;

	private: // Members:
		std::unique_ptr<emberBackendInterface::IMesh> m_pIMesh;
		emberBackendInterface::IMesh* GetInterfaceHandle();

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
		Mesh& operator=(Mesh&& other) noexcept;

		// Setters: (copy the vector)
		void SetName(const std::string& name);
		void SetPositions(const std::vector<Float3>& positions);
		void SetNormals(const std::vector<Float3>& normals);
		void SetTangents(const std::vector<Float3>& tangents);
		void SetColors(const std::vector<Float4>& colors);
		void SetUniformColor(const Float4& color);
		void SetUVs(const std::vector<Float4>& uvs);
		void SetTriangles(const std::vector<Uint3>& triangles);

		// Movers: (take ownership of vector)
		void MovePositions(std::vector<Float3>&& positions);
		void MoveNormals(std::vector<Float3>&& normals);
		void MoveTangents(std::vector<Float3>&& tangents);
		void MoveColors(std::vector<Float4>&& colors);
		void MoveUVs(std::vector<Float4>&& uvs);
		void MoveTriangles(std::vector<Uint3>&& triangles);
		void MoveMeshAsset(emberAssetLoader::Mesh&& meshAsset);

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
		void RegisterUpdate();
		Mesh GetCopy(const std::string& newName = "");

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
		void ComputeNormals();
		void ComputeTangents();

		// Mesh operations:
		static Mesh Merge(std::vector<Mesh>& meshes, const std::string& name = "");

	private: // Methods:
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
	};
}