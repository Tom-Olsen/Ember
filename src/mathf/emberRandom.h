#ifndef __INCLUDE_GUARD_emberRandom_h__
#define __INCLUDE_GUARD_emberRandom_h__
#include "float3.h"
#include <random>



namespace mathf
{
	class Random
	{
	private: // Members:
		static bool s_isInitialized;
		static std::mt19937 s_mersenneTwister;
		static uint32_t s_seed;

	public: // Methods:
		static void Init();

		// Seed management:
		static void SetSeed(uint32_t seed);
		static uint32_t GetSeed();

		// Random number generation:
		static float Uniform01();
		static float Uniform(float min, float max);
		static int Uniform(int min, int max);
		static Float3 UniformDirection();
	};
}


#endif // __INCLUDE_GUARD_emberRandom_h__