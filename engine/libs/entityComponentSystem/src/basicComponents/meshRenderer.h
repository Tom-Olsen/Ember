#pragma once
#include "commonCullMode.h"
#include "component.h"
#include "emberMath.h"
#include "material.h"
#include "mesh.h"
#include "shaderProperties.h"



namespace emberEcs
{
	class MeshRenderer : public Component
	{
	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
        emberCommon::CullMode m_cullMode;
        bool m_drawLocalBounds;
        bool m_drawWorldBounds;
		emberCore::Mesh* m_pMesh;
		emberCore::Material m_material;
		emberCore::ShaderProperties m_shaderProperties;

	public: // Methods:
		MeshRenderer();
		~MeshRenderer();

		// Setter:
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
        void SetCullMode(emberCommon::CullMode cullMode);
		void SetMesh(emberCore::Mesh& mesh);
		void SetMaterial(const emberCore::Material& material);

		// Getters:
		Bounds GetLocalBounds() const;
		Bounds GetWorldBounds() const;
		bool HasMesh() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
        emberCommon::CullMode GetCullMode() const;
		emberCore::Mesh& GetMesh();
		emberCore::Material& GetMaterial();
		emberCore::ShaderProperties& GetShaderProperties();

		// Overrides:
		void Update() override;
	};
}