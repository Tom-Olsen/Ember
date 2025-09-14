#pragma once



namespace vulkanRendererBackend
{
	class Compute
	{
	private: // Members
		static bool s_isInitialized;

	public: // Methods
		static void Init();
		static void Clear();

	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		~Compute() = delete;
	};
}