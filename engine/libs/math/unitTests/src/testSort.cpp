#include "emberMath.h"
#include "macros.h"
#include <gtest/gtest.h>



TEST(Sort, PermutationSort)
{
	math::Random::Init();

	// Number of elements:
	int count = 16;

	// Prepare cpu arrays:
	std::vector<int> data(count);
	std::vector<size_t> permutation(count);
	for (int i = 0; i < count; i++)
		data[i] = math::Random::Uniform(1, count);

	// Sort array on cpu:
	math::SortPermutation(data, permutation, [](int const& a, int const& b) { return a < b; });
	data = math::ApplyPermutation(data, permutation);

	// Check results:
	bool allGood = true;
	for (int i = 1; i < count; i++)
	{
		if (data[i - 1] > data[i])
		{
			allGood = false;
			EXPECT_FALSE(true) << "Sort mismatch: data[" << i-1 << "]=" << data[i-1] << ", data[" << i << "] = " << data[i];
		}
	}
	EXPECT_TRUE(allGood);
}