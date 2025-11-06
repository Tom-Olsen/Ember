#include "hashGrid2d.h"
#include "logger.h"
#include "vectorSort.h"



namespace fluidDynamics
{
	// Constructor/Destructor:
	HashGrid2d::HashGrid2d(int particleCount)
	{
		m_particleCount = particleCount;
		m_size = math::NextPrimeAbove(2 * particleCount);
		m_cellKeys.resize(particleCount);
		m_startIndices.resize(m_size, -1);
	}
	HashGrid2d::~HashGrid2d()
	{

	}



	// Geters:
	size_t HashGrid2d::GetSize() const
	{
		return m_size;
	}
	uint32_t HashGrid2d::GetCellKey(int particleIndex) const
	{
		return m_cellKeys[particleIndex];
	}
	uint32_t HashGrid2d::GetStartIndex(int cellKey) const
	{
		return m_startIndices[cellKey];
	}



	// Hashing:
	Int2 HashGrid2d::Cell(Float2 position, float radius)
	{
		return Int2(Float2::Floor((position / radius)));
	}
	int64_t HashGrid2d::CellHash(Int2 cell)
	{
		int64_t hash = (int64_t(cell.x) * m_prime0) ^ (int64_t(cell.y) * m_prime1);
		hash ^= (hash >> 33);
		hash *= 0xff51afd7ed558ccdULL;
		hash ^= (hash >> 33);
		return hash;
	}
	uint32_t HashGrid2d::CellKey(int cellHash)
	{
		return ((cellHash % m_size) + m_size) % m_size;
	}



	// Main hash grid mechanism:
	void HashGrid2d::UpdateGrid(std::vector<Float2>& positions, float radius)
	{
		size_t count = positions.size();
		if (count == 0)
			return;

		if (count != m_particleCount)
		{
			m_particleCount = count;
			m_cellKeys.resize(count);
		}

		// Compute cell keys:
		for (size_t i = 0; i < m_particleCount; i++)
		{
			Int2 cell = Cell(positions[i], radius);
			int64_t cellHash = CellHash(cell);
			m_cellKeys[i] = CellKey(cellHash);
		}

		// Sort vectors:
		m_permutation = math::SortPermutation(m_cellKeys, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_cellKeys = math::ApplyPermutation(m_cellKeys, m_permutation);
		
		// Compute start indices:
		std::fill(m_startIndices.begin(), m_startIndices.end(), -1);
		m_startIndices[m_cellKeys[0]] = 0;
		for (size_t i = 1; i < m_particleCount; i++)
		{
			if (m_cellKeys[i] != m_cellKeys[i - 1])
				m_startIndices[m_cellKeys[i]] = i;
		}
	}
	void HashGrid2d::ApplySort(std::vector<Float2>& data)
	{
		data = math::ApplyPermutation(data, m_permutation);
	}
}