#include "bitonicSortGraph.h"
#include "bitonicSortGraphEditorWindow.h"



namespace emberEngine
{
	// Constructor/Destructor:
	BitonicSortGraph::BitonicSortGraph()
	{
		count = 16;
		width = 10;
		height = 5;
		lineWidth = 0.1f;
		pMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
		editorWindow = std::make_unique<BitonicSortGraphEditorWindow>(this);
	}
	BitonicSortGraph::~BitonicSortGraph()
	{

	}


	// Overrides:
	void BitonicSortGraph::Update()
	{
		for (int i = 0; i < count; i++)
		{
			float t = i / (count - 1.0f);	// €[0,1]
			float z = height * (0.5f - t);
			Float3 start(-0.5f * width, 0.0f, z);
			Float3 end(0.5f * width, 0.0f, z);
			ShaderProperties* shaderProperties = Graphics::DrawLineSegment(start, end, lineWidth, pMaterial, false, false);
			shaderProperties->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		}
	}
	const std::string BitonicSortGraph::ToString() const
	{
		return "BitonicSortGraph";
	}
}