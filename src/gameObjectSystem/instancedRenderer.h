#ifndef __INCLUDE_GUARD_instancedRenderer_h__
#define __INCLUDE_GUARD_instancedRenderer_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class Mesh;
	class Material;
	class ShaderProperties;



	class InstancedRenderer : public Component
	{
	private: // Members:
		uint32_t m_instanceCount;
		bool m_castShadows;
		bool m_receiveShadows;
		Mesh* m_pMesh;
		Material* m_pMaterial;
		StorageBuffer* m_pInstanceBuffer;
		std::unique_ptr<ShaderProperties> m_pShaderProperties;

	public: // Methods:
		InstancedRenderer();
		~InstancedRenderer();
		
		// Setter:
		void SetInstanceCount(uint32_t instanceCount);
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetMesh(Mesh* pMesh);
		void SetMaterial(Material* pMaterial);
		void SetInstanceBuffer(StorageBuffer* pInstanceBuffer);

		// Forward render pass getters:
		uint32_t GetInstanceCount() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		Mesh* GetMesh();
		Material* GetMaterial();
		StorageBuffer* GetInstanceBuffer();
		ShaderProperties* GetShaderProperties();

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_instancedRenderer_h__