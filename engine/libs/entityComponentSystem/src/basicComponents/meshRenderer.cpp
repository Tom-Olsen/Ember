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
        m_cullMode = emberCommon::CullMode::pipelineDefault;
        m_drawLocalBounds = false;
        m_drawWorldBounds = false;

		m_pMesh = nullptr;
		m_material = MaterialManager::GetMaterial("errorMaterial");
		m_shaderProperties = ShaderProperties(m_material);
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
	}
	void MeshRenderer::SetMaterial(const Material& material)
	{
		if (m_material.GetName() != material.GetName())
		{
			m_material = material;
			m_shaderProperties = ShaderProperties{ material };
		}
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
	Material& MeshRenderer::GetMaterial()
	{
		return m_material;
	}
	ShaderProperties& MeshRenderer::GetShaderProperties()
	{
		return m_shaderProperties;
	}



	// Overrides:
	void MeshRenderer::Update()
	{
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		if (m_pMesh != nullptr)
        {
            Renderer::DrawMesh(*m_pMesh, m_material, m_shaderProperties, localToWorldMatrix, m_receiveShadows, m_castShadows, m_cullMode);
            if (m_drawLocalBounds)
                Gizmo::DrawBounds(localToWorldMatrix, GetLocalBounds());
            if (m_drawWorldBounds)
                Gizmo::DrawBounds(Float4x4::identity, GetWorldBounds());
        }
	}
}