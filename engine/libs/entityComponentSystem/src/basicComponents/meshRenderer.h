#pragma once
#include "emberEngine.h"



namespace emberEcs
{
	class MeshRenderer : public Component
	{
	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
        bool m_drawLocalBounds;
        bool m_drawWorldBounds;
		Mesh* m_pMesh;
		Material m_material;
		ShaderProperties m_shaderProperties;

	public: // Methods:
		MeshRenderer();
		~MeshRenderer();

		// Setter:
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetMesh(Mesh& mesh);
		void SetMaterial(const Material& material);

		// Getters:
		Bounds GetLocalBounds() const;
		Bounds GetWorldBounds() const;
		bool HasMesh() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		Mesh& GetMesh();
		Material& GetMaterial();
		ShaderProperties& GetShaderProperties();

		// Overrides:
		void Update() override;
	};
}