#pragma once
#include "emberMath.h"
#include "vulkanTexture2d.h"



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src/dst, sample <para/>
	/// -VkImageCreateFlagBits	= cube compatible <para/>
	/// -VkImageViewType		= cube
	/// -path = folder path containing 6 images, nx ny nz px pyp z
	/// </summary>
	class TextureCube : public Texture2d
	{
	public: // Methods:
		TextureCube(const std::string& name, VkFormat format, int width, int height, const std::array<void*, 6>&);
		TextureCube(const std::string& name, VkFormat format, const std::filesystem::path& path);

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// Movable:
		TextureCube(TextureCube&& other) noexcept = default;
		TextureCube& operator=(TextureCube&& other) noexcept = default;
		
		~TextureCube();
		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer);

	private: // Methods:
		void Init(StagingBuffer* pStagingBuffer);
		StagingBuffer* Upload(const std::array<void*, 6>& data);
	};
}