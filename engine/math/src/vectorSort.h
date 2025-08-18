#ifndef __INCLUDE_GUARD_vectorSort_h__
#define __INCLUDE_GUARD_vectorSort_h__
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>



namespace emberMath
{
	namespace math
	{
		// Sort vector in place:
		template <typename T, typename Compare>
		void Sort(std::vector<T>& vec, const Compare& compare)
		{
			std::sort(vec.begin(), vec.end(), compare);
		}

		// Get sorted copy of given vector:
		template <typename T, typename Compare>
		std::vector<T> CopySort(const std::vector<T>& vec, const Compare& compare)
		{
			std::vector<T> sortedVec = vec;
			std::sort(sortedVec.begin(), sortedVec.end(), compare);
			return sortedVec;
		}

		// Create sorting permutation vector:
		template <typename T, typename Compare>
		std::vector<std::size_t> SortPermutation(const std::vector<T>& vec, const Compare& compare)
		{
			std::vector<std::size_t> permutation(vec.size());
			std::iota(permutation.begin(), permutation.end(), 0);
			std::sort(permutation.begin(), permutation.end(), [&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
			return permutation;
		}
		// Apply sorting permutation to any vector of same length:
		template <typename T>
		std::vector<T> ApplyPermutation(const std::vector<T>& vec, const std::vector<std::size_t>& permutation)
		{
			if (vec.size() != permutation.size())
				throw std::runtime_error("ApplyPermutation: vector and permutation do not have same length.");

			std::vector<T> sortedVec(vec.size());
			std::transform(permutation.begin(), permutation.end(), sortedVec.begin(), [&](std::size_t i) { return vec[i]; });
			return sortedVec;
		}
	}
}



#endif // __INCLUDE_GUARD_vectorSort_h__