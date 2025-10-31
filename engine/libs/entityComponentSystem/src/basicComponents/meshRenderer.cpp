#include "meshRenderer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	MeshRenderer::MeshRenderer()
	{
		m_castShadows = true;
		m_receiveShadows = true;

		m_pMesh = nullptr;
		m_pMaterial = MaterialManager::TryGetMaterial("errorMaterial");
		m_shaderProperties = ShaderProperties(*m_pMaterial);
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
	void MeshRenderer::SetMesh(Mesh& mesh)
	{
		m_pMesh = &mesh;
	}
	void MeshRenderer::SetMaterial(Material& material)
	{
		if (m_pMaterial->GetName() != material.GetName())
		{
			m_pMaterial = &material;
			m_shaderProperties = ShaderProperties(material);
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
	Mesh& MeshRenderer::GetMesh()
	{
		return *m_pMesh;
	}
	Material& MeshRenderer::GetMaterial()
	{
		return *m_pMaterial;
	}
	ShaderProperties& MeshRenderer::GetShaderProperties()
	{
		return m_shaderProperties;
	}



	// Overrides:
	void MeshRenderer::Update()
	{
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		Renderer::DrawMesh(*m_pMesh, *m_pMaterial, m_shaderProperties, localToWorldMatrix, m_receiveShadows, m_castShadows);
	}
}