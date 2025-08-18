#ifndef __INCLUDE_GUARD_meshTester_h__
#define __INCLUDE_GUARD_meshTester_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class MeshRenderer;



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
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_meshTester_h__