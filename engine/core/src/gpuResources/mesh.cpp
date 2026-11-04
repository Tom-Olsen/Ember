#include "mesh.h"
#include "gpuResourceFactory.h"
#include "iMesh.h"
#include "logger.h"
#include <algorithm>
#include <limits>
#include <optional>
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructors/Destructor:
	Mesh::Mesh()
	{
		m_name = "";
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(GpuResourceFactory::CreateMesh());
	}
	Mesh::Mesh(const std::string& name)
	{
		m_name = name;
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(GpuResourceFactory::CreateMesh());
	}
	Mesh::Mesh(emberBackendInterface::IMesh* pIMesh)
	{
		m_name = "";
		m_pIMesh = std::unique_ptr<emberBackendInterface::IMesh>(pIMesh);
	}
	Mesh::~Mesh()
	{

	}


	// Movable:
	Mesh::Mesh(Mesh&& other) noexcept = default;
	Mesh& Mesh::operator=(Mesh&& other)
	{
		if (this == &other)
			return *this;

		m_name = std::move(other.m_name);
		m_positions = std::move(other.m_positions);
		m_normals = std::move(other.m_normals);
		m_tangents = std::move(other.m_tangents);
		m_colors = std::move(other.m_colors);
		m_uvs = std::move(other.m_uvs);
		m_triangles = std::move(other.m_triangles);
		m_submeshes = std::move(other.m_submeshes);
		m_bounds = other.m_bounds;

		// Preserve backend mesh identity for already queued draw calls.
		if (m_pIMesh)
			RegisterUpdate();
		else
			m_pIMesh = std::move(other.m_pIMesh);

		return *this;
	}


	// Setters:
	void Mesh::SetName(const std::string& name)
	{
		m_name = name;
	}



	// Geometry: (validates and replaces entire mesh data)
	void Mesh::SetGeometry(const emberCommon::MeshGeometry& geometry)
	{
		MoveGeometry(emberCommon::MeshGeometry(geometry));
	}
	void Mesh::MoveGeometry(emberCommon::MeshGeometry&& geometry)
	{
		ValidateGeometry(geometry);
		m_positions = std::move(geometry.positions);
		m_normals = std::move(geometry.normals);
		m_tangents = std::move(geometry.tangents);
		m_colors = std::move(geometry.colors);
		m_uvs = std::move(geometry.uvs);
		m_triangles = std::move(geometry.triangles);
		if (geometry.submeshes.empty())
			m_submeshes = { emberCommon::Submesh{ 0, GetVertexCount(), 0, GetTriangleCount() } };
		else
			m_submeshes = std::move(geometry.submeshes);
		UpdateBounds();
		RegisterUpdate();
	}



	// Optional vertex attributes: (added attributes must match the vertex count, adding an existing attribute is ignored)
	void Mesh::AddNormals(const std::vector<Float3>& normals)
	{
		if (!m_normals.empty())
		{
			LOG_WARN("Mesh::AddNormals(...) mesh '{}' already has normals. Use ModifyNormals(...) or RemoveNormals() first.", m_name);
			return;
		}
		ValidateAttributeCount(normals.size(), m_positions.size(), "normals");
		m_normals = normals;
		RegisterUpdate();
	}
	void Mesh::AddTangents(const std::vector<Float3>& tangents)
	{
		if (!m_tangents.empty())
		{
			LOG_WARN("Mesh::AddTangents(...) mesh '{}' already has tangents. Use ModifyTangents(...) or RemoveTangents() first.", m_name);
			return;
		}
		ValidateAttributeCount(tangents.size(), m_positions.size(), "tangents");
		m_tangents = tangents;
		RegisterUpdate();
	}
	void Mesh::AddColors(const std::vector<Float4>& colors)
	{
		if (!m_colors.empty())
		{
			LOG_WARN("Mesh::AddColors(...) mesh '{}' already has colors. Use ModifyColors(...) or RemoveColors() first.", m_name);
			return;
		}
		ValidateAttributeCount(colors.size(), m_positions.size(), "colors");
		m_colors = colors;
		RegisterUpdate();
	}
	void Mesh::AddColor(const Float4& color)
	{
		if (!m_colors.empty())
		{
			LOG_WARN("Mesh::AddColor(...) mesh '{}' already has colors. Use ModifyColors(...) or RemoveColors() first.", m_name);
			return;
		}
		m_colors.assign(m_positions.size(), color);
		RegisterUpdate();
	}
	void Mesh::AddUVs(const std::vector<Float4>& uvs)
	{
		if (!m_uvs.empty())
		{
			LOG_WARN("Mesh::AddUVs(...) mesh '{}' already has uvs. Use ModifyUVs(...) or RemoveUVs() first.", m_name);
			return;
		}
		ValidateAttributeCount(uvs.size(), m_positions.size(), "uvs");
		m_uvs = uvs;
		RegisterUpdate();
	}
	void Mesh::RemoveNormals()
	{
		m_normals.clear();
		RegisterUpdate();
	}
	void Mesh::RemoveTangents()
	{
		m_tangents.clear();
		RegisterUpdate();
	}
	void Mesh::RemoveColors()
	{
		m_colors.clear();
		RegisterUpdate();
	}
	void Mesh::RemoveUVs()
	{
		m_uvs.clear();
		RegisterUpdate();
	}



	// In-place vertex attribute edits: (changes are uploaded after the modifier returns)
	void Mesh::ModifyPositions(const std::function<void(std::span<Float3>)>& modifier)
	{
		modifier(m_positions);
		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ModifyNormals(const std::function<void(std::span<Float3>)>& modifier)
	{
		modifier(m_normals);
		RegisterUpdate();
	}
	void Mesh::ModifyTangents(const std::function<void(std::span<Float3>)>& modifier)
	{
		modifier(m_tangents);
		RegisterUpdate();
	}
	void Mesh::ModifyColors(const std::function<void(std::span<Float4>)>& modifier)
	{
		modifier(m_colors);
		RegisterUpdate();
	}
	void Mesh::ModifyUVs(const std::function<void(std::span<Float4>)>& modifier)
	{
		modifier(m_uvs);
		RegisterUpdate();
	}



	// Getters:
	const std::string& Mesh::GetName() const
	{
		return m_name;
	}
	uint32_t Mesh::GetVertexCount() const
	{
		return static_cast<uint32_t>(m_positions.size());
	}
	uint32_t Mesh::GetTriangleCount() const
	{
		return static_cast<uint32_t>(m_triangles.size());
	}
	uint32_t Mesh::GetSubmeshCount() const
	{
		return static_cast<uint32_t>(m_submeshes.size());
	}
	const std::vector<Float3>& Mesh::GetPositions() const
	{
		return m_positions;
	}
	const std::vector<Float3>& Mesh::GetNormals() const
	{
		return m_normals;
	}
	const std::vector<Float3>& Mesh::GetTangents() const
	{
		return m_tangents;
	}
	const std::vector<Float4>& Mesh::GetColors() const
	{
		return m_colors;
	}
	const std::vector<Float4>& Mesh::GetUVs() const
	{
		return m_uvs;
	}
	const std::vector<Uint3>& Mesh::GetTriangles() const
	{
		return m_triangles;
	}
	const emberCommon::Submesh& Mesh::GetSubmesh(uint32_t submeshIndex) const
	{
		return m_submeshes.at(submeshIndex);
	}
	const std::vector<emberCommon::Submesh>& Mesh::GetSubmeshes() const
	{
		return m_submeshes;
	}
	emberCommon::MeshGeometry Mesh::GetGeometry() const
	{
		return { m_positions, m_normals, m_tangents, m_colors, m_uvs, m_triangles, m_submeshes };
	}
    Bounds Mesh::GetBounds() const
    {
        return m_bounds;
    }
	RayMeshHit Mesh::IntersectRay(const Ray& localRay) const
	{
		return geometry3d::IntersectRayMesh(localRay, m_positions, m_triangles, m_bounds);
	}
	Mesh Mesh::GetCopy(const std::string& newName) const
	{
		Mesh copy(newName.empty() ? m_name : newName);
		copy.MoveGeometry(GetGeometry());
		return copy;
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
	Mesh& Mesh::Mirror(const Float3& planePosition, const Float3& planeNormal)&
	{
		ApplyMirror(planePosition, planeNormal);
		return *this;
	}
	Mesh  Mesh::Mirror(const Float3& planePosition, const Float3& planeNormal)&&
	{
		ApplyMirror(planePosition, planeNormal);
		return std::move(*this);
	}
	void Mesh::ComputeNormals()
	{
		m_normals.clear();
		m_normals.resize(GetVertexCount(), Float3::zero);
		for (const Uint3& triangle : m_triangles)
		{
			Float3 normal = Float3::Cross(m_positions[triangle[1]] - m_positions[triangle[0]], m_positions[triangle[2]] - m_positions[triangle[0]]).Normalize();
			m_normals[triangle[0]] += normal;
			m_normals[triangle[1]] += normal;
			m_normals[triangle[2]] += normal;
		}
		for (Float3& normal : m_normals)
			normal = normal.Normalize();
		RegisterUpdate();
	}
	void Mesh::ComputeTangents()
	{
		m_tangents.clear();
		m_tangents.resize(GetVertexCount(), Float3::zero);
		if (m_uvs.size() != GetVertexCount())
		{
			RegisterUpdate();
			return;
		}
		for (const Uint3& triangle : m_triangles)
		{
			const Float3& p0 = m_positions[triangle[0]];
			const Float3& p1 = m_positions[triangle[1]];
			const Float3& p2 = m_positions[triangle[2]];
			const Float4& uv0 = m_uvs[triangle[0]];
			const Float4& uv1 = m_uvs[triangle[1]];
			const Float4& uv2 = m_uvs[triangle[2]];
			Float3 edge1 = p1 - p0;
			Float3 edge2 = p2 - p0;
			float deltaU1 = uv1.x - uv0.x;
			float deltaV1 = uv1.y - uv0.y;
			float deltaU2 = uv2.x - uv0.x;
			float deltaV2 = uv2.y - uv0.y;
			float determinant = deltaU1 * deltaV2 - deltaU2 * deltaV1;
			if (math::IsEpsilonZero(determinant))
				continue;
			Float3 tangent = (deltaV2 * edge1 - deltaV1 * edge2) / determinant;
			m_tangents[triangle[0]] += tangent;
			m_tangents[triangle[1]] += tangent;
			m_tangents[triangle[2]] += tangent;
		}
		for (Float3& tangent : m_tangents)
			tangent = tangent.Normalize();
		RegisterUpdate();
	}



	// Mesh operations:
	// Submeshes of all meshes are kept, in order, so each stays addressable in the merged mesh.
	Mesh Mesh::Merge(const std::vector<Mesh>& meshes, const std::string& name)
	{
		// Get total counts and the optional attributes present in all meshes:
		size_t vertexCount = 0;
		size_t triangleCount = 0;
		size_t submeshCount = 0;
		bool hasNormals = true;
		bool hasTangents = true;
		bool hasColors = true;
		bool hasUvs = true;
		for (const Mesh& mesh : meshes)
		{
			vertexCount += mesh.GetVertexCount();
			triangleCount += mesh.GetTriangleCount();
			submeshCount += mesh.GetSubmeshCount();
			hasNormals = hasNormals && mesh.m_normals.size() == mesh.GetVertexCount();
			hasTangents = hasTangents && mesh.m_tangents.size() == mesh.GetVertexCount();
			hasColors = hasColors && mesh.m_colors.size() == mesh.GetVertexCount();
			hasUvs = hasUvs && mesh.m_uvs.size() == mesh.GetVertexCount();
		}

		// Prepare merged data: (attributes missing in any mesh are dropped, as they cannot be merged consistently)
		emberCommon::MeshGeometry geometry;
		geometry.positions.reserve(vertexCount);
		if (hasNormals)
			geometry.normals.reserve(vertexCount);
		if (hasTangents)
			geometry.tangents.reserve(vertexCount);
		if (hasColors)
			geometry.colors.reserve(vertexCount);
		if (hasUvs)
			geometry.uvs.reserve(vertexCount);
		geometry.triangles.reserve(triangleCount);
		geometry.submeshes.reserve(submeshCount);

		for (const Mesh& mesh : meshes)
		{
			uint32_t vertexOffset = static_cast<uint32_t>(geometry.positions.size());
			uint32_t triangleOffset = static_cast<uint32_t>(geometry.triangles.size());

			// Append vertex attributes:
			geometry.positions.insert(geometry.positions.end(), mesh.m_positions.begin(), mesh.m_positions.end());
			if (hasNormals)
				geometry.normals.insert(geometry.normals.end(), mesh.m_normals.begin(), mesh.m_normals.end());
			if (hasTangents)
				geometry.tangents.insert(geometry.tangents.end(), mesh.m_tangents.begin(), mesh.m_tangents.end());
			if (hasColors)
				geometry.colors.insert(geometry.colors.end(), mesh.m_colors.begin(), mesh.m_colors.end());
			if (hasUvs)
				geometry.uvs.insert(geometry.uvs.end(), mesh.m_uvs.begin(), mesh.m_uvs.end());

			// Append triangles:
			for (const Uint3& triangle : mesh.m_triangles)
				geometry.triangles.push_back(triangle + Uint3(vertexOffset));

			// Append submeshes with ranges relative to the merged mesh:
			for (emberCommon::Submesh submesh : mesh.m_submeshes)
			{
				submesh.firstVertex += vertexOffset;
				submesh.firstTriangle += triangleOffset;
				geometry.submeshes.push_back(submesh);
			}
		}

		// Construct mergedMesh:
		Mesh mergedMesh(name);
		mergedMesh.MoveGeometry(std::move(geometry));
		return mergedMesh;
	}



	// Private methods:
	// Backend access:
	emberBackendInterface::IMesh* const Mesh::GetInterfaceHandle() const
	{
		return m_pIMesh.get();
	}
	// Validation:
	void Mesh::ValidateGeometry(const emberCommon::MeshGeometry& geometry)
	{
		// Counts:
		size_t vertexCount = geometry.positions.size();
		size_t triangleCount = geometry.triangles.size();
		if (vertexCount > std::numeric_limits<uint32_t>::max() || 3 * triangleCount > std::numeric_limits<uint32_t>::max())
			throw std::length_error("Mesh::ValidateGeometry(...) failed. Geometry exceeds the supported vertex or index count.");
		ValidateAttributeCount(geometry.normals.size(), vertexCount, "normals");
		ValidateAttributeCount(geometry.tangents.size(), vertexCount, "tangents");
		ValidateAttributeCount(geometry.colors.size(), vertexCount, "colors");
		ValidateAttributeCount(geometry.uvs.size(), vertexCount, "uvs");

		// Triangles:
		for (const Uint3& triangle : geometry.triangles)
			for (uint32_t j = 0; j < 3; j++)
				if (triangle[j] >= vertexCount)
					throw std::invalid_argument("Mesh::ValidateGeometry(...) failed. Triangle references a vertex outside the mesh.");

		// Submeshes: (ordered, contiguous, covering all vertices and triangles, triangles only reference their own vertices)
		if (geometry.submeshes.empty())
			return;
		uint64_t vertexEnd = 0;
		uint64_t triangleEnd = 0;
		for (const emberCommon::Submesh& submesh : geometry.submeshes)
		{
			if (submesh.firstVertex != vertexEnd || submesh.firstTriangle != triangleEnd)
				throw std::invalid_argument("Mesh::ValidateGeometry(...) failed. Submeshes must be ordered and contiguous.");
			vertexEnd += submesh.vertexCount;
			triangleEnd += submesh.triangleCount;
			if (vertexEnd > vertexCount || triangleEnd > triangleCount)
				throw std::invalid_argument("Mesh::ValidateGeometry(...) failed. Submesh ranges exceed the mesh.");

			for (uint64_t i = submesh.firstTriangle; i < triangleEnd; i++)
				for (uint32_t j = 0; j < 3; j++)
					if (geometry.triangles[i][j] < submesh.firstVertex || geometry.triangles[i][j] >= vertexEnd)
						throw std::invalid_argument("Mesh::ValidateGeometry(...) failed. Submesh triangles must only reference vertices of their own submesh.");
		}
		if (vertexEnd != vertexCount || triangleEnd != triangleCount)
			throw std::invalid_argument("Mesh::ValidateGeometry(...) failed. Submeshes must cover all vertices and triangles.");
	}
	void Mesh::ValidateAttributeCount(size_t attributeCount, size_t vertexCount, const std::string& attributeName)
	{
		if (attributeCount != 0 && attributeCount != vertexCount)
			throw std::invalid_argument("Mesh " + attributeName + " count must be 0 or match the vertex count.");
	}
	// GPU upload/Bounds:
	void Mesh::RegisterUpdate()
	{
		uint32_t vertexCount = GetVertexCount();
		if (vertexCount == 0 || m_triangles.empty())
			return;

		std::vector<Float3>* pNormals = m_normals.empty() ? nullptr : &m_normals;
		std::vector<Float3>* pTangents = m_tangents.empty() ? nullptr : &m_tangents;
		std::vector<Float4>* pColors = m_colors.empty() ? nullptr : &m_colors;
		std::vector<Float4>* pUvs = m_uvs.empty() ? nullptr : &m_uvs;
		m_pIMesh->UpdateVertexBuffer(m_positions, pNormals, pTangents, pColors, pUvs);
		m_pIMesh->UpdateIndexBuffer(m_triangles, m_submeshes, m_bounds, vertexCount);
	}
	void Mesh::UpdateBounds()
	{
		m_bounds = m_positions.empty() ? Bounds() : Bounds(m_positions);
		for (emberCommon::Submesh& submesh : m_submeshes)
		{
			submesh.bounds = Bounds();
			if (submesh.triangleCount == 0)
				continue;

			// Initialize from geometry so the bounds do not include the origin unnecessarily:
			submesh.bounds = Bounds(m_positions[m_triangles[submesh.firstTriangle][0]], Float3::zero);
			uint32_t triangleEnd = submesh.firstTriangle + submesh.triangleCount;
			for (uint32_t i = submesh.firstTriangle; i < triangleEnd; i++)
			{
				const Uint3& triangle = m_triangles[i];
				for (uint32_t j = 0; j < 3; j++)
					submesh.bounds.Encapsulate(m_positions[triangle[j]]);
			}
		}
	}
	// Mesh modifiers:
	void Mesh::ApplyTranslate(const Float3& translation)
	{
		std::vector<Float3>& positions = m_positions;
		for (Float3& position : positions)
			position += translation;
		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ApplyRotate(const Float3x3& rotation)
	{
		uint32_t vertexCount = GetVertexCount();

		// Positions:
		std::vector<Float3>& positions = m_positions;
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = rotation * positions[i];

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = rotation * normals[i];

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = rotation * tangents[i];

		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ApplyRotate(const Float4x4& rotation)
	{
		uint32_t vertexCount = GetVertexCount();
		Float3x3 rotation3x3 = Float3x3(rotation);

		// Positions:
		std::vector<Float3>& positions = m_positions;
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = rotation3x3 * positions[i];

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = rotation3x3 * normals[i];

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = rotation3x3 * tangents[i];

		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ApplyScale(const Float3& scale)
	{
		uint32_t vertexCount = GetVertexCount();
		Float3 invScale = 1.0 / scale;

		// Positions:
		std::vector<Float3>& positions = m_positions;
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = scale * positions[i];

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = (invScale * normals[i]).Normalize();

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = (invScale * tangents[i]).Normalize();

		UpdateBounds();
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
		std::vector<Float3>& positions = m_positions;
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] = Float3(transformationMatrix * Float4(positions[i], 1.0f));

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] = Float3(normalMatrix * Float4(normals[i], 0.0f)).Normalize();

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] = Float3(normalMatrix * Float4(tangents[i], 0.0f)).Normalize();

		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ApplySubdivide()
	{
		uint32_t vertexCount = GetVertexCount();
		uint32_t triangleCount = GetTriangleCount();
		bool hasNormals = !m_normals.empty();
		bool hasTangents = !m_tangents.empty();
		bool hasColors = !m_colors.empty();
		bool hasUvs = !m_uvs.empty();

		// Prepare subdivided data: (each triangle gets 3 midpoint vertices and is split into 4 triangles)
		uint32_t newVertexCount = vertexCount + 3 * triangleCount;
		emberCommon::MeshGeometry geometry;
		geometry.positions.reserve(newVertexCount);
		if (hasNormals)
			geometry.normals.reserve(newVertexCount);
		if (hasTangents)
			geometry.tangents.reserve(newVertexCount);
		if (hasColors)
			geometry.colors.reserve(newVertexCount);
		if (hasUvs)
			geometry.uvs.reserve(newVertexCount);
		geometry.triangles.reserve(4 * triangleCount);
		geometry.submeshes.reserve(m_submeshes.size());

		// Keep each submesh's vertices contiguous by placing its midpoint vertices directly after its original vertices:
		for (const emberCommon::Submesh& submesh : m_submeshes)
		{
			uint32_t firstVertex = static_cast<uint32_t>(geometry.positions.size());
			uint32_t vertexOffset = firstVertex - submesh.firstVertex;
			uint32_t vertexEnd = submesh.firstVertex + submesh.vertexCount;
			uint32_t triangleEnd = submesh.firstTriangle + submesh.triangleCount;

			// Copy original vertices:
			geometry.positions.insert(geometry.positions.end(), m_positions.begin() + submesh.firstVertex, m_positions.begin() + vertexEnd);
			if (hasNormals)
				geometry.normals.insert(geometry.normals.end(), m_normals.begin() + submesh.firstVertex, m_normals.begin() + vertexEnd);
			if (hasTangents)
				geometry.tangents.insert(geometry.tangents.end(), m_tangents.begin() + submesh.firstVertex, m_tangents.begin() + vertexEnd);
			if (hasColors)
				geometry.colors.insert(geometry.colors.end(), m_colors.begin() + submesh.firstVertex, m_colors.begin() + vertexEnd);
			if (hasUvs)
				geometry.uvs.insert(geometry.uvs.end(), m_uvs.begin() + submesh.firstVertex, m_uvs.begin() + vertexEnd);

			// Subdivide each triangle:
			uint32_t newVertexIndex = firstVertex + submesh.vertexCount;
			for (uint32_t i = submesh.firstTriangle; i < triangleEnd; i++)
			{
				const Uint3& triangle = m_triangles[i];

				// Add positions:
				geometry.positions.push_back(0.5f * (m_positions[triangle[0]] + m_positions[triangle[1]]));
				geometry.positions.push_back(0.5f * (m_positions[triangle[1]] + m_positions[triangle[2]]));
				geometry.positions.push_back(0.5f * (m_positions[triangle[2]] + m_positions[triangle[0]]));

				// Add normals:
				if (hasNormals)
				{
					geometry.normals.push_back((m_normals[triangle[0]] + m_normals[triangle[1]]).Normalize());
					geometry.normals.push_back((m_normals[triangle[1]] + m_normals[triangle[2]]).Normalize());
					geometry.normals.push_back((m_normals[triangle[2]] + m_normals[triangle[0]]).Normalize());
				}

				// Add tangents:
				if (hasTangents)
				{
					geometry.tangents.push_back((m_tangents[triangle[0]] + m_tangents[triangle[1]]).Normalize());
					geometry.tangents.push_back((m_tangents[triangle[1]] + m_tangents[triangle[2]]).Normalize());
					geometry.tangents.push_back((m_tangents[triangle[2]] + m_tangents[triangle[0]]).Normalize());
				}

				// Add colors:
				if (hasColors)
				{
					geometry.colors.push_back(0.5f * (m_colors[triangle[0]] + m_colors[triangle[1]]));
					geometry.colors.push_back(0.5f * (m_colors[triangle[1]] + m_colors[triangle[2]]));
					geometry.colors.push_back(0.5f * (m_colors[triangle[2]] + m_colors[triangle[0]]));
				}

				// Add Uvs:
				if (hasUvs)
				{
					geometry.uvs.push_back(0.5f * (m_uvs[triangle[0]] + m_uvs[triangle[1]]));
					geometry.uvs.push_back(0.5f * (m_uvs[triangle[1]] + m_uvs[triangle[2]]));
					geometry.uvs.push_back(0.5f * (m_uvs[triangle[2]] + m_uvs[triangle[0]]));
				}

				// Add 4 triangles:
				Uint3 corners = triangle + Uint3(vertexOffset);
				geometry.triangles.push_back(Uint3(corners[0], newVertexIndex, newVertexIndex + 2));
				geometry.triangles.push_back(Uint3(newVertexIndex, newVertexIndex + 1, newVertexIndex + 2));
				geometry.triangles.push_back(Uint3(newVertexIndex, corners[1], newVertexIndex + 1));
				geometry.triangles.push_back(Uint3(newVertexIndex + 2, newVertexIndex + 1, corners[2]));

				newVertexIndex += 3;
			}

			// Subdivided submesh:
			geometry.submeshes.push_back(emberCommon::Submesh{ firstVertex, submesh.vertexCount + 3 * submesh.triangleCount, 4 * submesh.firstTriangle, 4 * submesh.triangleCount });
		}

		// Update mesh data:
		MoveGeometry(std::move(geometry));
	}
	void Mesh::ApplySpherify(float factor, float radius)
	{
		factor = std::clamp(factor, 0.0f, 1.0f);
		radius = std::max(1e-8f, radius);

		std::vector<Float3>& positions = m_positions;
		std::vector<Float3>& normals = m_normals;
		std::vector<Float3>& tangents = m_tangents;

		for (uint32_t i = 0; i < GetVertexCount(); i++)
		{
			Float3 sphereNormal = positions[i].Normalize();
			positions[i] = radius * (positions[i] + factor * (sphereNormal - positions[i]));
			normals[i] = (normals[i] + factor * (sphereNormal - normals[i])).Normalize();
			std::optional<Float3> tangentProjection = geometry3d::PointToPlaneProjection(tangents[i], Float3::zero, normals[i]);
			if (tangentProjection.has_value())
    			tangents[i] = tangentProjection.value().Normalize();
		}

		UpdateBounds();
		RegisterUpdate();
	}
	void Mesh::ApplyInvertFaces()
	{
		uint32_t vertexCount = GetVertexCount();

		// Triangles:
		std::vector<Uint3>& triangles = m_triangles;
		for (Uint3& triangle : triangles)
			std::swap(triangle[1], triangle[2]);

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (Float3& normal : normals)
				normal = -normal;

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (Float3& tangent : tangents)
				tangent = -tangent;

		RegisterUpdate();
	}
	void Mesh::ApplyRescaleUVs(const Float4& scale, const Float4& offset)
	{
		std::vector<Float4>& uvs = m_uvs;
		for (Float4& uv : uvs)
			uv = scale * uv + offset;
		RegisterUpdate();
	}
	void Mesh::ApplyMirror(const Float3& planePosition, const Float3& planeNormal)
	{
		if (planeNormal.IsEpsilonZero())
			return;

		uint32_t vertexCount = GetVertexCount();
		Float3 normal = planeNormal.Normalize();

		// Positions:
		std::vector<Float3>& positions = m_positions;
		for (uint32_t i = 0; i < vertexCount; i++)
			positions[i] -= 2.0f * Float3::Dot(positions[i] - planePosition, normal) * normal;

		// Normals:
		std::vector<Float3>& normals = m_normals;
		if (normals.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				normals[i] -= 2.0f * Float3::Dot(normals[i], normal) * normal;

		// Tangents:
		std::vector<Float3>& tangents = m_tangents;
		if (tangents.size() == vertexCount)
			for (uint32_t i = 0; i < vertexCount; i++)
				tangents[i] -= 2.0f * Float3::Dot(tangents[i], normal) * normal;

		// Triangles:
		std::vector<Uint3>& triangles = m_triangles;
		for (Uint3& triangle : triangles)
			std::swap(triangle[1], triangle[2]);

		UpdateBounds();
		RegisterUpdate();
	}
}