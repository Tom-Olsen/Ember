#include "instancedMeshRenderer.h"
#include "materialManager.h"
#include "renderer.h"
#include "transform.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	InstancedMeshRenderer::InstancedMeshRenderer(uint32_t instanceCount)
		: m_instanceBuffer(instanceCount, "InstancedMeshRenderer")
	{
		m_castShadows = true;
		m_receiveShadows = true;
		m_cullMode = emberCommon::CullMode::materialDefault;

		m_pMesh = nullptr;
		m_material = MaterialManager::TryGetMaterial("errorMaterial");
		m_callProperties = CallProperties(m_material);
	}
	InstancedMeshRenderer::~InstancedMeshRenderer()
	{

	}



	// Public methods:
	// Setter:
	void InstancedMeshRenderer::SetInstanceCount(uint32_t instanceCount)
	{
		if (instanceCount == m_instanceBuffer.GetCount())
			return;
		m_instanceBuffer = InstanceBuffer(instanceCount, m_instanceBuffer.GetName());
	}
	void InstancedMeshRenderer::SetCastShadows(bool castShadows)
	{
		m_castShadows = castShadows;
	}
	void InstancedMeshRenderer::SetReceiveShadows(bool receiveShadows)
	{
		m_receiveShadows = receiveShadows;
	}
	void InstancedMeshRenderer::SetCullMode(emberCommon::CullMode cullMode)
	{
		m_cullMode = cullMode;
	}
	void InstancedMeshRenderer::SetMesh(Mesh& mesh)
	{
		m_pMesh = &mesh;
	}
	void InstancedMeshRenderer::SetMaterial(const Material& material)
	{
		if (m_material.GetName() != material.GetName())
		{
			m_material = material;
			m_callProperties = CallProperties{ material };
		}
	}



	// Getters:
	uint32_t InstancedMeshRenderer::GetInstanceCount() const
	{
		return m_instanceBuffer.GetCount();
	}
	bool InstancedMeshRenderer::GetCastShadows() const
	{
		return m_castShadows;
	}
	bool InstancedMeshRenderer::GetReceiveShadows() const
	{
		return m_receiveShadows;
	}
	emberCommon::CullMode InstancedMeshRenderer::GetCullMode() const
	{
		return m_cullMode;
	}
	Mesh& InstancedMeshRenderer::GetMesh()
	{
		return *m_pMesh;
	}
	Material& InstancedMeshRenderer::GetMaterial()
	{
		return m_material;
	}
	InstanceBuffer& InstancedMeshRenderer::GetInstanceBuffer()
	{
		return m_instanceBuffer;
	}
	CallProperties& InstancedMeshRenderer::GetCallProperties()
	{
		return m_callProperties;
	}
	// Overrides:
	void InstancedMeshRenderer::Update()
	{
		if (!m_pMesh)
			return;

		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		DrawData drawData(localToWorldMatrix, *m_pMesh, m_material, m_instanceBuffer.GetCount(), &m_instanceBuffer, m_receiveShadows, m_castShadows, m_cullMode);
		Renderer::DrawMesh(drawData, m_callProperties);
	}
}