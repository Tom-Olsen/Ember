#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	class MeshTester : public Component
	{
	private: // Members:
		std::vector<Mesh*> m_meshes;
		MeshRenderer* m_pMeshRenderer;
		int index;

	public: // Methods:
		MeshTester(MeshRenderer* pMeshRenderer);
		~MeshTester();

		void AddMesh(Mesh* pMesh);

		// Overrides:
		void Start() override;
		void Update() override;
	};
}