#include "gpuSort.h"
#include "shaderProperties.h"
#include <assert.h>



namespace emberCore
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
	// Other compute shaders:
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pInitIndexBufferComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pApplyPermutationComputeShader;
	template <typename T>
	std::unique_ptr<ComputeShader> GpuSort<T>::s_pInvertPermutationComputeShader;



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
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortInt.comp.spv", "localBitonicSortInt");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipInt.comp.spv", "bigFlipInt");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseInt.comp.spv", "bigDisperseInt");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseInt.comp.spv", "localDisperseInt");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationInt.comp.spv", "localBitonicSortPermutationInt");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationInt.comp.spv", "bigFlipPermutationInt");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationInt.comp.spv", "bigDispersePermutationInt");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationInt.comp.spv", "localDispersePermutationInt");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationInt.comp.spv", "applyPermutationInt");
		}
		if constexpr (std::is_same_v<T, Int2>)
		{
			throw std::runtime_error("GpuSort<Int2>::Init: shaders for Int2 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortInt2.comp.spv", "localBitonicSortInt2");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipInt2.comp.spv", "bigFlipInt2");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseInt2.comp.spv", "bigDisperseInt2");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseInt2.comp.spv", "localDisperseInt2");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationInt2.comp.spv", "localBitonicSortPermutationInt2");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationInt2.comp.spv", "bigFlipPermutationInt2");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationInt2.comp.spv", "bigDispersePermutationInt2");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationInt2.comp.spv", "localDispersePermutationInt2");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationInt2.comp.spv", "applyPermutationInt2");
		}
		if constexpr (std::is_same_v<T, Int3>)
		{
			throw std::runtime_error("GpuSort<Int3>::Init: shaders for Int3 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortInt3.comp.spv", "localBitonicSortInt3");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipInt3.comp.spv", "bigFlipInt3");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseInt3.comp.spv", "bigDisperseInt3");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseInt3.comp.spv", "localDisperseInt3");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationInt3.comp.spv", "localBitonicSortPermutationInt3");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationInt3.comp.spv", "bigFlipPermutationInt3");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationInt3.comp.spv", "bigDispersePermutationInt3");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationInt3.comp.spv", "localDispersePermutationInt3");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationInt3.comp.spv", "applyPermutationInt3");
		}
		if constexpr (std::is_same_v<T, uint32_t>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortUint.comp.spv", "localBitonicSortUint");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipUint.comp.spv", "bigFlipUint");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseInt.comp.spv", "bigDisperseUint");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseUint.comp.spv", "localDisperseUint");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationUint.comp.spv", "localBitonicSortPermutationUint");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationUint.comp.spv", "bigFlipPermutationUint");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationUint.comp.spv", "bigDispersePermutationUint");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationUint.comp.spv", "localDispersePermutationUint");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationUint.comp.spv", "applyPermutationUint");
		}
		if constexpr (std::is_same_v<T, Uint2>)
		{
			throw std::runtime_error("GpuSort<Uint2>::Init: shaders for Uint2 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortUint2.comp.spv", "localBitonicSortUint2");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipUint2.comp.spv", "bigFlipUint2");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseUint2.comp.spv", "bigDisperseUint2");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseUint2.comp.spv", "localDisperseUint2");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationUint2.comp.spv", "localBitonicSortPermutationUint2");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationUint2.comp.spv", "bigFlipPermutationUint2");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationUint2.comp.spv", "bigDispersePermutationUint2");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationUint2.comp.spv", "localDispersePermutationUint2");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationUint2.comp.spv", "applyPermutationUint2");
		}
		if constexpr (std::is_same_v<T, Uint3>)
		{
			throw std::runtime_error("GpuSort<Uint3>::Init: shaders for Uint3 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortUint3.comp.spv", "localBitonicSortUint3");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipUint3.comp.spv", "bigFlipUint3");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseUint3.comp.spv", "bigDisperseUint3");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseUint3.comp.spv", "localDisperseUint3");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationUint3.comp.spv", "localBitonicSortPermutationUint3");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationUint3.comp.spv", "bigFlipPermutationUint3");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationUint3.comp.spv", "bigDispersePermutationUint3");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationUint3.comp.spv", "localDispersePermutationUint3");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationUint3.comp.spv", "applyPermutationUint3");
		}
		if constexpr (std::is_same_v<T, float>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortFloat.comp.spv", "localBitonicSortFloat");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipFloat.comp.spv", "bigFlipFloat");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseFloat.comp.spv", "bigDisperseFloat");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseFloat.comp.spv", "localDisperseFloat");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationFloat.comp.spv", "localBitonicSortPermutationFloat");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationFloat.comp.spv", "bigFlipPermutationFloat");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationFloat.comp.spv", "bigDispersePermutationFloat");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationFloat.comp.spv", "localDispersePermutationFloat");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationFloat.comp.spv", "applyPermutationFloat");
		}
		if constexpr (std::is_same_v<T, Float2>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortFloat2.comp.spv", "localBitonicSortFloat2");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipFloat2.comp.spv", "bigFlipFloat2");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseFloat2.comp.spv", "bigDisperseFloat2");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseFloat2.comp.spv", "localDisperseFloat2");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationFloat2.comp.spv", "localBitonicSortPermutationFloat2");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationFloat2.comp.spv", "bigFlipPermutationFloat2");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationFloat2.comp.spv", "bigDispersePermutationFloat2");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationFloat2.comp.spv", "localDispersePermutationFloat2");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationFloat2.comp.spv", "applyPermutationFloat2");
		}
		if constexpr (std::is_same_v<T, Float3>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortFloat3.comp.spv", "localBitonicSortFloat3");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipFloat3.comp.spv", "bigFlipFloat3");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseFloat3.comp.spv", "bigDisperseFloat3");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseFloat3.comp.spv", "localDisperseFloat3");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationFloat3.comp.spv", "localBitonicSortPermutationFloat3");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationFloat3.comp.spv", "bigFlipPermutationFloat3");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationFloat3.comp.spv", "bigDispersePermutationFloat3");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationFloat3.comp.spv", "localDispersePermutationFloat3");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationFloat3.comp.spv", "applyPermutationFloat3");
		}
		if constexpr (std::is_same_v<T, Float4>)
		{
			throw std::runtime_error("GpuSort<Float4>::Init: shaders for Float4 not implemented yet.");
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortFloat4.comp.spv", "localBitonicSortFloat4");
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipFloat4.comp.spv", "bigFlipFloat4");
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDisperseFloat4.comp.spv", "bigDisperseFloat4");
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDisperseFloat4.comp.spv", "localDisperseFloat4");

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localBitonicSortPermutationFloat4.comp.spv", "localBitonicSortPermutationFloat4");
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigFlipPermutationFloat4.comp.spv", "bigFlipPermutationFloat4");
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "bigDispersePermutationFloat4.comp.spv", "bigDispersePermutationFloat4");
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "localDispersePermutationFloat4.comp.spv", "localDispersePermutationFloat4");

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "applyPermutationFloat4.comp.spv", "applyPermutationFloat4");
		}

		s_pInitIndexBufferComputeShader = std::make_unique<ComputeShader>(directoryPath / "initIndexBuffer.comp.spv", "initIndexBuffer");
		s_pInvertPermutationComputeShader = std::make_unique<ComputeShader>(directoryPath / "invertPermutation.comp.spv", "invertPermutation");
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
		// Other compute shaders:
		s_pInitIndexBufferComputeShader.reset();
		s_pApplyPermutationComputeShader.reset();
		s_pInvertPermutationComputeShader.reset();

		s_isInitialized = false;
	}



	// Sort:
	template <typename T>
	void GpuSort<T>::Sort(ComputeType computeType, BufferView<T>& bufferView, uint32_t sessionID)
	{
		// Post render compute derives dispatch size from the render target and does not record barriers, so sorting is unsupported:
		assert(computeType != ComputeType::postRender);
		if (!s_pLocalBitonicSortComputeShader)
			throw std::runtime_error("GpuSort::Sort: shaders for this type not implemented yet.");

		// Gpu buffer access setup:
		int blockSize = 2 * s_pLocalBitonicSortComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());   // total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

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
	}



	// Permutation sort:
	template <typename T>
	void GpuSort<T>::SortPermutation(ComputeType computeType, BufferView<T>& bufferView, BufferView<uint32_t>& permutationBufferView, uint32_t sessionID)
	{
		// Post render compute derives dispatch size from the render target and does not record barriers, so sorting is unsupported:
		assert(computeType != ComputeType::postRender);
		if (!s_pLocalBitonicSortPermutationComputeShader)
			throw std::runtime_error("GpuSort::SortPermutation: shaders for this type not implemented yet.");

		// Gpu buffer access setup:
		int blockSize = 2 * s_pLocalBitonicSortPermutationComputeShader->GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());	// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Record compute shaders:
		{
			// Initialize index(permutation) buffer:
			Uint3 threadCountInit = Uint3(permutationBufferView.GetCount(), 1, 1);
			ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pInitIndexBufferComputeShader, threadCountInit, sessionID);
			shaderProperties.SetBuffer("indexBuffer", permutationBufferView.GetBuffer());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			// Local bitonic sort for each block:
			shaderProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortPermutationComputeShader, threadCountLocal, sessionID);
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
	}



	// Apply Permutation:
	template <typename T>
	void GpuSort<T>::ApplyPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pApplyPermutationComputeShader, threadCount, sessionID);
		shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		shaderProperties.SetBuffer("inBuffer", inBufferView.GetBuffer());
		shaderProperties.SetBuffer("outBuffer", outBufferView.GetBuffer());
	}


	// Invert Permutation:
	template <typename T>
	void GpuSort<T>::InvertPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		ShaderProperties shaderProperties = Compute::RecordComputeShader(computeType, *s_pInvertPermutationComputeShader, threadCount, sessionID);
		shaderProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		shaderProperties.SetBuffer("inversePermutationBuffer", inversePermutationBufferView.GetBuffer());
	}



	// Dispatch and wait:
	template <typename T>
	void GpuSort<T>::SortAndWait(BufferView<T>& bufferView)
	{
		uint32_t sessionID = Compute::Async::CreateComputeSession();
		Sort(ComputeType::async, bufferView, sessionID);
		Compute::Async::DispatchComputeSessionAndWait(sessionID);
	}
	template <typename T>
	void GpuSort<T>::SortPermutationAndWait(BufferView<T>& bufferView, BufferView<uint32_t>& permutationBufferView)
	{
		uint32_t sessionID = Compute::Async::CreateComputeSession();
		SortPermutation(ComputeType::async, bufferView, permutationBufferView, sessionID);
		Compute::Async::DispatchComputeSessionAndWait(sessionID);
	}
	template <typename T>
	void GpuSort<T>::ApplyPermutationAndWait(BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView)
	{
		uint32_t sessionID = Compute::Async::CreateComputeSession();
		ApplyPermutation(ComputeType::async, permutationBufferView, inBufferView, outBufferView, sessionID);
		Compute::Async::DispatchComputeSessionAndWait(sessionID);
	}
	template <typename T>
	void GpuSort<T>::InvertPermutationAndWait(BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView)
	{
		uint32_t sessionID = Compute::Async::CreateComputeSession();
		InvertPermutation(ComputeType::async, permutationBufferView, inversePermutationBufferView, sessionID);
		Compute::Async::DispatchComputeSessionAndWait(sessionID);
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