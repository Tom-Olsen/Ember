#include "infinitGrid2d.h"
#include "logger.h"
#include "vectorSort.h"



namespace emberEngine
{
	// Constructor/Destructor:
	InfinitGrid2d::InfinitGrid2d(int particleCount) : m_particleCount(particleCount)
	{
		m_cellKeys.resize(m_particleCount);
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
	uint32_t InfinitGrid2d::GetCellKey(int particleIndex)
	{
		return m_cellKeys[particleIndex];
	}
	uint32_t InfinitGrid2d::GetStartIndex(int cellKey)
	{
		return m_startIndices[cellKey];
	}



	void InfinitGrid2d::UpdateGrid(std::vector<Float2>& positions, float radius)
	{
		// Fill cellKeys vector:
		for (int i = 0; i < m_particleCount; i++)
		{
			Int2 cell = Cell(positions[i], radius);
			uint32_t cellHash = CellHash(cell);
			m_cellKeys[i] = CellKey(cellHash);
		}

		// Sort vectors:
		m_permutation = SortPermutation(m_cellKeys, [](uint32_t const& a, uint32_t const& b) { return a < b; });
		m_cellKeys = ApplyPermutation(m_cellKeys, m_permutation);
		positions = ApplyPermutation(positions, m_permutation);

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
	void InfinitGrid2d::ApplySort(std::vector<Float2>& data)
	{
		data = ApplyPermutation(data, m_permutation);
	}
}