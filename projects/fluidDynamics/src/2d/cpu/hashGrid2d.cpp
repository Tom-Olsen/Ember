#include "hashGrid2d.h"
#include "logger.h"
#include "vectorSort.h"



namespace fluidDynamics
{
	// Constructor/Destructor:
	HashGrid2d::HashGrid2d(int particleCount)
	{
		m_particleCount = particleCount;
		m_size = math::NextPrimeAbove(2 * particleCount);	// better hashing (fever collosions of cell hashes).
		m_cellKeys.resize(particleCount);
		m_startIndices.resize(m_size, uint32_t(-1));

		// Note on start indices array:
		// Start indices vector is sparse and mostly contains invalid (-1) entries.
		// Each access to this array is done by m_startIndices[CellKey(Cellhash(cell(position,radius)))],
		// and the celll keys range from 0 to m_size-1.
	}
	HashGrid2d::~HashGrid2d()
	{

	}



	// Getters:
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



	// Getters by cell:
	uint32_t HashGrid2d::GetCellKey(Int2 cell) const
	{
		int64_t cellHash = CellHash(cell);
		return CellKey(cellHash);
	}
	uint32_t HashGrid2d::GetStartIndex(Int2 cell) const
	{
		uint32_t cellKey = GetCellKey(cell);
		return m_startIndices[cellKey];
	}



	// Hashing:
	Int2 HashGrid2d::Cell(Float2 position, float radius) const
	{
		return Int2(Float2::Floor((position / radius)));
	}
	int64_t HashGrid2d::CellHash(Int2 cell) const
	{
		int64_t hash = (int64_t(cell.x) * m_prime0) ^ (int64_t(cell.y) * m_prime1);
		hash ^= (hash >> 33);
		hash *= 0xff51afd7ed558ccdULL;
		hash ^= (hash >> 33);
		return hash;
	}
	uint32_t HashGrid2d::CellKey(int cellHash) const
	{
		return ((cellHash % m_size) + m_size) % m_size;
	}



	// Main hash grid mechanism:
	void HashGrid2d::UpdateGrid(std::vector<Float2>& positions, float radius)
	{
		if (m_particleCount != positions.size())
		{
			LOG_ERROR("HashGrid2d::UpdateGrid: positions.size()={} must be equal to m_particleCount={}", positions.size(), m_particleCount);
			return;
		}

		// Compute cell keys:
		for (size_t i = 0; i < m_particleCount; i++)
		{
			Int2 cell = Cell(positions[i], radius);
			int64_t cellHash = CellHash(cell);
			m_cellKeys[i] = CellKey(cellHash);
		}

		// Sort vectors:
		m_sortPermutation = math::SortPermutation(m_cellKeys, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_cellKeys = math::ApplyPermutation(m_cellKeys, m_sortPermutation);
		
		// Compute start indices:
		std::fill(m_startIndices.begin(), m_startIndices.end(), uint32_t(-1));
		m_startIndices[m_cellKeys[0]] = 0;
		for (size_t i = 1; i < m_particleCount; i++)
		{
			if (m_cellKeys[i] != m_cellKeys[i - 1])
				m_startIndices[m_cellKeys[i]] = i;
		}
		
		// Result:
		// m_cellKeys(sorted) :  [0, 0, 0, 2, 2, 3, 3, 3, 4, 5]
		// m_startIndices[...] = { 0, -1, 3, 5, 8, 9, -1, ... }	here particle with cell key 1 does not exist so it gets start index -1.
		// meaning: particles with cellKey i start at the i's entry of m_startIndices (-1 means no particle has the cellKey i).
		
		// That means:
		// Cell0 -> particles[0...2]
		// Cell1 -> particles[3...4]
		// Cell2 -> particles[5...7]
		// Cell3 -> particles[8]
		// Cell4 -> particles[9]

		// When querying neighbors, you do:
		// Compute the cell of a particle -> Int2 cell
		//	For all neighboring cells(cell + offset), compute their hashes -> CellKey(...)
		//	For each of those cell keys:
		//		Look up startIndex = m_startIndices[cellKey]
		// 		If startIndex == -1, cell is empty -> skip
		// 		Otherwise, iterate through the contiguous block in m_cellKeys until the key changes
		// 		Those are your neighbor particles.
	}
	void HashGrid2d::ApplySort(std::vector<Float2>& data) const
	{
		data = math::ApplyPermutation(data, m_sortPermutation);
	}
}