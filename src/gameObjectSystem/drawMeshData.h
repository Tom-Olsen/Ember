#ifndef __INCLUDE_GUARD_drawMeshData_h__
#define __INCLUDE_GUARD_drawMeshData_h__
#include "component.h"



class Mesh;
class Material;
class MaterialProperties;



class DrawMeshData : public Component
{
public: // Members:
	Mesh* sphereMesh;
	Mesh* arrowMesh;
	Material* material;

private: // Members:
	Mesh* mesh;
	MaterialProperties* materialProperties;

public: // Methods:
	DrawMeshData();
	~DrawMeshData();

	// Overrides:
	void Update() override;
	std::string ToString() const override;
};



#endif // __INCLUDE_GUARD_drawMeshData_h__