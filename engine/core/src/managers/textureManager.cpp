#include "textureManager.h"
#include "gpuResourceFactory.h"
#include "imageLoader.h"
#include "iRenderer.h"
#include "iTexture.h"
#include "logger.h"
#include "renderer.h"
#include "texture2d.h"
#include "texture3d.h"
#include "textureAssetLoader.h"
#include "textureCube.h"
#include <algorithm>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>



namespace emberCore
{
	// Static members:
	const std::string TextureManager::s_finalRenderTextureName = "Renderer_FinalRenderTexture";
	const std::string TextureManager::s_gizmoTextureName = "Renderer_GizmoTexture";
	bool TextureManager::s_isInitialized = false;
	emberDataStructures::NamedSlotMap<emberCommon::TextureId, TextureManager::ManagedTexture> TextureManager::s_textureSlotMap;



	// Managed texture methods:
	TextureManager::ManagedTexture::ManagedTexture(bool ownedByTextureManager, emberCommon::TextureType textureType, emberBackendInterface::ITexture* pITexture)
		: ownedByTextureManager(ownedByTextureManager)
		, textureType(textureType)
		, pITexture(pITexture)
	{

	}
	TextureManager::ManagedTexture::~ManagedTexture() = default;
	TextureManager::ManagedTexture::ManagedTexture(ManagedTexture&&) noexcept = default;
	TextureManager::ManagedTexture& TextureManager::ManagedTexture::operator=(ManagedTexture&&) noexcept = default;



	// Public methods:
	// Asset loading:
	void TextureManager::LoadTextureAssets(const std::filesystem::path& directoryPath)
	{
		// Error handling:
		if (!s_isInitialized)
			throw std::runtime_error("TextureManager::LoadTextureAssets(...) failed. Texture manager is not initialized.");
		if (!std::filesystem::is_directory(directoryPath))
			throw std::runtime_error("TextureManager::LoadTextureAssets(...) failed. Directory does not exist: " + directoryPath.string());

		// Collect asset paths:
		std::vector<std::filesystem::path> assetPaths;
		for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_regular_file() && directoryEntry.path().filename().string().ends_with(".textureAsset.json"))
				assetPaths.push_back(directoryEntry.path());
		}
		std::sort(assetPaths.begin(), assetPaths.end());

		// Load and validate texture assets:
		std::vector<emberAssetLoader::TextureAsset> textureAssets;
		textureAssets.reserve(assetPaths.size());
		std::unordered_set<std::string> textureNames;
		for (const std::filesystem::path& assetPath : assetPaths)
		{
			emberAssetLoader::TextureAsset textureAsset = emberAssetLoader::TextureAssetLoader::Load(assetPath);
			if (textureAsset.textureName == s_finalRenderTextureName || textureAsset.textureName == s_gizmoTextureName)
				throw std::runtime_error("TextureManager::LoadTextureAssets(...) failed. Texture name is reserved for the renderer: " + textureAsset.textureName);
			if (s_textureSlotMap.Find(textureAsset.textureName) != emberCommon::invalidTextureId || !textureNames.insert(textureAsset.textureName).second)
				throw std::runtime_error("TextureManager::LoadTextureAssets(...) failed. Texture name already exists: " + textureAsset.textureName);
			textureAssets.push_back(std::move(textureAsset));
		}

		// Create textures:
		std::vector<emberCommon::TextureId> textureIds;
		textureIds.reserve(textureAssets.size());
		try
		{
			for (const emberAssetLoader::TextureAsset& textureAsset : textureAssets)
				textureIds.push_back(CreateTexture(textureAsset));
		}
		catch (...)
		{
			for (emberCommon::TextureId textureId : textureIds)
				DeleteTexture(textureId);
			throw;
		}
	}



	// Getters:
	Texture TextureManager::GetTexture(const std::string& name)
	{
		emberCommon::TextureId textureId = s_textureSlotMap.Find(name);
		if (TryGetTextureInterface(textureId) == nullptr)
			throw std::runtime_error("TextureManager::GetTexture(...) failed. Texture not found: " + name);
		return Texture(textureId);
	}
	template<typename T>
	T TextureManager::GetTexture(const std::string& name)
	{
		static_assert(std::is_same_v<T, Texture2d> || std::is_same_v<T, Texture3d> || std::is_same_v<T, TextureCube>, "Unsupported texture type.");
		emberCommon::TextureType expectedTextureType = emberCommon::TextureType::count;
		if constexpr (std::is_same_v<T, Texture2d>)
			expectedTextureType = emberCommon::TextureType::texture2d;
		if constexpr (std::is_same_v<T, Texture3d>)
			expectedTextureType = emberCommon::TextureType::texture3d;
		if constexpr (std::is_same_v<T, TextureCube>)
			expectedTextureType = emberCommon::TextureType::textureCube;

		Texture texture = GetTexture(name);
		if (GetTextureType(texture.m_textureId) != expectedTextureType)
			throw std::runtime_error("TextureManager::GetTexture<T>(...) failed. Texture type does not match the requested type: " + name);
		return T(texture);
	}
	Texture TextureManager::TryGetTexture(const std::string& name)
	{
		emberCommon::TextureId textureId = s_textureSlotMap.Find(name);
		if (TryGetTextureInterface(textureId) != nullptr)
			return Texture(textureId);
		LOG_WARN("TextureManager::TryGetTexture(...) failed. Texture '{}' not found.", name);
		return Texture();
	}



	// Deleter:
	void TextureManager::DeleteTexture(const std::string& name)
	{
		emberCommon::TextureId textureId = s_textureSlotMap.Find(name);
		if (textureId == emberCommon::invalidTextureId)
		{
			LOG_WARN("TextureManager::DeleteTexture(...) failed. Texture '{}' not found.", name);
			return;
		}
		DeleteTexture(textureId);
	}



	// Debugging:
	void TextureManager::Print()
	{
		LOG_TRACE("Names of all managed textures:");
		for (emberCommon::TextureId textureId : s_textureSlotMap.GetActiveIds())
			LOG_TRACE("  {}", GetTextureName(textureId));
	}



	// Private methods:
	// Initialization/Cleanup:
	void TextureManager::Init()
	{
		if (s_isInitialized)
			return;
		if (!GpuResourceFactory::s_isInitialized)
			throw std::runtime_error("TextureManager::Init() failed. Gpu resource factory is not initialized.");
		s_isInitialized = true;

		try
		{
			// Register non-owning entries for renderer owned textures.
			// TryGetTextureInterface resolves them through the renderer using the ownership flag and name.
			if (s_textureSlotMap.Add(s_finalRenderTextureName, ManagedTexture(false, emberCommon::TextureType::texture2d, nullptr)) == emberCommon::invalidTextureId)
				throw std::runtime_error("TextureManager::Init() failed. Could not register renderer texture: " + s_finalRenderTextureName);
			if (s_textureSlotMap.Add(s_gizmoTextureName, ManagedTexture(false, emberCommon::TextureType::texture2d, nullptr)) == emberCommon::invalidTextureId)
				throw std::runtime_error("TextureManager::Init() failed. Could not register renderer texture: " + s_gizmoTextureName);

			LoadTextureAssets(std::filesystem::path(ENGINE_RESOURCES_DIR) / "textureAssets");
		}
		catch (...)
		{
			Clear();
			throw;
		}
	}
	void TextureManager::Clear()
	{
		for (emberCommon::TextureId textureId : s_textureSlotMap.GetActiveIds())
		{
			std::optional<ManagedTexture> managedTexture = s_textureSlotMap.Remove(textureId);
			RetireTexture(managedTexture->pITexture.release());
		}
		s_isInitialized = false;
	}



	// Asset creation:
	emberCommon::TextureId TextureManager::CreateTexture(const emberAssetLoader::TextureAsset& textureAsset)
	{
		// Load image data:
		emberAssetLoader::ImageData imageData;
		switch (textureAsset.textureType)
		{
			case emberCommon::TextureType::texture2d:
				imageData = emberAssetLoader::ImageLoader::LoadFile(textureAsset.sourcePath, textureAsset.format.channels, textureAsset.flipImage);
				break;
			case emberCommon::TextureType::textureCube:
				imageData = emberAssetLoader::ImageLoader::LoadCubeFiles(textureAsset.sourcePath, textureAsset.format.channels, textureAsset.flipImage);
				break;
			default:
				throw std::runtime_error("TextureManager::CreateTexture(...) failed. Unsupported file-based texture type for: " + textureAsset.textureName);
		}

		// Validate read data:
		if (imageData.width <= 0 || imageData.height <= 0 || imageData.channels != textureAsset.format.channels || imageData.pixels.empty())
			throw std::runtime_error("TextureManager::CreateTexture(...) failed. Invalid image data for: " + textureAsset.textureName);
		if (textureAsset.textureType == emberCommon::TextureType::textureCube && imageData.width != imageData.height)
			throw std::runtime_error("TextureManager::CreateTexture(...) failed. Cubemap faces must be square for: " + textureAsset.textureName);

		// Create texture from imageData:
		emberBackendInterface::ITexture* pITexture = nullptr;
		if (textureAsset.textureType == emberCommon::TextureType::texture2d)
			pITexture = GpuResourceFactory::CreateTexture2d(imageData.width, imageData.height, textureAsset.format, textureAsset.usage, emberCommon::TextureImageCountMode::single, imageData.pixels.data());
		else
			pITexture = GpuResourceFactory::CreateTextureCube(imageData.width, imageData.height, textureAsset.format, textureAsset.usage, emberCommon::TextureImageCountMode::single, imageData.pixels.data());
		return AddTexture(textureAsset.textureName, pITexture, textureAsset.textureType);
	}



	// Management:
	emberCommon::TextureId TextureManager::AddTexture(const std::string& name, emberBackendInterface::ITexture* pITexture, emberCommon::TextureType textureType)
	{
		// Error handling:
		if (!s_isInitialized || pITexture == nullptr)
			throw std::runtime_error("TextureManager::AddTexture(...) failed. Texture manager is not initialized or pITexture is nullptr.");
		if (name == s_finalRenderTextureName || name == s_gizmoTextureName)
			throw std::runtime_error("TextureManager::AddTexture(...) failed. Texture name is reserved for the renderer: " + name);
		if (s_textureSlotMap.Find(name) != emberCommon::invalidTextureId)
			throw std::runtime_error("TextureManager::AddTexture(...) failed. Texture name already exists: " + name);

		// Add texture:
		ManagedTexture managedTexture(true, textureType, pITexture);
		pITexture->SetDebugName(name);
		emberCommon::TextureId textureId = s_textureSlotMap.Add(name, std::move(managedTexture));
		if (textureId == emberCommon::invalidTextureId)
			throw std::runtime_error("TextureManager::AddTexture(...) failed. Could not add texture: " + name);
		return textureId;
	}
	emberBackendInterface::ITexture* TextureManager::TryGetTextureInterface(emberCommon::TextureId textureId)
	{
		// Get managed texture:
		const ManagedTexture* pManagedTexture = s_textureSlotMap.TryGetValue(textureId);
		if (pManagedTexture == nullptr)
			return nullptr;
		if (pManagedTexture->ownedByTextureManager)
			return pManagedTexture->pITexture.get();

		// Get renderer owned texture:
		std::string name = GetTextureName(textureId);
		emberBackendInterface::IRenderer* pIRenderer = Renderer::GetInterfaceHandle();
		if (pIRenderer == nullptr)
			return nullptr;
		if (name == s_finalRenderTextureName)
			return pIRenderer->GetFinalRenderTexture();
		if (name == s_gizmoTextureName)
			return pIRenderer->GetGizmoTexture();
		LOG_WARN("TextureManager::TryGetTextureInterface(...) failed. Unknown renderer texture: {}", name);
		return nullptr;
	}
	std::string TextureManager::GetTextureName(emberCommon::TextureId textureId)
	{
		std::optional<std::string> name = s_textureSlotMap.TryGetName(textureId);
		if (!name)
			throw std::runtime_error("TextureManager::GetTextureName(...) failed. Texture is invalid or expired.");
		return std::move(*name);
	}
	emberCommon::TextureType TextureManager::GetTextureType(emberCommon::TextureId textureId)
	{
		const ManagedTexture* pManagedTexture = s_textureSlotMap.TryGetValue(textureId);
		if (pManagedTexture == nullptr)
			throw std::runtime_error("TextureManager::GetTextureType(...) failed. Texture is invalid or expired.");
		if (pManagedTexture->textureType < emberCommon::TextureType::texture1d || pManagedTexture->textureType >= emberCommon::TextureType::count)
			throw std::runtime_error("TextureManager::GetTextureType(...) failed. Texture type is invalid.");
		return pManagedTexture->textureType;
	}
	void TextureManager::DeleteTexture(emberCommon::TextureId textureId)
	{
		ManagedTexture* pManagedTexture = s_textureSlotMap.TryGetValue(textureId);
		if (pManagedTexture == nullptr)
			return;
		if (!pManagedTexture->ownedByTextureManager)
		{
			LOG_WARN("TextureManager::DeleteTexture(...) failed. Renderer textures are not owned by the texture manager and can't be deleted.");
			return;
		}
		std::optional<ManagedTexture> managedTexture = s_textureSlotMap.Remove(textureId);
		RetireTexture(managedTexture->pITexture.release());
	}
	void TextureManager::RetireTexture(emberBackendInterface::ITexture* pITexture)
	{
		if (pITexture != nullptr)
			GpuResourceFactory::RetireTexture(pITexture);
	}
	std::string TextureManager::CreateUniqueTextureName(const std::string& name)
	{
		static uint64_t uniqueTextureNameCounter = 0;
		if (name != s_finalRenderTextureName && name != s_gizmoTextureName && s_textureSlotMap.Find(name) == emberCommon::invalidTextureId)
			return name;

		std::string uniqueName = name + "#owned" + std::to_string(uniqueTextureNameCounter++);
		while (s_textureSlotMap.Find(uniqueName) != emberCommon::invalidTextureId)
			uniqueName = name + "#owned" + std::to_string(uniqueTextureNameCounter++);
		return uniqueName;
	}



	// Explicit template instantiations:
	//template Texture1d TextureManager::GetTexture<Texture1d>(const std::string& name);
	template Texture2d TextureManager::GetTexture<Texture2d>(const std::string& name);
	template Texture3d TextureManager::GetTexture<Texture3d>(const std::string& name);
	template TextureCube TextureManager::GetTexture<TextureCube>(const std::string& name);
}