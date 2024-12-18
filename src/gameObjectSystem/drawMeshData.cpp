#include "drawMeshData.h"



// Constructor/Destructor:
DrawMeshData::DrawMeshData()
{
	m_pSphereMesh = MeshGenerator::CubeSphere(0.5f, 1, "cubeSphere");
	m_pArrowMesh = MeshGenerator::ArrowEdgy(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 4, "arrowEdgy");
	m_pMesh = nullptr;
	m_pMaterial = nullptr;
	m_pMaterialProperties = nullptr;
}
DrawMeshData::~DrawMeshData()
{
	delete m_pSphereMesh;
	delete m_pArrowMesh;
}



// Overrides:
void DrawMeshData::LateUpdate()
{
	bool receiveShadows = false;
	bool castShadows = false;

	if (m_pSphereMesh == nullptr)
		m_pSphereMesh = MeshManager::GetMesh("cubeSphere");
	if (m_pArrowMesh == nullptr)
		m_pArrowMesh = MeshManager::GetMesh("arrowEdgy");
	if (m_pMaterial == nullptr)
		m_pMaterial = MaterialManager::GetMaterial("default");
	if (m_pMesh == nullptr)
		m_pMesh = GetGameObject()->GetComponent<MeshRenderer>()->GetMesh();

	// Transformation matrices:
	Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
	Float4x4 normalMatrix = GetTransform()->GetNormalMatrix();

	for (uint32_t i = 0; i < m_pMesh->GetVertexCount(); i++)
	{
		// Local space data:
		Float3 localPosition = m_pMesh->GetPositions()[i] + 0.05f * m_pMesh->GetNormals()[i];
		Float3 localNormal = m_pMesh->GetNormals()[i];
		Float3 localTangent = m_pMesh->GetTangents()[i];

		// World space data:
		Float3 worldPosition = Float3(localToWorld * Float4(localPosition, 1.0f));
		Float3 worldNormal = Float3(normalMatrix * Float4(localNormal, 0.0f));
		Float3 worldTangent = Float3(normalMatrix * Float4(localTangent, 0.0f));
		
		// Draw calls:
		m_pMaterialProperties = Graphics::DrawMesh(m_pSphereMesh, m_pMaterial, Float3(worldPosition), Float3x3::identity, 0.1f, receiveShadows, castShadows);
		m_pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.66f, 0.33f, 0.0f, 1.0f));

		m_pMaterialProperties = Graphics::DrawMesh(m_pArrowMesh, m_pMaterial, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldNormal), 0.1f, receiveShadows, castShadows);
		m_pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.0f, 0.0f, 1.0f, 1.0f));

		m_pMaterialProperties = Graphics::DrawMesh(m_pArrowMesh, m_pMaterial, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldTangent), 0.1f, receiveShadows, castShadows);
		m_pMaterialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}
const std::string DrawMeshData::ToString() const
{
	return "DrawMeshData";
}