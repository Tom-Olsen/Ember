#ifndef __INCLUDE_GUARD_graphics_h__
#define __INCLUDE_GUARD_graphics_h__
#include <vector>
#include "mathf.h"



struct VulkanContext;
class Mesh;
class Material;
class MaterialProperties;
class MeshRenderer;



class Graphics
{
public: // Members

private: // Members
    static uint32_t s_drawIndex;
    static bool s_isInitialized;
    static std::vector<MeshRenderer*> s_meshRenderers;

public: // Methods
    static void Init();
    static void Clear();

    // Draw calls:
    static MaterialProperties* Draw(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix = Float3x3::identity, float scale = 1.0f, bool receiveShadows = true, bool castShadows = true);
    static MaterialProperties* Draw(Mesh* pMesh, Material* pMaterial, Float3 position, Float3x3 rotationMatrix = Float3x3::identity, Float3 scale = Float3::one, bool receiveShadows = true, bool castShadows = true);
    static MaterialProperties* Draw(Mesh* pMesh, Material* pMaterial, Float4x4 localToWorldMatrix, bool receiveShadows = true, bool castShadows = true);
	static void ResetDrawCalls();

    // Getters:
    static std::vector<MeshRenderer*>* GetSortedMeshRenderers();

private: //const Methods
    // Delete all constructors:
    Graphics() = delete;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics() = delete;
};




#endif // __INCLUDE_GUARD_graphics_h__