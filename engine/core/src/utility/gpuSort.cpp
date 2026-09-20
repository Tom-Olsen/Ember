#include "gpuSort.h"
#include "callProperties.h"
#include "computeShaderManager.h"
#include <assert.h>



namespace emberCore
{
	// Static members:
	template <typename T>
	bool GpuSort<T>::s_isInitialized = false;
	// Bitonic sort compute shaders:
	template <typename T>
	ComputeShader GpuSort<T>::s_localBitonicSortComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_bigFlipComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_bigDisperseComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_localDisperseComputeShader;
	// Bitonic permutation sort compute shaders:
	template <typename T>
	ComputeShader GpuSort<T>::s_localBitonicSortPermutationComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_bigFlipPermutationComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_bigDispersePermutationComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_localDispersePermutationComputeShader;
	// Other compute shaders:
	template <typename T>
	ComputeShader GpuSort<T>::s_initIndexBufferComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_applyPermutationComputeShader;
	template <typename T>
	ComputeShader GpuSort<T>::s_invertPermutationComputeShader;



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
			s_localBitonicSortComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortInt");
			s_bigFlipComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipInt");
			s_bigDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("bigDisperseInt");
			s_localDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("localDisperseInt");

			s_localBitonicSortPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationInt");
			s_bigFlipPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipPermutationInt");
			s_bigDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigDispersePermutationInt");
			s_localDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localDispersePermutationInt");

			s_applyPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("applyPermutationInt");
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
			s_localBitonicSortComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortUint");
			s_bigFlipComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipUint");
			s_bigDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("bigDisperseInt");
			s_localDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("localDisperseUint");

			s_localBitonicSortPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationUint");
			s_bigFlipPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipPermutationUint");
			s_bigDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigDispersePermutationUint");
			s_localDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localDispersePermutationUint");

			s_applyPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("applyPermutationUint");
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
			s_localBitonicSortComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat");
			s_bigFlipComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipFloat");
			s_bigDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("bigDisperseFloat");
			s_localDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("localDisperseFloat");

			s_localBitonicSortPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat");
			s_bigFlipPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat");
			s_bigDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat");
			s_localDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat");

			s_applyPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("applyPermutationFloat");
		}
		if constexpr (std::is_same_v<T, Float2>)
		{
			s_localBitonicSortComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat2");
			s_bigFlipComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipFloat2");
			s_bigDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("bigDisperseFloat2");
			s_localDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("localDisperseFloat2");

			s_localBitonicSortPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat2");
			s_bigFlipPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat2");
			s_bigDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat2");
			s_localDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat2");

			s_applyPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("applyPermutationFloat2");
		}
		if constexpr (std::is_same_v<T, Float3>)
		{
			s_localBitonicSortComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortFloat3");
			s_bigFlipComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipFloat3");
			s_bigDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("bigDisperseFloat3");
			s_localDisperseComputeShader = ComputeShaderManager::TryGetComputeShader("localDisperseFloat3");

			s_localBitonicSortPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localBitonicSortPermutationFloat3");
			s_bigFlipPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigFlipPermutationFloat3");
			s_bigDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("bigDispersePermutationFloat3");
			s_localDispersePermutationComputeShader = ComputeShaderManager::TryGetComputeShader("localDispersePermutationFloat3");

			s_applyPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("applyPermutationFloat3");
		}
		if constexpr (std::is_same_v<T, Float4>)
		{
			throw std::runtime_error("GpuSort<Float4>::Init: shaders for Float4 not implemented yet.");
		}

		s_initIndexBufferComputeShader = ComputeShaderManager::TryGetComputeShader("initIndexBuffer");
		s_invertPermutationComputeShader = ComputeShaderManager::TryGetComputeShader("invertPermutation");
	}
	template <typename T>
	void GpuSort<T>::Clear()
	{
		// Bitonic sort compute shaders:
		s_localBitonicSortComputeShader = ComputeShader();
		s_bigFlipComputeShader = ComputeShader();
		s_bigDisperseComputeShader = ComputeShader();
		s_localDisperseComputeShader = ComputeShader();
		// Bitonic permutation sort compute shaders:
		s_localBitonicSortPermutationComputeShader = ComputeShader();
		s_bigFlipPermutationComputeShader = ComputeShader();
		s_bigDispersePermutationComputeShader = ComputeShader();
		s_localDispersePermutationComputeShader = ComputeShader();
		// Other compute shaders:
		s_initIndexBufferComputeShader = ComputeShader();
		s_applyPermutationComputeShader = ComputeShader();
		s_invertPermutationComputeShader = ComputeShader();

		s_isInitialized = false;
	}



	// Sort:
	template <typename T>
	void GpuSort<T>::Sort(ComputeType computeType, BufferView<T>& bufferView, uint32_t sessionID)
	{
		// Post render compute derives dispatch size from the render target and does not record barriers, so sorting is unsupported:
		assert(computeType != ComputeType::postRender);
		if (!s_localBitonicSortComputeShader.IsValid())
			throw std::runtime_error("GpuSort::Sort: shaders for this type not implemented yet.");

		// Gpu buffer access setup:
		int blockSize = 2 * s_localBitonicSortComputeShader.GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());   // total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Record compute shaders:
		{
			// Local bitonic sort for each block:
			CallProperties callProperties = Compute::RecordComputeShader(computeType, s_localBitonicSortComputeShader, threadCountLocal, sessionID);
			callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			callProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				callProperties = Compute::RecordComputeShader(computeType, s_bigFlipComputeShader, threadCountBig, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetValue("Values", "flipHeight", flipHeight);
				callProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					callProperties = Compute::RecordComputeShader(computeType, s_bigDisperseComputeShader, threadCountBig, sessionID);
					callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					callProperties.SetValue("Values", "disperseHeight", disperseHeight);
					callProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}

				// Local disperse:
				callProperties = Compute::RecordComputeShader(computeType, s_localDisperseComputeShader, threadCountLocal, sessionID);
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
		if (!s_localBitonicSortPermutationComputeShader.IsValid())
			throw std::runtime_error("GpuSort::SortPermutation: shaders for this type not implemented yet.");

		// Gpu buffer access setup:
		int blockSize = 2 * s_localBitonicSortPermutationComputeShader.GetBlockSize().x;
		int bufferSize = static_cast<int>(bufferView.GetCount());	// total number of elements for sorting (entire buffer).
		int height = math::NextPowerOfTwo((uint32_t)bufferSize);	// height of biggest flip.
		Uint3 threadCountLocal = Uint3(bufferSize / 2, 1, 1);		// local bitonicSort/dispere only ever need to check entries up to buffer size.
		Uint3 threadCountBig = Uint3(height / 2, 1, 1);				// needed to make sure that big flip/disperse hit all swap indices.

		// Record compute shaders:
		{
			// Initialize index(permutation) buffer:
			Uint3 threadCountInit = Uint3(permutationBufferView.GetCount(), 1, 1);
			CallProperties callProperties = Compute::RecordComputeShader(computeType, s_initIndexBufferComputeShader, threadCountInit, sessionID);
			callProperties.SetBuffer("indexBuffer", permutationBufferView.GetBuffer());
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			// Local bitonic sort for each block:
			callProperties = Compute::RecordComputeShader(computeType, s_localBitonicSortPermutationComputeShader, threadCountLocal, sessionID);
			callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
			callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
			callProperties.SetValue("Values", "bufferSize", bufferSize);
			Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

			for (int flipHeight = 2 * blockSize; flipHeight <= height; flipHeight *= 2)
			{
				// Big flip:
				callProperties = Compute::RecordComputeShader(computeType, s_bigFlipPermutationComputeShader, threadCountBig, sessionID);
				callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
				callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
				callProperties.SetValue("Values", "flipHeight", flipHeight);
				callProperties.SetValue("Values", "bufferSize", bufferSize);
				Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);

				for (int disperseHeight = flipHeight / 2; disperseHeight > blockSize; disperseHeight /= 2)
				{
					// Big disperse:
					callProperties = Compute::RecordComputeShader(computeType, s_bigDispersePermutationComputeShader, threadCountBig, sessionID);
					callProperties.SetBuffer("dataBuffer", bufferView.GetBuffer());
					callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
					callProperties.SetValue("Values", "disperseHeight", disperseHeight);
					callProperties.SetValue("Values", "bufferSize", bufferSize);
					Compute::RecordBarrierWaitStorageWriteBeforeRead(computeType, sessionID);
				}

				// Local disperse:
				callProperties = Compute::RecordComputeShader(computeType, s_localDispersePermutationComputeShader, threadCountLocal, sessionID);
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
		CallProperties callProperties = Compute::RecordComputeShader(computeType, s_applyPermutationComputeShader, threadCount, sessionID);
		callProperties.SetBuffer("permutationBuffer", permutationBufferView.GetBuffer());
		callProperties.SetBuffer("inBuffer", inBufferView.GetBuffer());
		callProperties.SetBuffer("outBuffer", outBufferView.GetBuffer());
	}


	// Invert Permutation:
	template <typename T>
	void GpuSort<T>::InvertPermutation(ComputeType computeType, BufferView<uint32_t>& permutationBufferView, BufferView<uint32_t>& inversePermutationBufferView, uint32_t sessionID)
	{
		Uint3 threadCount = Uint3(permutationBufferView.GetCount(), 1, 1);
		CallProperties callProperties = Compute::RecordComputeShader(computeType, s_invertPermutationComputeShader, threadCount, sessionID);
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