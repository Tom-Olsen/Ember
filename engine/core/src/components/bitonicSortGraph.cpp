#include "bitonicSortGraph.h"
#include "bitonicSortGraphEditorWindow.h"



namespace emberEngine
{
	// Constructor/Destructor:
	BitonicSortGraph::BitonicSortGraph()
	{
		// Local Bitonic 64:
		//count = 64;
		//width = 10;
		//height = 5;
		//lineWidth = 0.015f;
		//lineGap = 0.03f;
		//blockGap = 0.125f;

		count = 64;
		width = 10;
		height = 5;
		lineWidth = 0.015f;
		lineGap = 0.03f;
		blockGap = 0.125f;

		pMaterial = MaterialManager::GetMaterial("simpleUnlitMaterial");
		editorWindow = std::make_unique<BitonicSortGraphEditorWindow>(this);
	}
	BitonicSortGraph::~BitonicSortGraph()
	{

	}


	// Overrides:
	void BitonicSortGraph::DrawFlip(int k, int index, float x)
	{
		int q = ((2 * index) / k) * k;
		int kHalf = k / 2;
		int mod = index % kHalf;
		int index0 = q + mod;
		int index1 = q - mod - 1 + k;

		float t0 = index0 / (count - 1.0f);
		float t1 = index1 / (count - 1.0f);
		float y0 = height * (0.5f - t0);
		float y1 = height * (0.5f - t1);
		Float3 pos0 = Float3(GetTransform()->GetLocalToWorldMatrix() * Float4(x, y0, 0.1, 1));
		Float3 pos1 = Float3(GetTransform()->GetLocalToWorldMatrix() * Float4(x, y1, 0.1, 1));
		Graphics::DrawSphere(pos0, 3 * lineWidth, Float4::red, false, false);
		Graphics::DrawSphere(pos1, 3 * lineWidth, Float4::red, false, false);
		Graphics::DrawLineSegment(pos0, pos1, lineWidth, 0.8f * Float4::red, false, false);
	}
	void BitonicSortGraph::DrawDisperse(int k, int index, float x)
	{
		int q = ((2 * index) / k) * k;
		int kHalf = k / 2;
		int mod = index % kHalf;
		int index0 = q + mod;
		int index1 = q + mod + kHalf;

		float t0 = index0 / (count - 1.0f);
		float t1 = index1 / (count - 1.0f);
		float y0 = height * (0.5f - t0);
		float y1 = height * (0.5f - t1);
		Float3 pos0 = Float3(GetTransform()->GetLocalToWorldMatrix() * Float4(x, y0, 0.1, 1));
		Float3 pos1 = Float3(GetTransform()->GetLocalToWorldMatrix() * Float4(x, y1, 0.1, 1));
		Graphics::DrawSphere(pos0, 3 * lineWidth, Float4::orange, false, false);
		Graphics::DrawSphere(pos1, 3 * lineWidth, Float4::orange, false, false);
		Graphics::DrawLineSegment(pos0, pos1, lineWidth, 0.8f * Float4::orange, false, false);
	}
	void BitonicSortGraph::DrawHorizontalLines()
	{
		for (int i = 0; i < count; i++)
		{
			float t = i / (count - 1.0f);	// �[0,1]
			float z = height * (0.5f - t);
			Float3 start(-0.5f * width, 0.0f, z);
			Float3 end(0.5f * width, 0.0f, z);
			Graphics::DrawLineSegment(start, end, lineWidth, 0.8f * Float4::white, false, false);
		}
	}
	void BitonicSortGraph::DrawLocalBitonicSort(float& x, int startIndex, int localCount)
	{
		float xStart = x;
		for (int i = startIndex; i < localCount / 2 + startIndex; i++)
		{
			float x = xStart;
			for (int k = 2; k <= localCount; k *= 2)
			{
				DrawFlip(k, i, x + ((2 * i) % k) * lineGap);
				x += k * lineGap + blockGap;
				for (int kk = k / 2; kk > 1; kk /= 2)
				{
					DrawDisperse(kk, i, x + ((2 * i) % kk) * lineGap);
					x += kk * lineGap + blockGap;
				}
			}
		}
	}
	void BitonicSortGraph::DrawBigFlip(float& x)
	{

	}
	void BitonicSortGraph::Update()
	{
		DrawHorizontalLines();

		//float x = -0.48f * width;
		//DrawLocalBitonicSort(x, 0, count);

		//float x = -0.48f * width;
		//DrawLocalBitonicSort(x, 0, count / 2);
		//x = -0.48f * width;
		//DrawLocalBitonicSort(x, count / 4, count / 2);

		float x = -0.48f * width;
		DrawLocalBitonicSort(x, 0, count / 4);
		x = -0.48f * width;
		DrawLocalBitonicSort(x, 1 * count / 8, count / 4);
		x = -0.48f * width;
		DrawLocalBitonicSort(x, 2 * count / 8, count / 4);
		x = -0.48f * width;
		DrawLocalBitonicSort(x, 3 * count / 8, count / 4);
	}
	const std::string BitonicSortGraph::ToString() const
	{
		return "BitonicSortGraph";
	}
}