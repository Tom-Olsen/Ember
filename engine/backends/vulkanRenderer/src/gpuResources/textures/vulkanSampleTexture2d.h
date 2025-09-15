#pragma once
#include "vulkanTexture2d.h"
#include "emberMath.h"



namespace vulkanRendererBackend
{
	// Forward declerations:
	class StagingBuffer;
	class TextureBatchUploader;



	/// <summary>
	/// Texture2d specialization: <para/>
	/// -VkImageUsageFlags		= transfer src (for mipmap creation), transfer dst, storage, sample <para/>
	/// -VkImageCreateFlagBits	= 0 <para/>
	/// -VkImageViewType		= 2d
	/// </summary>
	class SampleTexture2d : public Texture2d
	{
	public: // Methods:
		// Constructor/Destructor
		SampleTexture2d(const std::string& name, VkFormat format, int width, int height);
		~SampleTexture2d();

		// Construction by file:
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path);
		SampleTexture2d(const std::string& name, VkFormat format, const std::filesystem::path& path, TextureBatchUploader& batchUploader);

		// Non-copyable:
		SampleTexture2d(const SampleTexture2d&) = delete;
		SampleTexture2d& operator=(const SampleTexture2d&) = delete;

		// Movable:
		SampleTexture2d(SampleTexture2d&& other) noexcept = default;
		SampleTexture2d& operator=(SampleTexture2d&& other) noexcept = default;

		void RecordGpuCommands(VkCommandBuffer& transferCommandBuffer, VkCommandBuffer& graphicsCommandBuffer, StagingBuffer* pStagingBuffer) override;

		// Setters:
		// Idea: SetByValue in parent class? What about cubeTextures?
		// First implemet setting via vector.
		// Make setbyValue highjack the vector version?
		void ResetTexture();
		void SetByValue(uint8_t value);
		void SetByValue(int8_t value);
		void SetByValue(float value);
		void SetByValue(double value);
		void SetByValue(uint8_t value0, uint8_t value1);
		void SetByValue(int8_t value0, int8_t value1);
		void SetByValue(float value0, float value1);
		void SetByValue(uint8_t value0, uint8_t value1, uint8_t value2);
		void SetByValue(int8_t value0, int8_t value1, int8_t value2);
		void SetByValue(float value0, float value1, float value2);
		void SetByValue(uint8_t value0, uint8_t value1, uint8_t value2, uint8_t value3);
		void SetByValue(int8_t value0, int8_t value1, int8_t value2, int8_t value3);
		void SetByValue(float value0, float value1, float value2, float value3);
		void SetByArray(const std::vector<uint8_t>& values);
		void SetByArray(const std::vector<int8_t>& values);
		void SetByArray(const std::vector<float>& values);


	private: // Methods:
		void Init(StagingBuffer* pStagingBuffer);
		StagingBuffer* Load(const std::string& name, VkFormat format, const std::filesystem::path& path) override;
		StagingBuffer* Load(float value);
		StagingBuffer* Load(const Float2& value);
		StagingBuffer* Load(const Float3& value);
		StagingBuffer* Load(const Float4& value);
	};
}