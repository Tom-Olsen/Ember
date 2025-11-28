#pragma once
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
		inline void Sort(std::vector<T>& vector, const Compare& compare)
		{
			std::sort(vector.begin(), vector.end(), compare);
		}
		// Get sorted copy of given vector:
		template <typename T, typename Compare>
		inline std::vector<T> CopySort(const std::vector<T>& vector, const Compare& compare)
		{
			std::vector<T> sortedVec = vector;
			std::sort(sortedVec.begin(), sortedVec.end(), compare);
			return sortedVec;
		}



		// Create sorting permutation vector:
		template <typename T, typename Compare>
		inline std::vector<std::size_t> SortPermutation(const std::vector<T>& vector, const Compare& compare)
		{
			std::vector<std::size_t> permutation(vector.size());
			std::iota(permutation.begin(), permutation.end(), 0);	// fills with indexes 0, 1, 2, ...
			std::sort(permutation.begin(), permutation.end(), [&](std::size_t i, std::size_t j) { return compare(vector[i], vector[j]); });
			return permutation;
		}
		// Create sorting permutation in given vector:
		template <typename T, typename Compare>
		inline void SortPermutation(const std::vector<T>& vector, std::vector<size_t>& permutation, const Compare& compare)
		{
			if (vector.size() != permutation.size())
				permutation.resize(vector.size());

			std::iota(permutation.begin(), permutation.end(), 0);	// fills with indexes 0, 1, 2, ...
			std::sort(permutation.begin(), permutation.end(), [&](std::size_t i, std::size_t j) { return compare(vector[i], vector[j]); });
		}



		// Apply sorting permutation to a vector in place (contains implicit allocation):
		template <typename T>
		inline std::vector<T> ApplyPermutation(const std::vector<T>& vector, const std::vector<std::size_t>& permutation)
		{
			if (vector.size() != permutation.size())
				throw std::runtime_error("ApplyPermutation: vector and permutation do not have same length.");

			std::vector<T> sortedVec(vector.size());
			std::transform(permutation.begin(), permutation.end(), sortedVec.begin(), [&](std::size_t i) { return vector[i]; });
			return sortedVec;
		}
		// Apply sorting permutation form in vector to out vector:
		template <typename T>
		inline void ApplyPermutation(const std::vector<T>& vecIn, std::vector<T>& vecOut, const std::vector<std::size_t>& permutation)
		{
			if (vecIn.size() != permutation.size())
				throw std::runtime_error("ApplyPermutation: vecIn and permutation do not have same length.");
			if (vecOut.size() != permutation.size())
				throw std::runtime_error("ApplyPermutation: vecOut and permutation do not have same length.");

			std::transform(permutation.begin(), permutation.end(), vecOut.begin(), [&](std::size_t i) { return vecIn[i]; });
		}
	}
}