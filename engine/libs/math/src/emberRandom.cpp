#include "emberRandom.h"



namespace emberMath
{
	namespace math
	{
		// Static members:
		bool Random::s_isInitialized = false;
		std::mt19937 Random::s_mersenneTwister;
		uint32_t Random::s_seed = 0;



		// Public methods:
		// Initialization/Clear:
		void Random::Init()
		{
			if (s_isInitialized)
				return;
			s_isInitialized = true;

			s_seed = static_cast<uint32_t>(std::random_device{}());
			s_mersenneTwister.seed(s_seed);
		}
		void Random::Clear()
		{
			s_isInitialized = false;
		}

		// Seed management:
		void Random::SetSeed(uint32_t seed)
		{
			s_seed = seed;
			s_mersenneTwister.seed(s_seed);
		}
		uint32_t Random::GetSeed()
		{
			return s_seed;
		}

		// Random number generation:
		float Random::Uniform01()
		{
			std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
			return distribution(s_mersenneTwister);
		}
		float Random::Uniform(float min, float max)
		{
			std::uniform_real_distribution<float> distribution(min, max);
			return distribution(s_mersenneTwister);
		}
		int Random::Uniform(int min, int max)
		{
			std::uniform_int_distribution<int> distribution(min, max);
			return distribution(s_mersenneTwister);
		}
		Float2 Random::UniformDirection2()
		{
			Float2 direction = Float2::zero;
			while (direction.IsEpsilonZero())
			{
				direction.x = Uniform(-1.0f, 1.0f);
				direction.y = Uniform(-1.0f, 1.0f);
			}
			return direction.Normalize();
		}
		Float3 Random::UniformDirection3()
		{
			Float3 direction = Float3::zero;
			while (direction.IsEpsilonZero())
			{
				direction.x = Uniform(-1.0f, 1.0f);
				direction.y = Uniform(-1.0f, 1.0f);
				direction.z = Uniform(-1.0f, 1.0f);
			}
			return direction.Normalize();
		}
	}
}