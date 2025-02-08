#ifndef __INCLUDE_GUARD_drawMeshData_h__
#define __INCLUDE_GUARD_drawMeshData_h__
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
		MaterialProperties* m_pMaterialProperties;

	public: // Methods:
		DrawMeshData();
		~DrawMeshData();

		// Overrides:
		void Start() override;
		void LateUpdate() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_drawMeshData_h__