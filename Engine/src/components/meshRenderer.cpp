#include "meshRenderer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	MeshRenderer::MeshRenderer()
	{
		m_castShadows = true;
		m_receiveShadows = true;

		m_pMesh = nullptr;
		m_pMaterial = MaterialManager::GetMaterial("errorMaterial");
		m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
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
	void MeshRenderer::SetMesh(Mesh* pMesh)
	{
		m_pMesh = pMesh;
	}
	void MeshRenderer::SetMaterial(Material* pMaterial)
	{
		if (pMaterial == nullptr)
		{
			if (m_pMaterial != MaterialManager::GetMaterial("errorMaterial"))
			{
				m_pMaterial = MaterialManager::GetMaterial("errorMaterial");
				m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
			}
		}
		else if (m_pMaterial != pMaterial)
		{
			m_pMaterial = pMaterial;
			m_pShaderProperties = std::make_unique<ShaderProperties>(m_pMaterial);
		}
	}



	// Getters:
	bool MeshRenderer::GetCastShadows() const
	{
		return m_castShadows;
	}
	bool MeshRenderer::GetReceiveShadows() const
	{
		return m_receiveShadows;
	}
	Mesh* MeshRenderer::GetMesh()
	{
		return m_pMesh;
	}
	Material* MeshRenderer::GetMaterial()
	{
		return m_pMaterial;
	}
	ShaderProperties* MeshRenderer::GetShaderProperties()
	{
		return m_pShaderProperties.get();
	}



	// Overrides:
	void MeshRenderer::Update()
	{
		Float4x4 localToWorldMatrix = m_pTransform->GetLocalToWorldMatrix();
		Graphics::DrawMesh(m_pMesh, m_pMaterial, m_pShaderProperties.get(), localToWorldMatrix, m_receiveShadows, m_castShadows);
	}
	const std::string MeshRenderer::ToString() const
	{
		return "MeshRenderer";
	}
}