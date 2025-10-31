#pragma once
#include "commonMaterialType.h"
#include "commonRenderQueue.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <memory>
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
		friend class Renderer;
		friend class ShaderProperties;

	private: // Members:
		std::unique_ptr<emberBackendInterface::IMaterial> m_pIMaterial;
		emberBackendInterface::IMaterial* GetInterfaceHandle();

	public: // Methods:
		// Constructors/Destructor:
		Material(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other);
		Material& operator=(Material&& other);

	public: // Methods:
		// Getters:
		const std::string& GetName() const;
		emberCommon::MaterialType GetType() const;
		uint32_t GetRenderQueue() const;
	};
}