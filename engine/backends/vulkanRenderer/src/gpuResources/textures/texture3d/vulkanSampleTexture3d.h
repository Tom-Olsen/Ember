#pragma once
#include "vulkanTexture3d.h"
#include "vulkanRendererExport.h"



// Forward declarations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class StagingBuffer;



	/// <summary>
	/// Texture3d specialization: <para/>
	/// -VkImageUsageFlags	= transfer src (for mipmap creation), transfer dst, sample <para/>
	/// -VkImageCreateFlags	= 0 <para/>
	/// -VkImageViewType    = 3d
	/// </summary>
	class VULKAN_RENDERER_API SampleTexture3d : public Texture3d
	{
	public: // Methods:
		// Constructor/Destructor:
		SampleTexture3d(VkFormat format, int width, int height, int depth, void* data = nullptr);
		~SampleTexture3d();

		// Non-copyable:
		SampleTexture3d(const SampleTexture3d&) = delete;
		SampleTexture3d& operator=(const SampleTexture3d&) = delete;

		// Movable:
		SampleTexture3d(SampleTexture3d&& other) noexcept;
		SampleTexture3d& operator=(SampleTexture3d&& other) noexcept;

		void SetData(void* data) override;

	private: // Methods:
		void Init(VkFormat format, int width, int height, int depth);
	};
}