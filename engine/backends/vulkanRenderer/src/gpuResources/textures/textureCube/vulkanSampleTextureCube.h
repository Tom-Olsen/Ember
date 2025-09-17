#pragma once
#include "vulkanTextureCube.h"
#include <filesystem>



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// TextureCube specialization: <para/>
	/// -VkImageUsageFlags		= transfer src/dst, sample <para/>
	/// -VkImageCreateFlagBits	= cube compatible <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTextureCube : public TextureCube
	{
	public: // Methods:
		// Constructor/Destructor:
		SampleTextureCube(const std::string& name, VkFormat format, int width, int height, const std::array<void*, 6>&);
		SampleTextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path);
		~SampleTextureCube();

		// Non-copyable:
		SampleTextureCube(const SampleTextureCube&) = delete;
		SampleTextureCube& operator=(const SampleTextureCube&) = delete;

		// Movable:
		SampleTextureCube(SampleTextureCube&& other) noexcept = default;
		SampleTextureCube& operator=(SampleTextureCube&& other) noexcept = default;
		
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer);

	private: // Methods:
		StagingBuffer* Upload(const std::array<void*, 6>& data);
		void Init(StagingBuffer* pStagingBuffer);
	};
}