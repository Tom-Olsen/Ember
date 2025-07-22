#include "hashGrid2d.h"
#include "logger.h"
#include "vectorSort.h"



namespace emberEngine
{
	// Constructor/Destructor:
	HashGrid2d::HashGrid2d(int particleCount) : m_particleCount(particleCount)
	{
		m_cellKeys.resize(m_particleCount);
		m_startIndices.resize(m_particleCount);
	}
	HashGrid2d::~HashGrid2d()
	{

	}



	Int2 HashGrid2d::Cell(Float2 position, float radius)
	{
		return Int2(Float2::Floor((position / radius)));
	}
	int HashGrid2d::CellHash(Int2 cell)
	{
		int a = cell.x * m_prime0;
		int b = cell.y * m_prime1;
		return a + b;
	}
	uint32_t HashGrid2d::CellKey(int cellHash)
	{
		if (cellHash < 0)
			return m_particleCount - (abs(cellHash) % m_particleCount);
		else
			return cellHash % m_particleCount;
	}
	uint32_t HashGrid2d::GetCellKey(int particleIndex)
	{
		return m_cellKeys[particleIndex];
	}
	uint32_t HashGrid2d::GetStartIndex(int cellKey)
	{
		return m_startIndices[cellKey];
	}



	void HashGrid2d::UpdateGrid(std::vector<Float2>& positions, float radius)
	{
		// Fill cellKeys vector:
		for (int i = 0; i < m_particleCount; i++)
		{
			Int2 cell = Cell(positions[i], radius);
			int cellHash = CellHash(cell);
			m_cellKeys[i] = CellKey(cellHash);
		}

		// Sort vectors:
		m_permutation = math::SortPermutation(m_cellKeys, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_cellKeys = math::ApplyPermutation(m_cellKeys, m_permutation);
		positions = math::ApplyPermutation(positions, m_permutation);

		// Reset start indices to invalid state:
		for (int i = 0; i < m_particleCount; i++)
			m_startIndices[i] = -1;
		
		// Fill start indices vector:
		m_startIndices[m_cellKeys[0]] = 0;
		for (int i = 1; i < m_particleCount; i++)
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