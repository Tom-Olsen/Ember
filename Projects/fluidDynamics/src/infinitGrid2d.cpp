#include "infinitGrid2d.h"
#include "vectorSort.h"



namespace emberEngine
{
	// Constructor/Destructor:
	InfinitGrid2d::InfinitGrid2d(int particleCount) : m_particleCount(particleCount)
	{
		m_particleLookup.resize(m_particleCount);
		m_startIndices.resize(m_particleCount);
	}
	InfinitGrid2d::~InfinitGrid2d()
	{

	}



	Int2 InfinitGrid2d::Cell(Float2 position, float radius)
	{
		return Int2(Float2::Floor((position / radius)));
	}
	uint32_t InfinitGrid2d::CellHash(Int2 cell)
	{
		uint32_t a = (uint32_t)cell.x * m_prime0;
		uint32_t b = (uint32_t)cell.y * m_prime1;
		return (a + b);
	}
	uint32_t InfinitGrid2d::CellKey(uint32_t cellHash)
	{
		return cellHash % m_particleCount;
	}



	void InfinitGrid2d::UpdateGrid(std::vector<Float2>& positions, std::vector<Float2>& velocities, float radius)
	{
		for (int i = 0; i < m_particleCount; i++)
		{
			Int2 cell = Cell(positions[i], radius);
			uint32_t cellHash = CellHash(cell);
			uint32_t cellKey = CellKey(cellHash);
			m_particleLookup[i] = cellKey;
		}

		// Sort vectors:
		std::vector<std::size_t> permutation = SortPermutation(m_particleLookup, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_particleLookup = ApplyPermutation(m_particleLookup, permutation);
		positions = ApplyPermutation(positions, permutation);
		velocities = ApplyPermutation(velocities, permutation);

		m_startIndices[m_particleLookup[0]] = 0;
		uint32_t previousValue = m_particleLookup[0];
		for (int i = 1; i < m_particleCount; i++)
		{
			if (previousValue != m_particleLookup[i])
			{
				m_startIndices[m_particleLookup[i]] = i;
				previousValue = m_particleLookup[i];
			}
			else
				m_startIndices[m_particleLookup[i]] = -1;
		}
	}

}