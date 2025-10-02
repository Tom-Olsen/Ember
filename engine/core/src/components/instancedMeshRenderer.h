#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class Mesh;
	class Material;
	class ShaderProperties;



	class InstancedMeshRenderer : public Component
	{
	private: // Members:
		uint32_t m_instanceCount;
		bool m_castShadows;
		bool m_receiveShadows;
		Mesh* m_pMesh;
		Material* m_pMaterial;
		Buffer* m_pInstanceBuffer;
		ShaderProperties m_shaderProperties;

	public: // Methods:
		InstancedMeshRenderer();
		~InstancedMeshRenderer();
		
		// Setter:
		void SetInstanceCount(uint32_t instanceCount);
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetMesh(Mesh* pMesh);
		void SetMaterial(Material* pMaterial);
		void SetInstanceBuffer(Buffer* pInstanceBuffer);

		// Forward render pass getters:
		uint32_t GetInstanceCount() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		Mesh* GetMesh();
		Material* GetMaterial();
		Buffer* GetInstanceBuffer();
		ShaderProperties& GetShaderProperties();

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
	};
}