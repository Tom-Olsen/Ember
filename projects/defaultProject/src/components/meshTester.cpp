#include "meshTester.h"
#include "meshRenderer.h"
using namespace emberCore;
using namespace emberEcs;
using namespace emberCommon;



namespace emberEngine
{
	// Constructor/Destructor:
	MeshTester::MeshTester(MeshRenderer* pMeshRenderer)
	{
		m_pMeshRenderer = pMeshRenderer;
		index = 0;
	}
	MeshTester::~MeshTester()
	{

	}



	// Public Methods:
	void MeshTester::AddMesh(Mesh* pMesh)
	{
		m_meshes.push_back(pMesh);
	}



	// Overrides:
	void MeshTester::Start()
	{
		m_pMeshRenderer->SetMesh(*m_meshes[index]);
	}
	void MeshTester::Update()
	{
		if (EventSystem::KeyDown(Input::Key::Plus))
		{
			index = (index + 1) % m_meshes.size();
			m_pMeshRenderer->SetMesh(*m_meshes[index]);
		}
		if (EventSystem::KeyDown(Input::Key::Minus))
		{
			index = (index - 1 + m_meshes.size()) % m_meshes.size();
			m_pMeshRenderer->SetMesh(*m_meshes[index]);
		}
	}
}