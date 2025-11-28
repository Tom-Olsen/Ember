#pragma once
#include "application.h"
#include "emberEngine.h"



// Encapsulate in emberEngine namespace to avoid main.cpp namespace polution due to using namespace emberEngine.
namespace emberEngine
{
	TEST(Random, SetSeed)
	{
		math::Random::SetSeed(12345);
		float prediction[10] =
		{
			0.9296161f,
			0.8901547f,
			0.31637555f,
			0.1307073f,
			0.18391882f,
			0.039759457f,
			0.20456028f,
			0.8264361f,
			0.567725f,
			0.5320779f,
		};
		for (int i = 0; i < 10; i++)
		{
			float random = math::Random::Uniform01();
			LOG_TRACE(prediction[i]);
			EXPECT_FLOAT_EQ(random, prediction[i]);
		}

	}
	TEST(CpuSort, PermutationSort)
	{
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
}