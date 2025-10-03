#include "drawMeshData.h"
#include "gameObject.h"
#include "meshRenderer.h"
using namespace emberEngine;



namespace emberEngine
{
	// Constructor/Destructor:
	DrawMeshData::DrawMeshData()
	{
		m_pSphereMesh = MeshManager::TryGetMesh("cubeSphere");
		m_pArrowMesh = MeshManager::TryGetMesh("arrowFlat");
		m_pMesh = nullptr;
		m_pMaterial = nullptr;
	}
	DrawMeshData::~DrawMeshData()
	{
		delete m_pSphereMesh;
		delete m_pArrowMesh;
	}



	// Overrides:
	void DrawMeshData::Start()
	{
		m_pSphereMesh = MeshManager::TryGetMesh("cubeSphere");
		m_pArrowMesh = MeshManager::TryGetMesh("arrowFlat");
		m_pMaterial = MaterialManager::TryGetMaterial("defaultMaterial");
		m_pMesh = &GetGameObject()->GetComponent<MeshRenderer>()->GetMesh();
	}
	void DrawMeshData::LateUpdate()
	{
		bool receiveShadows = false;
		bool castShadows = false;

		// Transformation matrices:
		Float4x4 localToWorld = GetTransform()->GetLocalToWorldMatrix();
		Float4x4 normalMatrix = GetTransform()->GetLocalToWorldNormalMatrix();

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
			m_shaderProperties[0] = Renderer::DrawMesh(*m_pSphereMesh, *m_pMaterial, Float3(worldPosition), Float3x3::identity, 0.1f, receiveShadows, castShadows);
			m_shaderProperties[0].SetValue("SurfaceProperties", "diffuseColor", Float4(0.66f, 0.33f, 0.0f, 1.0f));

			m_shaderProperties[1] = Renderer::DrawMesh(*m_pArrowMesh, *m_pMaterial, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldNormal), 0.1f, receiveShadows, castShadows);
			m_shaderProperties[1].SetValue("SurfaceProperties", "diffuseColor", Float4(0.0f, 0.0f, 1.0f, 1.0f));

			m_shaderProperties[2] = Renderer::DrawMesh(*m_pArrowMesh, *m_pMaterial, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldTangent), 0.1f, receiveShadows, castShadows);
			m_shaderProperties[2].SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}
	const std::string DrawMeshData::ToString() const
	{
		return "DrawMeshData";
	}
}