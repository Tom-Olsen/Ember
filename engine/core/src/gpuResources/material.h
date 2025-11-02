#pragma once
#include "commonMaterialType.h"
#include "commonRenderQueue.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class IMaterial;
}



namespace emberEngine
{
	class EMBER_CORE_API Material
	{
		// Friends:
		friend class MaterialManager;
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		emberBackendInterface::IMaterial* m_pIMaterial;

	private: // Methods:
		emberBackendInterface::IMaterial* const GetInterfaceHandle() const;
		Material(emberBackendInterface::IMaterial* pIMaterial);

	public: // Methods:
		// Constructor/Destructor:
		Material(); // for invalid materials only.
		~Material();

		// Creation/Destruction: (register/delete from MaterialManager)
		static Material Create(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		void Destroy();

		// Copyable:
		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialType GetType() const;
		uint32_t GetRenderQueue() const;
		bool IsValid() const;
	};
}