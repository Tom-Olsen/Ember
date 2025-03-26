#ifndef __INCLUDE_GUARD_testEmberRandom_h__
#define __INCLUDE_GUARD_testEmberRandom_h__



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
		0.039759498f,
		0.20456028f,
		0.8264361f,
		0.567725f,
		0.5320779f,
	};
	for (int i = 0; i < 10; i++)
	{
		float random = math::Random::Uniform01();
		EXPECT_FLOAT_EQ(random, prediction[i]);
	}
}



#endif // __INCLUDE_GUARD_testEmberRandom_h__