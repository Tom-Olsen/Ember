#pragma once
#include "iComputeShaderManager.h"
#include "commonResourceAccessRights.h"
#include "vulkanRendererExport.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



// Forward declarations:
namespace emberAssetLoader
{
	struct ComputeShaderAsset;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class ComputeShader;



	class VULKAN_RENDERER_API ComputeShaderManager : public emberBackendInterface::IComputeShaderManager
	{
	private: // Structs:
		struct ManagedComputeShader
		{
			std::string name;
			emberCommon::ResourceAccessRights accessRights;
			std::unique_ptr<ComputeShader> pComputeShader;

			// Constructor:
			ManagedComputeShader(std::string name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<ComputeShader> pComputeShader);

			// Non-copyable:
			ManagedComputeShader(const ManagedComputeShader& other) = delete;
			ManagedComputeShader& operator=(const ManagedComputeShader& other) = delete;

			// Movable:
			ManagedComputeShader(ManagedComputeShader&& other) noexcept = default;
			ManagedComputeShader& operator=(ManagedComputeShader&& other) noexcept = default;
		};
		struct ComputeShaderSlot
		{
			uint32_t generation;
			ManagedComputeShader managedComputeShader;

			// Constructor:
			ComputeShaderSlot(uint32_t generation, ManagedComputeShader managedComputeShader);

			// Non-copyable:
			ComputeShaderSlot(const ComputeShaderSlot& other) = delete;
			ComputeShaderSlot& operator=(const ComputeShaderSlot& other) = delete;
			
			// Movable:
			ComputeShaderSlot(ComputeShaderSlot&& other) noexcept = default;
			ComputeShaderSlot& operator=(ComputeShaderSlot&& other) noexcept = default;
		};

	private: // Members:
		std::unordered_map<std::string, uint32_t> m_computeShaderIdsMap;
		std::vector<ComputeShaderSlot> m_computeShaderSlots;
		std::vector<uint32_t> m_freeComputeShaderIds;

	public: // Methods:
		// Constructor/Destructor:
		ComputeShaderManager();
		~ComputeShaderManager() override;

		// Non-copyable:
		ComputeShaderManager(const ComputeShaderManager& other) = delete;
		ComputeShaderManager& operator=(const ComputeShaderManager& other) = delete;
		
		// Non-movable:
		ComputeShaderManager(ComputeShaderManager&& other) = delete;
		ComputeShaderManager& operator=(ComputeShaderManager&& other) = delete;

		// Creation:
		void InitializeDefaultComputeShaders() override;
		emberCommon::ComputeShaderId CreateComputeShader(const emberAssetLoader::ComputeShaderAsset& computeShaderAsset) override;

		// Getters:
		emberCommon::ComputeShaderId TryGetComputeShaderId(const std::string& name) const override;
		emberBackendInterface::IComputeShader* TryGetComputeShader(emberCommon::ComputeShaderId computeShaderId) const override;
		const std::string* TryGetComputeShaderName(emberCommon::ComputeShaderId computeShaderId) const override;
		bool IsComputeShaderMutable(emberCommon::ComputeShaderId computeShaderId) const override;

		// Debugging:
		void Print() const override;

	private: // Methods:
		// Management:
		emberCommon::ComputeShaderId AddComputeShader(const std::string& name, const emberCommon::ResourceAccessRights& accessRights, std::unique_ptr<ComputeShader> pComputeShader);
		void Clear();
		void DestroyComputeShader(std::unique_ptr<ComputeShader> pComputeShader);
		emberCommon::ComputeShaderId FindComputeShaderId(const std::string& name) const;
		ComputeShader* GetComputeShader(const std::string& name) const;
	};
}