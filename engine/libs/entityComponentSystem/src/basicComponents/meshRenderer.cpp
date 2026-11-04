#include "meshRenderer.h"
#include "gizmo.h"
#include "materialManager.h"
#include "renderer.h"
#include "transform.h"
#include <array>
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	MeshRenderer::MeshRenderer()
	{
		m_castShadows = true;
		m_receiveShadows = true;
		m_cullMode = emberCommon::CullMode::materialDefault;
        m_drawLocalBounds = false;
        m_drawWorldBounds = false;
		m_pMesh = nullptr;
		m_raycastEnabled = true;
	}
	MeshRenderer::~MeshRenderer()
	{

	}



	// Public methods:
	// Setter:
	void MeshRenderer::SetCastShadows(bool castShadows)
	{
		m_castShadows = castShadows;
	}
	void MeshRenderer::SetReceiveShadows(bool receiveShadows)
	{
		m_receiveShadows = receiveShadows;
	}
	void MeshRenderer::SetCullMode(emberCommon::CullMode cullMode)
	{
		m_cullMode = cullMode;
	}
	void MeshRenderer::SetMesh(Mesh& mesh)
	{
		m_pMesh = &mesh;
		SyncSubmeshProperties();
	}
	void MeshRenderer::SetMaterial(const Material& material, uint32_t submeshIndex)
	{
		SyncSubmeshProperties();
		if (submeshIndex >= m_submeshProperties.size())
			return;
		AssignMaterial(submeshIndex, material);
	}
	void MeshRenderer::SetMaterials(const std::vector<Material>& materials)
	{
		SyncSubmeshProperties();
		Material errorMaterial = MaterialManager::TryGetMaterial("errorMaterial");
		for (size_t submeshIndex = 0; submeshIndex < m_submeshProperties.size(); submeshIndex++)
			AssignMaterial(static_cast<uint32_t>(submeshIndex), submeshIndex < materials.size() ? materials[submeshIndex] : errorMaterial);
	}
	void MeshRenderer::SetRaycastEnabled(bool raycastEnabled)
	{
		m_raycastEnabled = raycastEnabled;
	}



	// Getters:
	Bounds MeshRenderer::GetLocalBounds() const
	{
		return (m_pMesh != nullptr) ? m_pMesh->GetBounds() : Bounds();
	}
	Bounds MeshRenderer::GetWorldBounds() const
	{
		if (m_pMesh == nullptr)
			return Bounds();

		std::array<Float3, 8> localCorners = GetLocalBounds().GetCorners();
		std::array<Float3, 8> worldCorners;
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		for (std::size_t i = 0; i < worldCorners.size(); i++)
			worldCorners[i] = Float3(localToWorldMatrix * Float4(localCorners[i], 1.0f));
		return Bounds(worldCorners.data());
	}
	bool MeshRenderer::HasMesh() const
	{
		return m_pMesh != nullptr;
	}
	bool MeshRenderer::GetCastShadows() const
	{
		return m_castShadows;
	}
	bool MeshRenderer::GetReceiveShadows() const
	{
		return m_receiveShadows;
	}
	emberCommon::CullMode MeshRenderer::GetCullMode() const
	{
		return m_cullMode;
	}
	Mesh& MeshRenderer::GetMesh()
	{
		return *m_pMesh;
	}
	Material MeshRenderer::GetMaterial(uint32_t submeshIndex) const
	{
		SyncSubmeshProperties();
		if (submeshIndex >= m_submeshProperties.size())
			return MaterialManager::TryGetMaterial("errorMaterial");
		return m_submeshProperties[submeshIndex].material;
	}
	std::vector<Material> MeshRenderer::GetMaterials() const
	{
		SyncSubmeshProperties();
		std::vector<Material> materials;
		materials.reserve(m_submeshProperties.size());
		for (const SubmeshProperties& submeshProperties : m_submeshProperties)
			materials.push_back(submeshProperties.material);
		return materials;
	}
	CallProperties& MeshRenderer::GetCallProperties(uint32_t submeshIndex)
	{
		SyncSubmeshProperties();
		return m_submeshProperties.at(submeshIndex).callProperties;
	}
	bool MeshRenderer::GetRaycastEnabled() const
	{
		return m_raycastEnabled;
	}



	// Overrides:
	void MeshRenderer::Update()
	{
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		if (m_pMesh != nullptr)
		{
			// Submesh count can change when the mesh is modified after SetMesh(...):
			SyncSubmeshProperties();
			for (uint32_t submeshIndex = 0; submeshIndex < m_pMesh->GetSubmeshCount(); submeshIndex++)
			{
				SubmeshProperties& submeshProperties = m_submeshProperties[submeshIndex];
				DrawData drawData(localToWorldMatrix, *m_pMesh, submeshProperties.material, m_receiveShadows, m_castShadows, m_cullMode);
				Renderer::DrawSubmesh(drawData, submeshIndex, submeshProperties.callProperties);
			}
			if (m_drawLocalBounds)
				Gizmo::DrawBounds(localToWorldMatrix, GetLocalBounds());
			if (m_drawWorldBounds)
				Gizmo::DrawBounds(Float4x4::identity, GetWorldBounds());
		}
	}



	// Private methods:
	// Submesh properties:
	void MeshRenderer::SyncSubmeshProperties() const
	{
		// Slot count follows the submesh count of the mesh. New slots use the error material until a material is assigned:
		size_t submeshCount = (m_pMesh != nullptr) ? m_pMesh->GetSubmeshCount() : 0;
		size_t oldCount = m_submeshProperties.size();
		if (submeshCount == oldCount)
			return;

		Material errorMaterial = MaterialManager::TryGetMaterial("errorMaterial");
		m_submeshProperties.resize(submeshCount);
		for (size_t submeshIndex = oldCount; submeshIndex < submeshCount; submeshIndex++)
		{
			m_submeshProperties[submeshIndex].material = errorMaterial;
			m_submeshProperties[submeshIndex].callProperties = CallProperties(errorMaterial);
		}
	}
	void MeshRenderer::AssignMaterial(uint32_t submeshIndex, const Material& material)
	{
		// Invalid materials fall back to the error material.
		// CallProperties are bound to a material and only rebuilt when it changes:
		Material resolvedMaterial = material.IsValid() ? material : MaterialManager::TryGetMaterial("errorMaterial");
		SubmeshProperties& submeshProperties = m_submeshProperties[submeshIndex];
		if (submeshProperties.material.GetName() == resolvedMaterial.GetName())
			return;
		submeshProperties.material = resolvedMaterial;
		submeshProperties.callProperties = CallProperties(resolvedMaterial);
	}
}