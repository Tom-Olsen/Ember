#pragma once
#include "callProperties.h"
#include "commonCullMode.h"
#include "component.h"
#include "emberMath.h"
#include "material.h"
#include "mesh.h"
#include <cstdint>
#include <vector>



namespace emberEcs
{
	class MeshRenderer : public Component
	{
	private: // Structs:
		struct SubmeshProperties
		{
		public: // Members:
			emberCore::Material material;
			emberCore::CallProperties callProperties;
		};

	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
		emberCommon::CullMode m_cullMode;
        bool m_drawLocalBounds;
        bool m_drawWorldBounds;

		emberCore::Mesh* m_pMesh;
		mutable std::vector<SubmeshProperties> m_submeshProperties;
        
		bool m_raycastEnabled;

	public: // Methods:
		// Constructor/Destructor:
		MeshRenderer();
		~MeshRenderer();

		// Setter:
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetMesh(emberCore::Mesh& mesh);
		void SetMaterial(const emberCore::Material& material, uint32_t submeshIndex = 0);
		void SetMaterials(const std::vector<emberCore::Material>& materials);
		void SetRaycastEnabled(bool raycastEnabled);

		// Getters:
		Bounds GetLocalBounds() const;
		Bounds GetWorldBounds() const;
		bool HasMesh() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		emberCommon::CullMode GetCullMode() const;
		emberCore::Mesh& GetMesh();
		emberCore::Material GetMaterial(uint32_t submeshIndex = 0) const;
		std::vector<emberCore::Material> GetMaterials() const;
		emberCore::CallProperties& GetCallProperties(uint32_t submeshIndex = 0);
		bool GetRaycastEnabled() const;

		// Overrides:
		void Update() override;

	private: // Methods:
		// Submesh properties:
		void SyncSubmeshProperties() const;
		void AssignMaterial(uint32_t submeshIndex, const emberCore::Material& material);
	};
}