#include "gpuSort.h"



namespace emberEngine
{
	// Static members:
	template <typename T>
	bool GpuSort<T>::s_isInitialized = false;
	// Bitonic sort compute shaders:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pLocalBitonicSortComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pBigFlipComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pBigDisperseComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pLocalDisperseComputeShader;
	// Bitonic permutation sort compute shaders:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pLocalBitonicSortPermutationComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pBigFlipPermutationComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pBigDispersePermutationComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pLocalDispersePermutationComputeShader;
	// Initialize index(permutation) buffer:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pInitIndexBufferComputeShader;
	template <typename T>
	std::unique_ptr<ShaderProperties> GpuSort<T>::s_pInitIndexBufferShaderProperties;
	// Apply permutation:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pApplyPermutationComputeShader;
	template <typename T>
	std::unique_ptr<ShaderProperties> GpuSort<T>::s_pApplyPermutationShaderProperties;
	// Invert permutation:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pInvertPermutationComputeShader;
	template <typename T>
	std::unique_ptr<ShaderProperties> GpuSort<T>::s_pInvertPermutationShaderProperties;



	// Public methods:
	// Initialization/Cleanup:
	template <typename T>
	void GpuSort<T>::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_SHADERS_DIR) / "bin").make_preferred();
		if constexpr (std::is_same_v<T, int>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortInt", directoryPath / "localBitonicSortInt.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipInt", directoryPath / "bigFlipInt.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseInt", directoryPath / "bigDisperseInt.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseInt", directoryPath / "localDisperseInt.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationInt", directoryPath / "localBitonicSortPermutationInt.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationInt", directoryPath / "bigFlipPermutationInt.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationInt", directoryPath / "bigDispersePermutationInt.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationInt", directoryPath / "localDispersePermutationInt.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationInt", directoryPath / "applyPermutationInt.comp.spv");
		}
		if constexpr (std::is_same_v<T, Int2>)
		{
			throw std::runtime_error("GpuSort<Int2>::Init: shaders for Int2 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortInt2", directoryPath / "localBitonicSortInt2.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipInt2", directoryPath / "bigFlipInt2.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseInt2", directoryPath / "bigDisperseInt2.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseInt2", directoryPath / "localDisperseInt2.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationInt2", directoryPath / "localBitonicSortPermutationInt2.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationInt2", directoryPath / "bigFlipPermutationInt2.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationInt2", directoryPath / "bigDispersePermutationInt2.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationInt2", directoryPath / "localDispersePermutationInt2.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationInt2", directoryPath / "applyPermutationInt2.comp.spv");
		}
		if constexpr (std::is_same_v<T, Int3>)
		{
			throw std::runtime_error("GpuSort<Int3>::Init: shaders for Int3 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortInt3", directoryPath / "localBitonicSortInt3.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipInt3", directoryPath / "bigFlipInt3.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseInt3", directoryPath / "bigDisperseInt3.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseInt3", directoryPath / "localDisperseInt3.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationInt3", directoryPath / "localBitonicSortPermutationInt3.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationInt3", directoryPath / "bigFlipPermutationInt3.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationInt3", directoryPath / "bigDispersePermutationInt3.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationInt3", directoryPath / "localDispersePermutationInt3.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationInt3", directoryPath / "applyPermutationInt3.comp.spv");
		}
		if constexpr (std::is_same_v<T, uint32_t>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortUint", directoryPath / "localBitonicSortUint.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipUint", directoryPath / "bigFlipUint.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseUint", directoryPath / "bigDisperseInt.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseUint", directoryPath / "localDisperseUint.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationUint", directoryPath / "localBitonicSortPermutationUint.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationUint", directoryPath / "bigFlipPermutationUint.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationUint", directoryPath / "bigDispersePermutationUint.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationUint", directoryPath / "localDispersePermutationUint.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationUint", directoryPath / "applyPermutationUint.comp.spv");
		}
		if constexpr (std::is_same_v<T, Uint2>)
		{
			throw std::runtime_error("GpuSort<Uint2>::Init: shaders for Uint2 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortUint2", directoryPath / "localBitonicSortUint2.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipUint2", directoryPath / "bigFlipUint2.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseUint2", directoryPath / "bigDisperseUint2.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseUint2", directoryPath / "localDisperseUint2.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationUint2", directoryPath / "localBitonicSortPermutationUint2.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationUint2", directoryPath / "bigFlipPermutationUint2.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationUint2", directoryPath / "bigDispersePermutationUint2.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationUint2", directoryPath / "localDispersePermutationUint2.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationUint2", directoryPath / "applyPermutationUint2.comp.spv");
		}
		if constexpr (std::is_same_v<T, Uint3>)
		{
			throw std::runtime_error("GpuSort<Uint3>::Init: shaders for Uint3 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortUint3", directoryPath / "localBitonicSortUint3.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipUint3", directoryPath / "bigFlipUint3.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseUint3", directoryPath / "bigDisperseUint3.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseUint3", directoryPath / "localDisperseUint3.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationUint3", directoryPath / "localBitonicSortPermutationUint3.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationUint3", directoryPath / "bigFlipPermutationUint3.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationUint3", directoryPath / "bigDispersePermutationUint3.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationUint3", directoryPath / "localDispersePermutationUint3.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationUint3", directoryPath / "applyPermutationUint3.comp.spv");
		}
		if constexpr (std::is_same_v<T, float>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortFloat", directoryPath / "localBitonicSortFloat.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipFloat", directoryPath / "bigFlipFloat.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseFloat", directoryPath / "bigDisperseFloat.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseFloat", directoryPath / "localDisperseFloat.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationFloat", directoryPath / "localBitonicSortPermutationFloat.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationFloat", directoryPath / "bigFlipPermutationFloat.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationFloat", directoryPath / "bigDispersePermutationFloat.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationFloat", directoryPath / "localDispersePermutationFloat.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat", directoryPath / "applyPermutationFloat.comp.spv");
		}
		if constexpr (std::is_same_v<T, Float2>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortFloat2", directoryPath / "localBitonicSortFloat2.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipFloat2", directoryPath / "bigFlipFloat2.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseFloat2", directoryPath / "bigDisperseFloat2.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseFloat2", directoryPath / "localDisperseFloat2.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationFloat2", directoryPath / "localBitonicSortPermutationFloat2.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationFloat2", directoryPath / "bigFlipPermutationFloat2.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationFloat2", directoryPath / "bigDispersePermutationFloat2.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationFloat2", directoryPath / "localDispersePermutationFloat2.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat2", directoryPath / "applyPermutationFloat2.comp.spv");
		}
		if constexpr (std::is_same_v<T, Float3>)
		{
			throw std::runtime_error("GpuSort<Float3>::Init: shaders for Float3 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortFloat3", directoryPath / "localBitonicSortFloat3.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipFloat3", directoryPath / "bigFlipFloat3.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseFloat3", directoryPath / "bigDisperseFloat3.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseFloat3", directoryPath / "localDisperseFloat3.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationFloat3", directoryPath / "localBitonicSortPermutationFloat3.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationFloat3", directoryPath / "bigFlipPermutationFloat3.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationFloat3", directoryPath / "bigDispersePermutationFloat3.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationFloat3", directoryPath / "localDispersePermutationFloat3.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat3", directoryPath / "applyPermutationFloat3.comp.spv");
		}
		if constexpr (std::is_same_v<T, Float4>)
		{
			throw std::runtime_error("GpuSort<Float4>::Init: shaders for Float4 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>("localBitonicSortFloat4", directoryPath / "localBitonicSortFloat4.comp.spv");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>("bigFlipFloat4", directoryPath / "bigFlipFloat4.comp.spv");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>("bigDisperseFloat4", directoryPath / "bigDisperseFloat4.comp.spv");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>("localDisperseFloat4", directoryPath / "localDisperseFloat4.comp.spv");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>("localBitonicSortPermutationFloat4", directoryPath / "localBitonicSortPermutationFloat4.comp.spv");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>("bigFlipPermutationFloat4", directoryPath / "bigFlipPermutationFloat4.comp.spv");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>("bigDispersePermutationFloat4", directoryPath / "bigDispersePermutationFloat4.comp.spv");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>("localDispersePermutationFloat4", directoryPath / "localDispersePermutationFloat4.comp.spv");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>("applyPermutationFloat4", directoryPath / "applyPermutationFloat4.comp.spv");
		}
		s_pApplyPermutationShaderProperties = std::make_unique<ShaderProperties>(*s_pApplyPermutationComputeShader);

		s_pInitIndexBufferComputeShader = std::make_unique<ComputeShader>("initIndexBuffer", directoryPath / "initIndexBuffer.comp.spv");
		s_pInitIndexBufferShaderProperties = std::make_unique<ShaderProperties>(*s_pInitIndexBufferComputeShader);
		s_pInvertPermutationComputeShader = std::make_unique<ComputeShader>("invertPermutation", directoryPath / "invertPermutation.comp.spv");
		s_pInvertPermutationShaderProperties = std::make_unique<ShaderProperties>(*s_pInvertPermutationComputeShader);
	}
	template <typename T>
	void GpuSort<T>::Clear()
	{
		// Bitonic sort compute shaders:
		s_pLocalBitonicSortComputeShader.reset();
		s_pBigFlipComputeShader.reset();
		s_pBigDisperseComputeShader.reset();
		s_pLocalDisperseComputeShader.reset();
		// Bitonic permutation sort compute shaders:
		s_pLocalBitonicSortPermutationComputeShader.reset();
		s_pBigFlipPermutationComputeShader.reset();
		s_pBigDispersePermutationComputeShader.reset();
		s_pLocalDispersePermutationComputeShader.reset();
		// Initialize index(permutation) buffer:
		s_pInitIndexBufferComputeShader.reset();
		s_pInitIndexBufferShaderProperties.reset();
		// Apply permutation:
		s_pApplyPermutationComputeShader.reset();
		s_pApplyPermutationShaderProperties.reset();
		// Invert permutation:
		s_pInvertPermutationComputeShader.reset();
		s_pInvertPermutationShaderProperties.reset();

		s_isInitialized = false;
	}



	// Sort:
	template <typename T>
	void GpuSort<T>::Sort(ComputeType computeType, BufferView<T>& bufferView)
	{
		// Sorting makes no sense for PostRender compute, as it is only for post processing effects:
		assert(computeType != ComputeType::postRender);

		// Gpu buffer access setup:
		int blockSize = s_pLocalBitonicSortComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());		// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Sorting requires multiple synced kernel dispatches. Immediate dispatch mode gets emmulated by an async launch with wait for completion:
		uint32_t sessionID = -1;
		bool isImmediateCompute = (computeType == ComputeType::immediate);
		if (isImmediateCompute)
		{
			sessionID = Compute::Async::CreateComputeSession();
			computeType = ComputeType::async;
		}

		// Record compute shaders:
		{
			// Local bitonic sort for each block:
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortComputeShader, threadCountLocal, sessionID);
			shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
			
			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				shaderProperties = Compute::RecordComputeShader(computeType, *s_pBigFlipComputeShader, threadCountBig, sessionID);
				shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				shaderProperties.SetValue("Values", "flipHeight", flipHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				
				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					shaderProperties = Compute::RecordComputeShader(computeType, *s_pBigDisperseComputeShader, threadCountBig, sessionID);
					shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
					shaderProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}
				
				// Local disperse:
				shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalDisperseComputeShader, threadCountLocal, sessionID);
				shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
			}
		}

		// For immediate compute we dispatch and wait for the async compute session:
		if (isImmediateCompute)
		{
			Compute::Async::DispatchComputeSession(sessionID);
			Compute::Async::WaitForFinish(sessionID);
		}
	}



	// Permutation sort:
	template <typename T>
	void GpuSort<T>::SortPermutation(ComputeType computeType, BufferView<T>& bufferView, BufferView<uint32_t>& permutationBufferView)
	{
		// Sorting makes no sense for PostRender compute, as it is only for post processing effects:
		assert(computeType != ComputeType::postRender);

		// Gpu buffer access setup:
		int blockSize = s_pLocalBitonicSortPermutationComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());	// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Sorting requires multiple synced kernel dispatches. Immediate dispatch mode gets emmulated by an async launch with wait for completion:
		uint32_t sessionID = -1;
		bool isImmediateCompute = (computeType == ComputeType::immediate);
		if (isImmediateCompute)
		{
			sessionID = Compute::Async::CreateComputeSession();
			computeType = ComputeType::async;
		}

		// Record compute shaders:
		{
			// Initialize index(permutation) buffer:
			Uint3 threadCountInit = Uint3(permutationBufferView.GetCount(), 1, 1);
			s_pInitIndexBufferShaderProperties->SetBuffer("indexBuffer", permutationBufferView.GetBuffer());
			Compute::RecordComputeShader(computeType, *s_pInitIndexBufferComputeShader, *s_pInitIndexBufferShaderProperties, threadCountInit, sessionID);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			// Local bitonic sort for each block:
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortPermutationComputeShader, threadCountLocal, sessionID);
			shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
			shaderProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				shaderProperties = Compute::RecordComputeShader(computeType, *s_pBigFlipPermutationComputeShader, threadCountBig, sessionID);
				shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
				shaderProperties.SetValue("Values", "flipHeight", flipHeight);
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					shaderProperties = Compute::RecordComputeShader(computeType, *s_pBigDispersePermutationComputeShader, threadCountBig, sessionID);
					shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
					shaderProperties.SetValue("Values", "disperseHeight", disperseHeight);
					shaderProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}

				// Local disperse:
				shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalDispersePermutationComputeShader, threadCountLocal, sessionID);
				shaderProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
				shaderProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
			}
		}

		// For immediate compute we dispatch and wait for the async compute session:
		if (isImmediateCompute)
		{
			Compute::Async::DispatchComputeSession(sessionID);
			Compute::Async::WaitForFinish(sessionID);
		}
	}



	// Apply Permutation:
	template <typename T>
	void GpuSort<T>::ApplyPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		s_pApplyPermutationShaderProperties->SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		s_pApplyPermutationShaderProperties->SetBuffer("inBuffer", inBufferView.GetBuffer());
		s_pApplyPermutationShaderProperties->SetBuffer("outBuffer", outBufferView.GetBuffer());
		Compute::RecordComputeShader(computeType , *s_pApplyPermutationComputeShader, *s_pApplyPermutationShaderProperties, threadCount, sessionID);
	}


	// Invert Permutation:
	template <typename T>
	void GpuSort<T>::InvertPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		s_pInvertPermutationShaderProperties->SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		s_pInvertPermutationShaderProperties->SetBuffer("inversePermutationBuffer", inversePermutationBufferView.GetBuffer());
		Compute::RecordComputeShader(computeType, *s_pInvertPermutationComputeShader, *s_pInvertPermutationShaderProperties, threadCount, sessionID);
	}

    // Explicit template instantiations:
    template class GpuSort<int>;
    template class GpuSort<Int2>;
    template class GpuSort<Int3>;
    template class GpuSort<uint32_t>;
    template class GpuSort<Uint2>;
    template class GpuSort<Uint3>;
    template class GpuSort<float>;
    template class GpuSort<Float2>;
    template class GpuSort<Float3>;
    template class GpuSort<Float4>;
}