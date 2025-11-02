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
			static emberBackendInterface::ICompute::IAsync* m_pIAsync;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IAsync* pIAsync);
			static void Clear();

			// Dispatch logic:
			static uint32_t CreateComputeSession();
			static void DispatchComputeSession(uint32_t sessionID);
			static bool IsFinished(uint32_t sessionID);
			static void WaitForFinish(uint32_t sessionID);

			// Workload recording:
			static void RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
			static ShaderProperties RecordComputeShader(uint32_t sessionID, ComputeShader& computeShader, Uint3 threadCount);
			static void RecordBarrier(uint32_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask);

		private: // Methods
			// Delete all constructors:
			Async() = delete;
			Async(const Async&) = delete;
			Async& operator=(const Async&) = delete;
			Async(Async&&) = delete;
			Async& operator=(Async&&) = delete;
			~Async() = delete;
		};



		class EMBER_CORE_API Immediate
		{
		private: // Members:
			static emberBackendInterface::ICompute::IImmediate* m_pIImmediate;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IImmediate* pIImmediate);
			static void Clear();

			// Immediate dispatch call:
			static void Dispatch(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);

		private: // Methods
			// Delete all constructors:
			Immediate() = delete;
			Immediate(const Immediate&) = delete;
			Immediate& operator=(const Immediate&) = delete;
			Immediate(Immediate&&) = delete;
			Immediate& operator=(Immediate&&) = delete;
			~Immediate() = delete;
		};



		class EMBER_CORE_API PostRender
		{
		private: // Members:
			static emberBackendInterface::ICompute::IPostRender* m_pIPostRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IPostRender* pIPostRender);
			static void Clear();

			// Workload recording:
			static void RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties);
			static ShaderProperties RecordComputeShader(ComputeShader& computeShader);

		private: // Methods
			// Delete all constructors:
			PostRender() = delete;
			PostRender(const PostRender&) = delete;
			PostRender& operator=(const PostRender&) = delete;
			PostRender(PostRender&&) = delete;
			PostRender& operator=(PostRender&&) = delete;
			~PostRender() = delete;
		};



		class EMBER_CORE_API PreRender
		{
		private: // Members:
			static emberBackendInterface::ICompute::IPreRender* m_pIPreRender;

		public: // Methods:
			// Constructor/Destructor:
			static void Init(emberBackendInterface::ICompute::IPreRender* pIPreRender);
			static void Clear();

			// Workload recording:
			static void RecordComputeShader(ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
			static ShaderProperties RecordComputeShader(ComputeShader& computeShader, Uint3 threadCount);
			static void RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask);

		private: // Methods
			// Delete all constructors:
			PreRender() = delete;
			PreRender(const PreRender&) = delete;
			PreRender& operator=(const PreRender&) = delete;
			PreRender(PreRender&&) = delete;
			PreRender& operator=(PreRender&&) = delete;
			~PreRender() = delete;
		};



	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::ICompute> s_pICompute;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::ICompute* pICompute);
		static void Clear();

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