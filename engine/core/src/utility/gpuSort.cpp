#include "gpuSort.h"
#include "callProperties.h"
#include "computeShaderManager.h"
#include <assert.h>



namespace emberCore
{
	// Ember::ToDo: replace computeShader unique ptrs with value members.
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

		if constexpr (std::is_same_v<T, int>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortInt"));
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipInt"));
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDisperseInt"));
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDisperseInt"));

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationInt"));
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipPermutationInt"));
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDispersePermutationInt"));
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDispersePermutationInt"));

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("applyPermutationInt"));
		}
		if constexpr (std::is_same_v<T, Int2>)
		{
			throw std::runtime_error("GpuSort<Int2>::Init: shaders for Int2 not implemented yet.");
		}
		if constexpr (std::is_same_v<T, Int3>)
		{
			throw std::runtime_error("GpuSort<Int3>::Init: shaders for Int3 not implemented yet.");
		}
		if constexpr (std::is_same_v<T, uint32_t>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortUint"));
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipUint"));
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDisperseInt"));
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDisperseUint"));

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationUint"));
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipPermutationUint"));
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDispersePermutationUint"));
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDispersePermutationUint"));

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("applyPermutationUint"));
		}
		if constexpr (std::is_same_v<T, Uint2>)
		{
			throw std::runtime_error("GpuSort<Uint2>::Init: shaders for Uint2 not implemented yet.");
		}
		if constexpr (std::is_same_v<T, Uint3>)
		{
			throw std::runtime_error("GpuSort<Uint3>::Init: shaders for Uint3 not implemented yet.");
		}
		if constexpr (std::is_same_v<T, float>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat"));
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipFloat"));
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDisperseFloat"));
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDisperseFloat"));

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat"));
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat"));
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat"));
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat"));

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("applyPermutationFloat"));
		}
		if constexpr (std::is_same_v<T, Float2>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat2"));
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipFloat2"));
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDisperseFloat2"));
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDisperseFloat2"));

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat2"));
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat2"));
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat2"));
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat2"));

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("applyPermutationFloat2"));
		}
		if constexpr (std::is_same_v<T, Float3>)
		{
			s_pLocalBitonicSortComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat3"));
			s_pBigFlipComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipFloat3"));
			s_pBigDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDisperseFloat3"));
			s_pLocalDisperseComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDisperseFloat3"));

			s_pLocalBitonicSortPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat3"));
			s_pBigFlipPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat3"));
			s_pBigDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat3"));
			s_pLocalDispersePermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat3"));

			s_pApplyPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("applyPermutationFloat3"));
		}
		if constexpr (std::is_same_v<T, Float4>)
		{
			throw std::runtime_error("GpuSort<Float4>::Init: shaders for Float4 not implemented yet.");
		}

		s_pInitIndexBufferComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("initIndexBuffer"));
		s_pInvertPermutationComputeShader = std::make_unique<ComputeShader>(ComputeShaderManager::TryGetComputeShader("invertPermutation"));
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
			CallProperties callProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortComputeShader, threadCountLocal, sessionID);
			callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			callProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				callProperties = Compute::RecordComputeShader(computeType, *s_pBigFlipComputeShader, threadCountBig, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetValue("Values", "flipHeight", flipHeight);
				callProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					callProperties = Compute::RecordComputeShader(computeType, *s_pBigDisperseComputeShader, threadCountBig, sessionID);
					callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					callProperties.SetValue("Values", "disperseHeight", disperseHeight);
					callProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}

				// Local disperse:
				callProperties = Compute::RecordComputeShader(computeType, *s_pLocalDisperseComputeShader, threadCountLocal, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetValue("Values", "bufferSize", bufferSize);
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
			CallProperties callProperties = Compute::RecordComputeShader(computeType, *s_pInitIndexBufferComputeShader, threadCountInit, sessionID);
			callProperties.SetBuffer("indexBuffer", permutationBufferView.GetBuffer());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			// Local bitonic sort for each block:
			callProperties = Compute::RecordComputeShader(computeType, *s_pLocalBitonicSortPermutationComputeShader, threadCountLocal, sessionID);
			callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
			callProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				callProperties = Compute::RecordComputeShader(computeType, *s_pBigFlipPermutationComputeShader, threadCountBig, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
				callProperties.SetValue("Values", "flipHeight", flipHeight);
				callProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					callProperties = Compute::RecordComputeShader(computeType, *s_pBigDispersePermutationComputeShader, threadCountBig, sessionID);
					callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
					callProperties.SetValue("Values", "disperseHeight", disperseHeight);
					callProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}

				// Local disperse:
				callProperties = Compute::RecordComputeShader(computeType, *s_pLocalDispersePermutationComputeShader, threadCountLocal, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
				callProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
			}
		}
	}



	// Apply Permutation:
	template <typename T>
	void GpuSort<T>::ApplyPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<T>& inBufferView, BufferView<T>& outBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		CallProperties callProperties = Compute::RecordComputeShader(computeType, *s_pApplyPermutationComputeShader, threadCount, sessionID);
		callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		callProperties.SetBuffer("inBuffer", inBufferView.GetBuffer());
		callProperties.SetBuffer("outBuffer", outBufferView.GetBuffer());
	}


	// Invert Permutation:
	template <typename T>
	void GpuSort<T>::InvertPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		CallProperties callProperties = Compute::RecordComputeShader(computeType, *s_pInvertPermutationComputeShader, threadCount, sessionID);
		callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		callProperties.SetBuffer("inversePermutationBuffer", inversePermutationBufferView.GetBuffer());
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