#pragma once


// Forward declaration:
typedef struct VkSampler_T* VkSampler;



namespace vulkanRendererBackend
{
	class Sampler
	{
	protected: // Members:
		VkSampler m_sampler;

	public: // Methods:
		Sampler();
		~Sampler();

		// Non-copyable:
		Sampler(const Sampler&) = delete;
		Sampler& operator=(const Sampler&) = delete;

		// Movable:
		Sampler(Sampler&& other) noexcept;
		Sampler& operator=(Sampler&& other) noexcept;

		// Getters:
		VkSampler& GetVkSampler();

	private: // Methods:
		void Cleanup();
		void MoveFrom(Sampler& other) noexcept;
	};
}