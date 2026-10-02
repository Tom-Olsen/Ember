#include "materialShaderManager.h"
#include "gpuResourceFactory.h"
#include "iMaterialShader.h"
#include "materialAsset.h"
#include <stdexcept>
#include <utility>



namespace emberCore
{
	// Static members:
	bool MaterialShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, uint32_t> MaterialShaderManager::s_materialShaderIdsMap;
	std::vector<MaterialShaderManager::MaterialShaderSlot> MaterialShaderManager::s_materialShaderSlots;
	std::vector<uint32_t> MaterialShaderManager::s_freeMaterialShaderIds;



	// Managed material shader methods:
	MaterialShaderManager::ManagedMaterialShader::ManagedMaterialShader()
		: createInfo{ {}, {}, emberCommon::MaterialPass::count, "" }
	{

	}
	MaterialShaderManager::ManagedMaterialShader::ManagedMaterialShader(emberCommon::MaterialShaderCreateInfo createInfo, std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader)
		: createInfo(std::move(createInfo))
		, pMaterialShader(std::move(pMaterialShader))
	{

	}
	MaterialShaderManager::ManagedMaterialShader::~ManagedMaterialShader() = default;
	MaterialShaderManager::ManagedMaterialShader::ManagedMaterialShader(ManagedMaterialShader&&) noexcept = default;
	MaterialShaderManager::ManagedMaterialShader& MaterialShaderManager::ManagedMaterialShader::operator=(ManagedMaterialShader&&) noexcept = default;



	// Material shader slot methods:
	MaterialShaderManager::MaterialShaderSlot::MaterialShaderSlot(uint32_t generation, ManagedMaterialShader managedMaterialShader)
		: generation(generation)
		, managedMaterialShader(std::move(managedMaterialShader))
	{

	}
	MaterialShaderManager::MaterialShaderSlot::~MaterialShaderSlot() = default;
	MaterialShaderManager::MaterialShaderSlot::MaterialShaderSlot(MaterialShaderSlot&&) noexcept = default;
	MaterialShaderManager::MaterialShaderSlot& MaterialShaderManager::MaterialShaderSlot::operator=(MaterialShaderSlot&&) noexcept = default;



	// Private methods:
	// Initialization/Cleanup:
	void MaterialShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("MaterialShaderManager::Init() failed. Gpu resource factory is not initialized.");
		s_isInitialized = true;
	}
	void MaterialShaderManager::Clear()
	{
		if (!s_isInitialized)
			return;

		for (uint32_t index = 0; index < s_materialShaderSlots.size(); index++)
		{
			MaterialShaderSlot& slot = s_materialShaderSlots[index];
			if (slot.managedMaterialShader.pMaterialShader == nullptr)
				continue;

			RetireMaterialShader(std::move(slot.managedMaterialShader.pMaterialShader));
			InvalidateMaterialShaderSlot(index);
		}
		s_materialShaderIdsMap.clear();
		s_isInitialized = false;
	}



	// Creation:
	MaterialShader MaterialShaderManager::CreateMaterialShader(const emberAssetLoader::MaterialAsset& materialAsset)
	{
		if (!s_isInitialized)
			throw std::runtime_error("MaterialShaderManager::CreateMaterialShader(...) failed. Material shader manager is not initialized.");

		emberCommon::MaterialShaderCreateInfo createInfo
		{
			materialAsset.shaderStages[static_cast<size_t>(emberCommon::ShaderStage::vertex)].binaryPath,
			materialAsset.shaderStages[static_cast<size_t>(emberCommon::ShaderStage::fragment)].binaryPath,
			materialAsset.GetMaterialPass(),
			materialAsset.materialShaderName
		};

		emberCommon::MaterialShaderId materialShaderId = FindMaterialShaderId(createInfo.name);
		if (TryGetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			const emberCommon::MaterialShaderCreateInfo& existingCreateInfo = s_materialShaderSlots[materialShaderId.index].managedMaterialShader.createInfo;
			if (!(existingCreateInfo == createInfo))
				throw std::runtime_error("MaterialShaderManager::CreateMaterialShader(...) failed. MaterialShader name is already used by a different shader description: " + createInfo.name);
			return MaterialShader(materialShaderId);
		}

		std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader(GpuResourceFactory::CreateMaterialShader(createInfo));
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShaderManager::CreateMaterialShader(...) failed. Gpu resource factory returned nullptr for: " + createInfo.name);
		if (pMaterialShader->GetMaterialPass() != createInfo.materialPass)
			throw std::runtime_error("MaterialShaderManager::CreateMaterialShader(...) failed. Gpu resource factory returned a material shader with the wrong material pass: " + createInfo.name);

		return MaterialShader(AddMaterialShader(createInfo, std::move(pMaterialShader)));
	}



	// Getters:
	emberBackendInterface::IMaterialShader* MaterialShaderManager::TryGetMaterialShaderInterface(emberCommon::MaterialShaderId materialShaderId)
	{
		if (materialShaderId.index == emberCommon::invalidMaterialShaderId.index || materialShaderId.index >= s_materialShaderSlots.size())
			return nullptr;

		const MaterialShaderSlot& slot = s_materialShaderSlots[materialShaderId.index];
		if (slot.generation != materialShaderId.generation)
			return nullptr;
		return slot.managedMaterialShader.pMaterialShader.get();
	}
	const std::string* MaterialShaderManager::TryGetMaterialShaderName(emberCommon::MaterialShaderId materialShaderId)
	{
		if (TryGetMaterialShaderInterface(materialShaderId) == nullptr)
			return nullptr;
		return &s_materialShaderSlots[materialShaderId.index].managedMaterialShader.createInfo.name;
	}



	// Management:
	emberCommon::MaterialShaderId MaterialShaderManager::AddMaterialShader(const emberCommon::MaterialShaderCreateInfo& createInfo, std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader)
	{
		if (pMaterialShader == nullptr)
			throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. pMaterialShader is nullptr.");
		if (TryGetMaterialShaderInterface(FindMaterialShaderId(createInfo.name)) != nullptr)
			throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. MaterialShader already exists: " + createInfo.name);

		emberCommon::MaterialShaderId materialShaderId;
		if (s_freeMaterialShaderIds.empty())
		{
			if (s_materialShaderSlots.size() >= emberCommon::invalidMaterialShaderId.index)
				throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. Material shader id limit reached.");
			materialShaderId.index = static_cast<uint32_t>(s_materialShaderSlots.size());
			s_materialShaderSlots.emplace_back(1, ManagedMaterialShader(createInfo, std::move(pMaterialShader)));
		}
		else
		{
			materialShaderId.index = s_freeMaterialShaderIds.back();
			s_freeMaterialShaderIds.pop_back();

			MaterialShaderSlot& slot = s_materialShaderSlots[materialShaderId.index];
			slot.managedMaterialShader.createInfo = createInfo;
			slot.managedMaterialShader.pMaterialShader = std::move(pMaterialShader);
		}

		materialShaderId.generation = s_materialShaderSlots[materialShaderId.index].generation;
		s_materialShaderIdsMap[createInfo.name] = materialShaderId.index;
		return materialShaderId;
	}
	void MaterialShaderManager::RetireMaterialShader(std::unique_ptr<emberBackendInterface::IMaterialShader> pMaterialShader)
	{
		if (pMaterialShader != nullptr)
			GpuResourceFactory::RetireMaterialShader(pMaterialShader.release());
	}
	emberCommon::MaterialShaderId MaterialShaderManager::FindMaterialShaderId(const std::string& name)
	{
		std::unordered_map<std::string, uint32_t>::const_iterator iterator = s_materialShaderIdsMap.find(name);
		if (iterator == s_materialShaderIdsMap.end())
			return emberCommon::invalidMaterialShaderId;

		const uint32_t index = iterator->second;
		return emberCommon::MaterialShaderId{ index, s_materialShaderSlots[index].generation };
	}
	void MaterialShaderManager::InvalidateMaterialShaderSlot(uint32_t index)
	{
		MaterialShaderSlot& slot = s_materialShaderSlots[index];
		slot.managedMaterialShader.createInfo = { {}, {}, emberCommon::MaterialPass::count, "" };
		slot.generation++;
		if (slot.generation != emberCommon::invalidMaterialShaderId.generation)
			s_freeMaterialShaderIds.push_back(index);
	}
}