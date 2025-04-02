#ifndef __INCLUDE_GUARD_vectorSort_h__
#define __INCLUDE_GUARD_vectorSort_h__
#include <vector>
#include <numeric>
#include <algorithm>



template <typename T, typename Compare>
std::vector<std::size_t> SortPermutation(const std::vector<T>& vec, const Compare& compare)
{
	std::vector<std::size_t> permutation(vec.size());
	std::iota(permutation.begin(), permutation.end(), 0);
	std::sort(permutation.begin(), permutation.end(), [&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
	return permutation;
}
template <typename T>
std::vector<T> ApplyPermutation(const std::vector<T>& vec, const std::vector<std::size_t>& permutation)
{
	std::vector<T> sortedVec(vec.size());
	std::transform(permutation.begin(), permutation.end(), sortedVec.begin(), [&](std::size_t i) { return vec[i]; });
	return sortedVec;
}



#endif // __INCLUDE_GUARD_vectorSort_h__