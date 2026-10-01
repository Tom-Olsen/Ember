#pragma once



namespace emberBackendInterface
{
	// Forward declarations:
	class ICompute;
	class IDefaultGpuResources;
	class IGpuResourceFactory;
	class IRenderer;



	class IGpuBackend
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IGpuBackend() = default;

		// Getters:
		virtual IRenderer* GetRenderer() = 0;
		virtual ICompute* GetCompute() = 0;
		virtual IGpuResourceFactory* GetGpuResourceFactory() = 0;
		virtual IDefaultGpuResources* GetDefaultGpuResources() = 0;
	};
}