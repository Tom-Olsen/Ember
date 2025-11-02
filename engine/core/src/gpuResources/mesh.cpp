#include "mesh.h"
#include "iMesh.h"
#include "renderer.h"



namespace emberEngine
{
	// Private methods:
	emberBackendInterface::IMesh* const Mesh::GetInterfaceHandle() const
	{
		return m_pIMesh.get();
	}



	// Public methods:
	// Constructors/Destructor:
	Mesh::Mesh()
	{
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(Renderer::CreateMesh(""));
	}
	Mesh::Mesh(const std::string& name)
	{
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(Renderer::CreateMesh(name));
	}
	Mesh::Mesh(emberBackendInterface::IMesh* pIMesh)
	{
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(pIMesh);
	}
	Mesh::~Mesh()
	{

	}


	// Movable:
	Mesh::Mesh(Mesh&& other) noexcept = default;
	Mesh& Mesh::operator=(Mesh&& other) noexcept = default;


	// Setters: (copy the vector)
	void Mesh::SetName(const std::string& name)
	{
		m_pIMesh->SetName(name);
	}
	void Mesh::SetPositions(const std::vector<Float3>& positions)
	{
		m_pIMesh->SetPositions(positions);
	}
	void Mesh::SetNormals(const std::vector<Float3>& normals)
	{
		m_pIMesh->SetNormals(normals);
	}
	void Mesh::SetTangents(const std::vector<Float3>& tangents)
	{
		m_pIMesh->SetTangents(tangents);
	}
	void Mesh::SetColors(const std::vector<Float4>& colors)
	{
		m_pIMesh->SetColors(colors);
	}
	void Mesh::SetUniformColor(const Float4& color)
	{
		m_pIMesh->SetUniformColor(color);
	}
	void Mesh::SetUVs(const std::vector<Float4>& uvs)
	{
		m_pIMesh->SetUVs(uvs);
	}
	void Mesh::SetTriangles(const std::vector<Uint3>& triangles)
	{
		m_pIMesh->SetTriangles(triangles);
	}



	// Movers: (take ownership of vector)
	void Mesh::MovePositions(std::vector<Float3>&& positions)
	{
		m_pIMesh->MovePositions(std::move(positions));
	}
	void Mesh::MoveNormals(std::vector<Float3>&& normals)
	{
		m_pIMesh->MoveNormals(std::move(normals));
	}
	void Mesh::MoveTangents(std::vector<Float3>&& tangents)
	{
		m_pIMesh->MoveTangents(std::move(tangents));
	}
	void Mesh::MoveColors(std::vector<Float4>&& colors)
	{
		m_pIMesh->MoveColors(std::move(colors));
	}
	void Mesh::MoveUVs(std::vector<Float4>&& uvs)
	{
		m_pIMesh->MoveUVs(std::move(uvs));
	}
	void Mesh::MoveTriangles(std::vector<Uint3>&& triangles)
	{
		m_pIMesh->MoveTriangles(std::move(triangles));
	}
	void Mesh::MoveMeshAsset(emberAssetLoader::Mesh&& meshAsset)
	{
		MovePositions(std::move(meshAsset.positions));
		MoveNormals(std::move(meshAsset.normals));
		MoveTangents(std::move(meshAsset.tangents));
		MoveColors(std::move(meshAsset.colors));
		MoveUVs(std::move(meshAsset.uvs));
		MoveTriangles(std::move(meshAsset.triangles));
	}



	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_pIMesh->GetName();
	}
	uint32_t Mesh::GetVertexCount() const
	{
		return m_pIMesh->GetVertexCount();
	}
	uint32_t Mesh::GetTriangleCount() const
	{
		return m_pIMesh->GetTriangleCount();
	}
	std::vector<Float3>& Mesh::GetPositions()
	{
		return m_pIMesh->GetPositions();
	}
	std::vector<Float3>& Mesh::GetNormals()
	{
		return m_pIMesh->GetNormals();
	}
	std::vector<Float3>& Mesh::GetTangents()
	{
		return m_pIMesh->GetTangents();
	}
	std::vector<Float4>& Mesh::GetColors()
	{
		return m_pIMesh->GetColors();
	}
	std::vector<Float4>& Mesh::GetUVs()
	{
		return m_pIMesh->GetUVs();
	}
	std::vector<Uint3>& Mesh::GetTriangles()
	{
		return m_pIMesh->GetTriangles();
	}
	void Mesh::RegisterUpdate()
	{
		m_pIMesh->RegisterUpdate();
	}
	Mesh Mesh::GetCopy(const std::string& newName)
	{
		return Mesh(m_pIMesh->GetCopy(newName));
	}



	// Mesh transformations (changes *this):
	Mesh& Mesh::Translate(const Float3& translation)&
	{
		ApplyTranslate(translation);
		return *this;
	}
	Mesh  Mesh::Translate(const Float3& translation)&&
	{
		ApplyTranslate(translation);
		return std::move(*this);
	}
	Mesh& Mesh::Rotate(const Float3x3& rotation)&
	{
		ApplyRotate(rotation);
		return *this;
	}
	Mesh  Mesh::Rotate(const Float3x3& rotation)&&
	{
		ApplyRotate(rotation);
		return std::move(*this);
	}
	Mesh& Mesh::Rotate(const Float4x4& rotation)&
	{
		ApplyRotate(rotation);
		return *this;
	}
	Mesh  Mesh::Rotate(const Float4x4& rotation)&&
	{
		ApplyRotate(rotation);
		return std::move(*this);
	}
	Mesh& Mesh::Scale(const Float3& scale)&
	{
		ApplyScale(scale);
		return *this;
	}
	Mesh  Mesh::Scale(const Float3& scale)&&
	{
		ApplyScale(scale);
		return std::move(*this);
	}
	Mesh& Mesh::Scale(float scale)&
	{
		ApplyScale(scale);
		return *this;
	}
	Mesh  Mesh::Scale(float scale)&&
	{
		ApplyScale(scale);
		return std::move(*this);
	}
	Mesh& Mesh::Transform(const Float4x4& transformationMatrix)&
	{
		ApplyTransform(transformationMatrix);
		return *this;
	}
	Mesh  Mesh::Transform(const Float4x4& transformationMatrix)&&
	{
		ApplyTransform(transformationMatrix);
		return std::move(*this);
	}
	Mesh& Mesh::Subdivide()&
	{
		ApplySubdivide();
		return *this;
	}
	Mesh  Mesh::Subdivide()&&
	{
		ApplySubdivide();
		return std::move(*this);
	}
	Mesh& Mesh::Spherify(float factor, float radius)&
	{
		ApplySpherify(factor, radius);
		return *this;
	}
	Mesh  Mesh::Spherify(float factor, float radius)&&
	{
		ApplySpherify(factor, radius);
		return std::move(*this);
	}
	Mesh& Mesh::InvertFaces()&
	{
		ApplyInvertFaces();
		return *this;
	}
	Mesh  Mesh::InvertFaces()&&
	{
		ApplyInvertFaces();
		return std::move(*this);
	}
	Mesh& Mesh::RescaleUVs(const Float4& scale, const Float4& offset)&
	{
		ApplyRescaleUVs(scale, offset);
		return *this;
	}
	Mesh  Mesh::RescaleUVs(const Float4& scale, const Float4& offset)&&
	{
		ApplyRescaleUVs(scale, offset);
		return std::move(*this);
	}
	void Mesh::ComputeNormals()
	{
		m_pIMesh->ComputeNormals();
	}
	void Mesh::ComputeTangents()
	{
		m_pIMesh->ComputeTangents();
	}



	// Mesh operations:
	Mesh Mesh::Merge(std::vector<Mesh>& meshes, const std::string& name)
	{
		// Get total vertex and triangle counts:
		uint32_t vertexCount = 0;
		uint32_t triangleCount = 0;
		for (const Mesh& mesh : meshes)
		{
			vertexCount += mesh.GetVertexCount();
			triangleCount += mesh.GetTriangleCount();
		}

		// Triangle reindexing:
		uint32_t index = 0;
		uint32_t vertCount = 0;

		// Prepare merged data:
		std::vector<Float3> mergedPositions;
		std::vector<Float3> mergedNormals;
		std::vector<Float3> mergedTangents;
		std::vector<Float4> mergedColors;
		std::vector<Float4> mergedUvs;
		std::vector<Uint3> mergedTriangles;
		mergedPositions.reserve(vertexCount);
		mergedNormals.reserve(vertexCount);
		mergedTangents.reserve(vertexCount);
		mergedColors.reserve(vertexCount);
		mergedUvs.reserve(vertexCount);
		mergedTriangles.reserve(triangleCount);

		for (Mesh& mesh : meshes)
		{
			// Get data of current mesh:

			// Append positions (must always be present).
			std::vector<Float3>& positions = mesh.GetPositions();
			mergedPositions.insert(mergedPositions.end(), positions.begin(), positions.end());

			// Append normals:
			std::vector<Float3>& normals = mesh.GetNormals();
			mergedNormals.insert(mergedNormals.end(), normals.begin(), normals.end());


			// Append tangents:
			std::vector<Float3>& m_tangents = mesh.GetTangents();
			mergedTangents.insert(mergedTangents.end(), m_tangents.begin(), m_tangents.end());

			// Append colors:
			std::vector<Float4>& colors = mesh.GetColors();
			mergedColors.insert(mergedColors.end(), colors.begin(), colors.end());

			// Append uvs:
			std::vector<Float4>& uvs = mesh.GetUVs();
			mergedUvs.insert(mergedUvs.end(), uvs.begin(), uvs.end());

			// Handle triangles:
			std::vector<Uint3> triangles = mesh.GetTriangles();
			for (int i = 0; i < triangles.size(); i++)
			{
				mergedTriangles.push_back(triangles[i] + Uint3(vertCount));
				index++;
			}
			vertCount += mesh.GetVertexCount();
		}

		// Construct mergedMesh:
		Mesh mergedMesh(name);
		mergedMesh.MovePositions(std::move(mergedPositions));
		mergedMesh.MoveNormals(std::move(mergedNormals));
		mergedMesh.MoveTangents(std::move(mergedTangents));
		mergedMesh.MoveColors(std::move(mergedColors));
		mergedMesh.MoveUVs(std::move(mergedUvs));
		mergedMesh.MoveTriangles(std::move(mergedTriangles));
		return mergedMesh;
	}



	// Private methods:
	// Mesh modifiers:
	void Mesh::ApplyTranslate(const Float3& translation)
	{
		std::vector<Float3>& positions = GetPositions();
		for (Float3& position : positions)
			position += translation;
		RegisterUpdate();
	}
	void Mesh::ApplyRotate(const Float3x3& rotation)
	{
		uint32_t vertexCount = GetVertexCount();

		// Positions:
		std::vector<Float3>& positions = GetPositions();
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = rotation * positions[i];

		// Normals:
		std::vector<Float3>& normals = GetNormals();
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = rotation * normals[i];

		// Tangents:
		std::vector<Float3>& tangents = GetTangents();
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = rotation * tangents[i];

		RegisterUpdate();
	}
	void Mesh::ApplyRotate(const Float4x4& rotation)
	{
		uint32_t vertexCount = GetVertexCount();
		Float3x3 rotation3x3 = Float3x3(rotation);

		// Positions:
		std::vector<Float3>& positions = GetPositions();
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = rotation3x3 * positions[i];

		// Normals:
		std::vector<Float3>& normals = GetNormals();
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = rotation3x3 * normals[i];

		// Tangents:
		std::vector<Float3>& tangents = GetTangents();
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = rotation3x3 * tangents[i];

		RegisterUpdate();
	}
	void Mesh::ApplyScale(const Float3& scale)
	{
		uint32_t vertexCount = GetVertexCount();
		Float3 invScale = 1.0 / scale;

		// Positions:
		std::vector<Float3>& positions = GetPositions();
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = scale * positions[i];

		// Normals:
		std::vector<Float3>& normals = GetNormals();
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = (invScale * normals[i]).Normalize();

		// Tangents:
		std::vector<Float3>& tangents = GetTangents();
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = (invScale * tangents[i]).Normalize();

		RegisterUpdate();
	}
	void Mesh::ApplyScale(float scale)
	{
		Scale(Float3(scale));
	}
	void Mesh::ApplyTransform(const Float4x4& transformationMatrix)
	{
		uint32_t vertexCount = GetVertexCount();
		Float4x4 normalMatrix = transformationMatrix.Inverse().Transpose();

		// Positions:
		std::vector<Float3>& positions = GetPositions();
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = Float3(transformationMatrix * Float4(positions[i], 1.0f));

		// Normals:
		std::vector<Float3>& normals = GetNormals();
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = Float3(normalMatrix * Float4(normals[i], 0.0f)).Normalize();

		// Tangents:
		std::vector<Float3>& tangents = GetTangents();
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = Float3(normalMatrix * Float4(tangents[i], 0.0f)).Normalize();

		RegisterUpdate();
	}
	void Mesh::ApplySubdivide()
	{
		std::vector<Uint3> newTriangles;
		newTriangles.reserve(4 * GetTriangleCount());

		std::vector<Float3>& positions = GetPositions();
		std::vector<Float3>& normals = GetNormals();
		std::vector<Float3>& tangents = GetTangents();
		std::vector<Float4>& colors = GetColors();
		std::vector<Float4>& uvs = GetUVs();
		std::vector<Uint3>& triangles = GetTriangles();

		// Subdivide each triangle:
		uint32_t newVertexIndex = GetVertexCount();
		for (uint32_t i = 0; i < GetTriangleCount(); i++)
		{
			Uint3 triangle = triangles[i];

			// Add positions:
			Float3 positionA = 0.5f * (positions[triangle[0]] + positions[triangle[1]]);
			Float3 positionB = 0.5f * (positions[triangle[1]] + positions[triangle[2]]);
			Float3 positionC = 0.5f * (positions[triangle[2]] + positions[triangle[0]]);
			positions.push_back(positionA);
			positions.push_back(positionB);
			positions.push_back(positionC);

			// Add normals:
			Float3 normalA = (normals[triangle[0]] + normals[triangle[1]]).Normalize();
			Float3 normalB = (normals[triangle[1]] + normals[triangle[2]]).Normalize();
			Float3 normalC = (normals[triangle[2]] + normals[triangle[0]]).Normalize();
			normals.push_back(normalA);
			normals.push_back(normalB);
			normals.push_back(normalC);

			// Add tangents:
			Float3 tangentA = (tangents[triangle[0]] + tangents[triangle[1]]).Normalize();
			Float3 tangentB = (tangents[triangle[1]] + tangents[triangle[2]]).Normalize();
			Float3 tangentC = (tangents[triangle[2]] + tangents[triangle[0]]).Normalize();
			tangents.push_back(tangentA);
			tangents.push_back(tangentB);
			tangents.push_back(tangentC);

			// Add colors:
			Float4 colorA = 0.5f * (colors[triangle[0]] + colors[triangle[1]]);
			Float4 colorB = 0.5f * (colors[triangle[1]] + colors[triangle[2]]);
			Float4 colorC = 0.5f * (colors[triangle[2]] + colors[triangle[0]]);
			colors.push_back(colorA);
			colors.push_back(colorB);
			colors.push_back(colorC);

			// Add Uvs:
			Float4 uvA = 0.5f * (uvs[triangle[0]] + uvs[triangle[1]]);
			Float4 uvB = 0.5f * (uvs[triangle[1]] + uvs[triangle[2]]);
			Float4 uvC = 0.5f * (uvs[triangle[2]] + uvs[triangle[0]]);
			uvs.push_back(uvA);
			uvs.push_back(uvB);
			uvs.push_back(uvC);

			// Add 4 triangles:
			Uint3 newTriangleA = { triangle[0], newVertexIndex, newVertexIndex + 2 };
			Uint3 newTriangleB = { newVertexIndex, newVertexIndex + 1, newVertexIndex + 2 };
			Uint3 newTriangleC = { newVertexIndex, triangle[1], newVertexIndex + 1 };
			Uint3 newTriangleD = { newVertexIndex + 2, newVertexIndex + 1, triangle[2] };
			newTriangles.push_back(newTriangleA);
			newTriangles.push_back(newTriangleB);
			newTriangles.push_back(newTriangleC);
			newTriangles.push_back(newTriangleD);

			newVertexIndex += 3;
		}

		// Update mesh data:
		RegisterUpdate();
		MoveTriangles(std::move(newTriangles));
	}
	void Mesh::ApplySpherify(float factor, float radius)
	{
		factor = std::clamp(factor, 0.0f, 1.0f);
		radius = std::max(1e-8f, radius);

		std::vector<Float3>& positions = GetPositions();
		std::vector<Float3>& normals = GetNormals();
		std::vector<Float3>& tangents = GetTangents();

		for (uint32_t i = 0; i < GetVertexCount(); i++)
		{
			Float3 sphereNormal = positions[i].Normalize();
			positions[i] = radius * (positions[i] + factor * (sphereNormal - positions[i]));
			normals[i] = (normals[i] + factor * (sphereNormal - normals[i])).Normalize();
			tangents[i] = geometry3d::PointToPlaneProjection(tangents[i], Float3::zero, normals[i]).Normalize();
		}

		RegisterUpdate();
	}
	void Mesh::ApplyInvertFaces()
	{
		uint32_t vertexCount = GetVertexCount();

		// Triangles:
		std::vector<Uint3>& triangles = GetTriangles();
		for (Uint3& triangle : triangles)
			std::swap(triangle[1], triangle[2]);

		// Normals:
		std::vector<Float3>& normals = GetNormals();
		if (normals.size() == vertexCount)
			for (Float3& normal : normals)
				normal = -normal;

		// Tangents:
		std::vector<Float3>& tangents = GetTangents();
		if (tangents.size() == vertexCount)
			for (Float3& tangent : tangents)
				tangent = -tangent;

		RegisterUpdate();
	}
	void Mesh::ApplyRescaleUVs(const Float4& scale, const Float4& offset)
	{
		std::vector<Float4>& uvs = GetUVs();
		for (Float4& uv : uvs)
			uv = scale * uv + offset;
		RegisterUpdate();
	}
}