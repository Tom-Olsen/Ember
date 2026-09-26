#include "drawMeshData.h"
#include "meshRenderer.h"
using namespace emberCore;
using namespace emberEcs;



namespace emberEngine
{
	// Constructor/Destructor:
	DrawMeshData::DrawMeshData()
	{
		m_pSphereMesh = MeshManager::TryGetMesh("cubeSphere");
		m_pArrowMesh = MeshManager::TryGetMesh("arrowFlat");
		m_pMesh = nullptr;
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
		m_material = MaterialManager::TryGetMaterial("pbrDeferredGeometryMaterial");
		MeshRenderer* pMeshRenderer = GetComponent<MeshRenderer>();
		m_pMesh = &pMeshRenderer->GetMesh();
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
			DrawData sphereDrawData(Float4x4::TRS(worldPosition, Float3x3::identity, Float3(0.1f)), *m_pSphereMesh, m_material, receiveShadows, castShadows);
			m_callProperties[0] = Renderer::DrawMesh(sphereDrawData);
			m_callProperties[0].SetValue("SurfaceProperties", "surface_diffuseColor", Float4(0.66f, 0.33f, 0.0f, 1.0f));

			DrawData normalDrawData(Float4x4::TRS(worldPosition, Float3x3::RotateFromTo(Float3::forward, worldNormal), Float3(0.1f)), *m_pArrowMesh, m_material, receiveShadows, castShadows);
			m_callProperties[1] = Renderer::DrawMesh(normalDrawData);
			m_callProperties[1].SetValue("SurfaceProperties", "surface_diffuseColor", Float4(0.0f, 0.0f, 1.0f, 1.0f));

			DrawData tangentDrawData(Float4x4::TRS(worldPosition, Float3x3::RotateFromTo(Float3::forward, worldTangent), Float3(0.1f)), *m_pArrowMesh, m_material, receiveShadows, castShadows);
			m_callProperties[2] = Renderer::DrawMesh(tangentDrawData);
			m_callProperties[2].SetValue("SurfaceProperties", "surface_diffuseColor", Float4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}