#pragma once
#include "iCompute.h"
#include "commonAccessMask.h"
#include "commonRendererCreateInfo.h"
#include "emberMath.h"
#include <memory>
#include <vector>



namespace emberEngine
{
	// Forward decleration:
	class ComputeShader;
	class Renderer;
	class ShaderProperties;


	class Compute
	{
		// Friends:
		friend class Renderer;

	public: // Subclasses:
		class Async
		{
		private: // Members:
			std::unique_ptr<emberBackendInterface::ICompute::IAsync> m_pIAsync;
			emberBackendInterface::ICompute::IAsync* GetInterfaceHandle();

		public: // Methods:
			// Constructor/Destructor:
			Async(uint16_t sessionCount);
			~Async();

			// Non-copyable:
			Async(const Async&) = delete;
			Async& operator=(const Async&) = delete;

			// Movable:
			Async(Async&& other) noexcept = default;
			Async& operator=(Async&& other) noexcept = default;

			// Dispatch logic:
			uint16_t CreateComputeSession();
			void DispatchComputeSession(uint16_t sessionID);
			bool IsFinished(uint16_t sessionID);
			void WaitForFinish(uint16_t sessionID);

			// Workload recording:
			void RecordComputeShader(uint16_t sessionID, ComputeShader& computeShader, ShaderProperties& shaderProperties, Uint3 threadCount);
			ShaderProperties RecordComputeShader(uint16_t sessionID, ComputeShader& computeShader, Uint3 threadCount);
			void RecordBarrier(uint16_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask);
		};



		class Immediate
		{
		private: // Members:
			std::unique_ptr<emberBackendInterface::ICompute::IImmediate> m_pIImmediate;
			emberBackendInterface::ICompute::IImmediate* GetInterfaceHandle();

		public: // Methods:
			// Constructor/Destructor:
			Immediate();
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



		class PostRender
		{
		private: // Members:
			std::unique_ptr<emberBackendInterface::ICompute::IPostRender> m_pIPostRender;
			emberBackendInterface::ICompute::IPostRender* GetInterfaceHandle();

		public: // Methods:
			// Constructor/Destructor:
			PostRender();
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



		class PreRender
		{
		private: // Members:
			std::unique_ptr<emberBackendInterface::ICompute::IPreRender> m_pIPreRender;
			emberBackendInterface::ICompute::IPreRender* GetInterfaceHandle();

		public: // Methods:
			// Constructor/Destructor:
			PreRender();
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
		static std::unique_ptr<emberBackendInterface::ICompute> s_pICompute;
		static std::unique_ptr<Async> s_pAsync;
		static std::unique_ptr<Immediate> s_pImmediate;
		static std::unique_ptr<PostRender> s_pPostRender;
		static std::unique_ptr<PreRender> s_pPreRender;
		static emberBackendInterface::ICompute* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Cleanup:
		static void Init();
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