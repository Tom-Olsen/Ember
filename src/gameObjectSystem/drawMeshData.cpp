#include "drawMeshData.h"
#include "graphics.h"
#include "mesh.h"
#include "mathf.h"
#include "material.h"
#include "meshManager.h"
#include "materialManager.h"
#include "meshGenerator.h"
#include "logger.h"



// Constructor/Destructor:
DrawMeshData::DrawMeshData()
{
	sphereMesh = MeshGenerator::CubeSphere(0.5f, 1, "cubeSphere");
	arrowMesh = MeshGenerator::ArrowEdgy(Float3::forward, 0.8f, 0.1f, 0.2f, 0.2f, 4, "arrowEdgy");
	mesh = nullptr;
	material = nullptr;
	materialProperties = nullptr;
}
DrawMeshData::~DrawMeshData()
{
	delete sphereMesh;
	delete arrowMesh;
}



// Overrides:
void DrawMeshData::Update()
{
	bool receiveShadows = false;
	bool castShadows = false;

	if (sphereMesh == nullptr)
		sphereMesh = MeshManager::GetMesh("cubeSphere");
	if (arrowMesh == nullptr)
		arrowMesh = MeshManager::GetMesh("arrowEdgy");
	if (material == nullptr)
		material = MaterialManager::GetMaterial("default");
	if (mesh == nullptr)
		mesh = gameObject->GetComponent<MeshRenderer>()->mesh;

	// Transformation matrices:
	Float4x4 localToWorld = transform->GetLocalToWorldMatrix();
	Float4x4 normalMatrix = transform->GetNormalMatrix();

	for (uint32_t i = 0; i < mesh->GetVertexCount(); i++)
	{
		// Local space data:
		Float3 localPosition = mesh->GetPositions()[i] + 0.05f * mesh->GetNormals()[i];
		Float3 localNormal = mesh->GetNormals()[i];
		Float3 localTangent = mesh->GetTangents()[i];

		// World space data:
		Float3 worldPosition = Float3(localToWorld * Float4(localPosition, 1.0f));
		Float3 worldNormal = Float3(normalMatrix * Float4(localNormal, 0.0f));
		Float3 worldTangent = Float3(normalMatrix * Float4(localTangent, 0.0f));
		
		// Draw calls:
		materialProperties = Graphics::Draw(sphereMesh, material, Float3(worldPosition), Float3x3::identity, 0.1f, receiveShadows, castShadows);
		materialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.66f, 0.33f, 0.0f, 1.0f));

		materialProperties = Graphics::Draw(arrowMesh, material, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldNormal), 0.1f, receiveShadows, castShadows);
		materialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(0.0f, 0.0f, 1.0f, 1.0f));

		materialProperties = Graphics::Draw(arrowMesh, material, Float3(worldPosition), Float3x3::RotateFromTo(Float3::forward, worldTangent), 0.1f, receiveShadows, castShadows);
		materialProperties->SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}
std::string DrawMeshData::ToString() const
{
	return "DrawMeshData";
}