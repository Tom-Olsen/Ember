#ifndef __INCLUDE_GUARD_vulkanRendererBackend_compute_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_compute_h__



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



#endif // __INCLUDE_GUARD_vulkanRendererBackend_compute_h__