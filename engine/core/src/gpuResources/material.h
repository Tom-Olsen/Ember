#pragma once
#include "materialType.h"
#include "renderQueue.h"
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
	class Material
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::IMaterial> m_pIMaterial;

	public: // Methods:
		// Constructors/Destructor:
		Material(MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
		~Material();

		// Non-copyable:
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Movable:
		Material(Material&& other) = default;
		Material& operator=(Material&& other) = default;

		// Getters:
		Type GetType() const;
		uint32_t GetRenderQueue() const;

	public: // Methods:
		// Getters:
		MaterialType GetType() const;
		uint32_t GetRenderQueue() const;
	};
}