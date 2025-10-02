#pragma once
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declarations:
	class Mesh;
	class Material;
	class ShaderProperties;



	class MeshRenderer : public Component
	{
	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
		Mesh* m_pMesh;
		Material* m_pMaterial;
		ShaderProperties m_shaderProperties;

	public: // Methods:
		MeshRenderer();
		~MeshRenderer();

		// Setter:
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetMesh(Mesh* pMesh);
		void SetMaterial(Material* pMaterial);

		// Forward render pass getters:
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		Mesh* GetMesh();
		Material* GetMaterial();
		ShaderProperties& GetShaderProperties();

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
	};
}