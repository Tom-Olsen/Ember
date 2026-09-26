#include "instancedMeshRenderer.h"
#include "materialManager.h"
#include "renderer.h"
#include "transform.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	InstancedMeshRenderer::InstancedMeshRenderer()
	{
		m_instanceCount = 1;
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
		m_instanceCount = instanceCount;
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
	void InstancedMeshRenderer::SetInstanceBuffer(Buffer& instanceBuffer)
	{
		m_pInstanceBuffer = &instanceBuffer;
	}



	// Getters:
	uint32_t InstancedMeshRenderer::GetInstanceCount() const
	{
		return math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
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
	Buffer& InstancedMeshRenderer::GetInstanceBuffer()
	{
		return *m_pInstanceBuffer;
	}
	CallProperties& InstancedMeshRenderer::GetCallProperties()
	{
		return m_callProperties;
	}
	// Overrides:
	void InstancedMeshRenderer::Update()
	{
		uint32_t instanceCount = math::Min(m_instanceCount, m_pInstanceBuffer->GetCount());
		Float4x4 localToWorldMatrix = GetTransform()->GetLocalToWorldMatrix();
		DrawData drawData(localToWorldMatrix, *m_pMesh, m_material, instanceCount, m_pInstanceBuffer, m_receiveShadows, m_castShadows, m_cullMode);
		Renderer::DrawMesh(drawData, m_callProperties);
	}
}