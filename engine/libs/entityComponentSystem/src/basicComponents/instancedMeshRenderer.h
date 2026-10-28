#pragma once
#include "component.h"
#include "callProperties.h"
#include "commonCullMode.h"
#include "instanceBuffer.h"
#include "material.h"
#include <cstdint>



// Forward declarations:
namespace emberCore
{
	class Mesh;
}



namespace emberEcs
{
	class InstancedMeshRenderer : public Component
	{
	private: // Members:
		bool m_castShadows;
		bool m_receiveShadows;
		emberCommon::CullMode m_cullMode;
		emberCore::Mesh* m_pMesh;
		emberCore::Material m_material;
		emberCore::InstanceBuffer m_instanceBuffer;
		emberCore::CallProperties m_callProperties;

	public: // Methods:
		// Constructor/Destructor:
		explicit InstancedMeshRenderer(uint32_t instanceCount);
		~InstancedMeshRenderer();
		
		// Setter:
		void SetInstanceCount(uint32_t instanceCount);
		void SetCastShadows(bool castShadows);
		void SetReceiveShadows(bool receiveShadows);
		void SetCullMode(emberCommon::CullMode cullMode);
		void SetMesh(emberCore::Mesh& mesh);
		void SetMaterial(const emberCore::Material& material);

		// Getters:
		uint32_t GetInstanceCount() const;
		bool GetCastShadows() const;
		bool GetReceiveShadows() const;
		emberCommon::CullMode GetCullMode() const;
		emberCore::Mesh& GetMesh();
		emberCore::Material& GetMaterial();
		emberCore::InstanceBuffer& GetInstanceBuffer();
		emberCore::CallProperties& GetCallProperties();

		// Overrides:
		void Update() override;
	};
}