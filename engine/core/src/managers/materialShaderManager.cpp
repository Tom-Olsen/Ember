#include "materialShaderManager.h"
#include "iMaterialShader.h"
#include "logger.h"
#include "materialManager.h"
#include "renderer.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	bool MaterialShaderManager::s_isInitialized = false;
	std::unordered_map<std::string, uint32_t> MaterialShaderManager::s_materialShaderIdsMap;
	std::vector<MaterialShaderManager::MaterialShaderSlot> MaterialShaderManager::s_materialShaderSlots;
	std::vector<uint32_t> MaterialShaderManager::s_freeMaterialShaderIds;



	// Initialization/Cleanup:
	void MaterialShaderManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void MaterialShaderManager::Clear()
	{
		for (uint32_t index = 0; index < s_materialShaderSlots.size(); index++)
		{
			const MaterialShaderSlot& slot = s_materialShaderSlots[index];
			if (slot.materialShader.pIMaterialShader != nullptr && MaterialManager::MaterialShaderInUse({ index, slot.generation }))
				throw std::runtime_error("MaterialShaderManager::Clear() failed. MaterialShader is still used by at least one material: " + slot.materialShader.name);
		}

		for (uint32_t index = 0; index < s_materialShaderSlots.size(); index++)
		{
			MaterialShaderSlot& slot = s_materialShaderSlots[index];
			if (slot.materialShader.pIMaterialShader == nullptr)
				continue;

			Renderer::DestroyMaterialShader(slot.materialShader.pIMaterialShader.release());
			slot.materialShader.name.clear();
			slot.generation++;
			s_freeMaterialShaderIds.push_back(index);
		}
		s_materialShaderIdsMap.clear();
		s_isInitialized = false;
	}



	// Creators:
	MaterialShader MaterialShaderManager::CreateGizmoMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::gizmo)
				throw std::runtime_error("MaterialShaderManager::CreateGizmoMaterialShader(...) failed. Existing MaterialShader is not a gizmo shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateGizmoMaterialShader(vertexSpv, fragmentSpv, name));
	}
	MaterialShader MaterialShaderManager::CreateShadowMaterialShader(const std::filesystem::path& vertexSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::shadow)
				throw std::runtime_error("MaterialShaderManager::CreateShadowMaterialShader(...) failed. Existing MaterialShader is not a shadow shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateShadowMaterialShader(vertexSpv, name));
	}
	MaterialShader MaterialShaderManager::CreateDeferredGeometryMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::deferredGeometry)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredGeometryMaterialShader(...) failed. Existing MaterialShader is not a deferred geometry shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateDeferredGeometryMaterialShader(vertexSpv, fragmentSpv, name));
	}
	MaterialShader MaterialShaderManager::CreateForwardMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::forward)
				throw std::runtime_error("MaterialShaderManager::CreateForwardMaterialShader(...) failed. Existing MaterialShader is not a forward shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateForwardMaterialShader(vertexSpv, fragmentSpv, name));
	}



	// Getters:
	MaterialShader MaterialShaderManager::GetMaterialShader(const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) == nullptr)
		{
			LOG_WARN("MaterialShaderManager::GetMaterialShader(...) failed. MaterialShader '{}' not found or expired.", name);
			return MaterialShader();
		}
		return MaterialShader{ materialShaderId };
	}



	// Deleter:
	void MaterialShaderManager::DeleteMaterialShader(const std::string& name)
	{
		DeleteMaterialShader(GetMaterialShaderId(name));
	}



	// Debugging:
	void MaterialShaderManager::Print()
	{
		LOG_TRACE("MaterialShaderManager contents:");
		for (const auto& [name, index] : s_materialShaderIdsMap)
			LOG_TRACE("  {}: index {}, generation {}", name, index, s_materialShaderSlots[index].generation);
	}



	// Private methods:
	// Creators:
	MaterialShader MaterialShaderManager::CreateOutlineMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::outline)
				throw std::runtime_error("MaterialShaderManager::CreateOutlineMaterialShader(...) failed. Existing MaterialShader is not an outline shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateOutlineMaterialShader(vertexSpv, fragmentSpv, name));
	}
	MaterialShader MaterialShaderManager::CreateDeferredLightingMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::deferredLighting)
				throw std::runtime_error("MaterialShaderManager::CreateDeferredLightingMaterialShader(...) failed. Existing MaterialShader is not a deferred lighting shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreateDeferredLightingMaterialShader(vertexSpv, fragmentSpv, name));
	}
	MaterialShader MaterialShaderManager::CreatePresentMaterialShader(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& name)
	{
		MaterialShaderId materialShaderId = GetMaterialShaderId(name);
		if (GetMaterialShaderInterface(materialShaderId) != nullptr)
		{
			MaterialShader materialShader{ materialShaderId };
			if (materialShader.GetMaterialPass() != emberCommon::MaterialPass::present)
				throw std::runtime_error("MaterialShaderManager::CreatePresentMaterialShader(...) failed. Existing MaterialShader is not a present shader: " + name);
			return materialShader;
		}

		return AddMaterialShader(name, Renderer::CreatePresentMaterialShader(vertexSpv, fragmentSpv, name));
	}



	// Getters:
	MaterialShaderId MaterialShaderManager::GetMaterialShaderId(const std::string& name)
	{
		auto it = s_materialShaderIdsMap.find(name);
		if (it == s_materialShaderIdsMap.end())
			return invalidMaterialShaderId;

		const uint32_t index = it->second;
		return MaterialShaderId{ index, s_materialShaderSlots[index].generation };
	}
	emberBackendInterface::IMaterialShader* MaterialShaderManager::GetMaterialShaderInterface(MaterialShaderId materialShaderId)
	{
		if (materialShaderId.index == invalidMaterialShaderId.index || materialShaderId.index >= s_materialShaderSlots.size())
			return nullptr;

		MaterialShaderSlot& slot = s_materialShaderSlots[materialShaderId.index];
		if (slot.generation != materialShaderId.generation)
			return nullptr;
		return slot.materialShader.pIMaterialShader.get();
	}
	const std::string* MaterialShaderManager::GetMaterialShaderName(MaterialShaderId materialShaderId)
	{
		if (GetMaterialShaderInterface(materialShaderId) == nullptr)
			return nullptr;
		return &s_materialShaderSlots[materialShaderId.index].materialShader.name;
	}



	// Add/Delete material shader:
	MaterialShader MaterialShaderManager::AddMaterialShader(const std::string& name, emberBackendInterface::IMaterialShader* pIMaterialShader)
	{
		if (pIMaterialShader == nullptr)
			throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. pIMaterialShader is nullptr.");

		MaterialShaderId materialShaderId;
		if (s_freeMaterialShaderIds.empty())
		{
			if (s_materialShaderSlots.size() >= invalidMaterialShaderId.index)
			{
				Renderer::DestroyMaterialShader(pIMaterialShader);
				throw std::runtime_error("MaterialShaderManager::AddMaterialShader(...) failed. MaterialShader id limit reached.");
			}
			materialShaderId.index = static_cast<uint32_t>(s_materialShaderSlots.size());
			s_materialShaderSlots.push_back({ 1, ManagedMaterialShader{ name, std::unique_ptr<emberBackendInterface::IMaterialShader>(pIMaterialShader) } });
		}
		else
		{
			materialShaderId.index = s_freeMaterialShaderIds.back();
			s_freeMaterialShaderIds.pop_back();

			MaterialShaderSlot& slot = s_materialShaderSlots[materialShaderId.index];
			slot.materialShader.name = name;
			slot.materialShader.pIMaterialShader.reset(pIMaterialShader);
		}

		materialShaderId.generation = s_materialShaderSlots[materialShaderId.index].generation;
		s_materialShaderIdsMap[name] = materialShaderId.index;
		return MaterialShader{ materialShaderId };
	}
	void MaterialShaderManager::DeleteMaterialShader(MaterialShaderId materialShaderId)
	{
		if (GetMaterialShaderInterface(materialShaderId) == nullptr)
			return;

		MaterialShaderSlot& slot = s_materialShaderSlots[materialShaderId.index];
		if (MaterialManager::MaterialShaderInUse(materialShaderId))
			throw std::runtime_error("MaterialShaderManager::DeleteMaterialShader(...) failed. MaterialShader is still used by at least one material: " + slot.materialShader.name);

		s_materialShaderIdsMap.erase(slot.materialShader.name);
		Renderer::DestroyMaterialShader(slot.materialShader.pIMaterialShader.release());
		slot.materialShader.name.clear();
		slot.generation++;
		s_freeMaterialShaderIds.push_back(materialShaderId.index);
	}
}