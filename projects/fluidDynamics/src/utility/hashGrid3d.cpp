#include "hashGrid3d.h"
#include "logger.h"
#include "vectorSort.h"



namespace fluidDynamics
{
	// Public methods:
	// Constructor/Destructor:
	HashGrid3d::HashGrid3d()
	{
		m_particleCount = 0;
		m_size = 0;
		m_inverseValid = false;
	}
	HashGrid3d::HashGrid3d(int particleCount)
	{
		Reallocate(particleCount);
		m_inverseValid = false;
	}
	HashGrid3d::~HashGrid3d()
	{

	}
	void HashGrid3d::Reallocate(int particleCount)
	{
		m_particleCount = particleCount;
		m_size = math::NextPrimeAbove(2 * m_particleCount);
		m_cellKeys.resize(m_particleCount);
		m_startIndices.resize(m_size, uint32_t(-1));
		m_sortPermutation.resize(m_particleCount);
		m_inverseValid = false;
		// Note on start indices array:
		// Start indices vector is sparse and mostly contains invalid (-1) entries.
		// Each access to this array is done by m_startIndices[CellKey(Cellhash(cell(position,radius)))],
		// and the celll keys range from 0 to m_size-1.
	}



	// Getters:
	size_t HashGrid3d::GetSize() const
	{
		return m_size;
	}
	uint32_t HashGrid3d::GetCellKey(int particleIndex) const
	{
		return m_cellKeys[particleIndex];
	}
	uint32_t HashGrid3d::GetStartIndex(int cellKey) const
	{
		return m_startIndices[cellKey];
	}



	// Getters by cell:
	uint32_t HashGrid3d::GetCellKey(Int3 cell) const
	{
		uint32_t cellHash = CellHash(cell);
		return CellKey(cellHash);
	}
	uint32_t HashGrid3d::GetStartIndex(Int3 cell) const
	{
		uint32_t cellKey = GetCellKey(cell);
		return m_startIndices[cellKey];
	}



	// Hashing:
	Int3 HashGrid3d::Cell(Float3 position, float radius) const
	{
		return Int3(Float3::Floor(position / radius));
	}
	uint32_t HashGrid3d::CellHash(Int3 cell) const
	{
		uint32_t hash = (uint32_t(cell.x) * m_prime0) + (uint32_t(cell.y) * m_prime1) + (uint32_t(cell.z) * m_prime2);
		hash ^= hash >> 16;
		hash *= 0x85ebca6b;
		hash ^= hash >> 13;
		hash *= 0xc2b2ae35;
		hash ^= hash >> 16;
		return hash;
	}
	uint32_t HashGrid3d::CellKey(uint32_t cellHash) const
	{
		return cellHash % m_size;
	}



	// Main hash grid mechanism:
	void HashGrid3d::UpdateGrid(std::vector<Float3>& positions, float radius)
	{
		if (m_particleCount != positions.size())
		{
			LOG_ERROR("HashGrid3d::UpdateGrid: positions.size()={} must be equal to m_particleCount={}", positions.size(), m_particleCount);
			return;
		}

		// Compute cell keys:
		for (size_t i = 0; i < m_particleCount; i++)
		{
			Int3 cell = Cell(positions[i], radius);
			uint32_t cellHash = CellHash(cell);
			m_cellKeys[i] = CellKey(cellHash);
		}

		// Sort vectors:
		math::SortPermutation(m_cellKeys, m_sortPermutation, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_cellKeys = math::ApplyPermutation(m_cellKeys, m_sortPermutation);

		// Compute start indices:
		std::fill(m_startIndices.begin(), m_startIndices.end(), uint32_t(-1));
		m_startIndices[m_cellKeys[0]] = 0;
		for (size_t i = 1; i < m_particleCount; i++)
		{
			if (m_cellKeys[i] != m_cellKeys[i - 1])
				m_startIndices[m_cellKeys[i]] = i;
		}
		m_inverseValid = false;

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
		// Compute the cell of a particle -> Int3 cell
		//	For all neighboring cells(cell + offset), compute their hashes -> CellKey(...)
		//	For each of those cell keys:
		//		Look up startIndex = m_startIndices[cellKey]
		// 		If startIndex == -1, cell is empty -> skip
		// 		Otherwise, iterate through the contiguous block in m_cellKeys until the key changes
		// 		Those are your neighbor particles.
	}
	void HashGrid3d::ApplySort(std::vector<float>& data) const
	{
		data = math::ApplyPermutation(data, m_sortPermutation);
	}
	void HashGrid3d::ApplySort(std::vector<Float3>& data) const
	{
		data = math::ApplyPermutation(data, m_sortPermutation);
	}
	void HashGrid3d::UndoSort(std::vector<float>& data)
	{
		if (!m_inverseValid)
		{
			math::InvertPermutation(m_sortPermutation, m_inverseSortPermutation);
			m_inverseValid = true;
		}
		data = math::ApplyPermutation(data, m_inverseSortPermutation);
	}
	void HashGrid3d::UndoSort(std::vector<Float3>& data)
	{
		if (!m_inverseValid)
		{
			math::InvertPermutation(m_sortPermutation, m_inverseSortPermutation);
			m_inverseValid = true;
		}
		data = math::ApplyPermutation(data, m_inverseSortPermutation);
	}
}