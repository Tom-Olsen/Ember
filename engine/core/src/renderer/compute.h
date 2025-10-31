#pragma once
#include "iCompute.h"
#include "commonAccessMask.h"
#include "commonRendererCreateInfo.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <memory>
#include <vector>



namespace emberEngine
{
	// Forward decleration:
	class ComputeShader;
	class Renderer;
	class ShaderProperties;



	class EMBER_CORE_API Compute
	{
	public: // Subclasses:
		class EMBER_CORE_API Async
		{
		private: // Members:
			emberBackendInterface::ICompute::IAsync* m_pIAsync; // ICompute owns the ICompute::IAsync instance.

		public: // Methods:
			// Constructor/Destructor:
			Async(emberBackendInterface::ICompute::IAsync* pIAsync);
			~Async();

			// Non-copyable:
			Async(const Async&) = delete;
			Async& operator=(const Async&) = delete;

			// Movable:
			Async(Async&& other) noexcept = default;
			Async& operator=(Async&& other) noexcept = default;

			// Dispatch logic:
			uint32_t CreateComputeSession();
			void DispatchComputeSession(uint32_t sessionID);
			bool IsFinished(uint32_t sessionID);
			void WaitForFinish(uint32_t sessionID);

			// Workload recording:
			void RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
			ShaderProperties RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount);
			void RecordBarrier(uint32_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask);
		};



		class EMBER_CORE_API Immediate
		{
		private: // Members:
			emberBackendInterface::ICompute::IImmediate* m_pIImmediate;  // ICompute owns the ICompute::IImmediate instance.

		public: // Methods:
			// Constructor/Destructor:
			Immediate(emberBackendInterface::ICompute::IImmediate* pIImmediate);
			~Immediate();

			// Non-copyable:
			Immediate(const Immediate&) = delete;
			Immediate& operator=(const Immediate&) = delete;

			// Movable:
			Immediate(Immediate&& other) noexcept = default;
			Immediate& operator=(Immediate&& other) noexcept = default;

			// Immediate dispatch call:
			void Dispatch(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
		};



		class EMBER_CORE_API PostRender
		{
		private: // Members:
			emberBackendInterface::ICompute::IPostRender* m_pIPostRender;  // ICompute owns the ICompute::IPostRender instance.

		public: // Methods:
			// Constructor/Destructor:
			PostRender(emberBackendInterface::ICompute::IPostRender* pIPostRender);
			~PostRender();

			// Non-copyable:
			PostRender(const PostRender&) = delete;
			PostRender& operator=(const PostRender&) = delete;

			// Movable:
			PostRender(PostRender&& other) noexcept = default;
			PostRender& operator=(PostRender&& other) noexcept = default;

			// Workload recording:
			void RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties);
			ShaderProperties RecordComputeShader(ComputeShader& computeShader);
		};



		class EMBER_CORE_API PreRender
		{
		private: // Members:
			emberBackendInterface::ICompute::IPreRender* m_pIPreRender;  // ICompute owns the ICompute::IPreRender instance.

		public: // Methods:
			// Constructor/Destructor:
			PreRender(emberBackendInterface::ICompute::IPreRender* pIPreRender);
			~PreRender();

			// Non-copyable:
			PreRender(const PreRender&) = delete;
			PreRender& operator=(const PreRender&) = delete;

			// Movable:
			PreRender(PreRender&& other) noexcept = default;
			PreRender& operator=(PreRender&& other) noexcept = default;

			// Workload recording:
			void RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
			ShaderProperties RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			void RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask);
		};



	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::ICompute> s_pICompute;
		static std::unique_ptr<Async> s_pAsync;
		static std::unique_ptr<Immediate> s_pImmediate;
		static std::unique_ptr<PostRender> s_pPostRender;
		static std::unique_ptr<PreRender> s_pPreRender;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::ICompute* pICompute);
		static void Clear();

		// Getters:
		static Async* GetAsync();
		static Immediate* GetImmediate();
		static PostRender* GetPostRender();
		static PreRender* GetPreRender();


	private: // Methods
		// Delete all constructors:
		Compute() = delete;
		Compute(const Compute&) = delete;
		Compute& operator=(const Compute&) = delete;
		Compute(Compute&&) = delete;
		Compute& operator=(Compute&&) = delete;
		~Compute() = delete;
	};
}