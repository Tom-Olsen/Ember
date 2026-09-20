#pragma once
#include "commonCullMode.h"
#include "commonMaterialId.h"
#include "commonMaterialPass.h"
#include "emberCoreExport.h"
#include "shader.h"
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterial;
}



namespace emberCore
{
	// Forward declarations:
	class GizmoMaterial;
	class ShadowMaterial;
	class DeferredMaterial;
	class ForwardMaterial;



	class EMBER_CORE_API Material : public Shader
	{
		// Friends:
		friend class GizmoMaterial;
		friend class ShadowMaterial;
		friend class DeferredMaterial;
		friend class ForwardMaterial;
		friend class MaterialManager;
		friend class Renderer;
		friend class CallProperties;

	private: // Members:
		emberCommon::MaterialId m_materialId;

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Copyable:
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Destruction:
		void Destroy();

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialPass GetMaterialPass() const;
		emberCommon::CullMode GetCullMode() const;
		bool IsValid() const;

		// Debugging:
		void Print() const;

	protected: // Methods:
		explicit Material(emberCommon::MaterialId materialId);
		emberBackendInterface::IMaterial* TryGetInterfaceHandle() const;
		emberBackendInterface::IMaterial* TryGetMutableInterfaceHandle() const;
		emberBackendInterface::IDescriptorSetBinding* TryGetShaderDescriptorSetBinding() const override;
	};
}