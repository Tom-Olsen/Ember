#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class DrawMeshData : public Component
	{
	private: // Members:
		Mesh* m_pSphereMesh;
		Mesh* m_pArrowMesh;
		Material* m_pMaterial;
		Mesh* m_pMesh;
		ShaderProperties m_shaderProperties[3];

	public: // Methods:
		DrawMeshData();
		~DrawMeshData();

		// Overrides:
		void Start() override;
		void LateUpdate() override;
	};
}