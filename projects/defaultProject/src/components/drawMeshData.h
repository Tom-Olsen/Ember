#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class DrawMeshData : public emberEcs::Component
	{
	private: // Members:
		emberCore::Mesh* m_pSphereMesh;
		emberCore::Mesh* m_pArrowMesh;
		emberCore::Material m_material;
		emberCore::Mesh* m_pMesh;
		emberCore::ShaderProperties m_shaderProperties[3];

	public: // Methods:
		DrawMeshData();
		~DrawMeshData();

		// Overrides:
		void Start() override;
		void LateUpdate() override;
	};
}