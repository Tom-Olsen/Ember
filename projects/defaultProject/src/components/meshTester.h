#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class MeshTester : public emberEcs::Component
	{
	private: // Members:
		std::vector<emberCore::Mesh*> m_meshes;
		emberEcs::MeshRenderer* m_pMeshRenderer;
		int index;

	public: // Methods:
		MeshTester(emberEcs::MeshRenderer* pMeshRenderer);
		~MeshTester();

		void AddMesh(emberCore::Mesh* pMesh);

		// Overrides:
		void Start() override;
		void Update() override;
	};
}