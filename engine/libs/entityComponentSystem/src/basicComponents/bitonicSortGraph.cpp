#include "bitonicSortGraph.h"
#include "debugRenderer.h"
#include "emberMath.h"
#include "shaderProperties.h"
#include "transform.h"
using namespace emberCore;



namespace emberEcs
{
	// Constructor/Destructor:
	BitonicSortGraph::BitonicSortGraph()
	{
		count = 64;
		width = 10;
		height = 5;
		lineWidth = 0.015f;
		lineGap = 0.03f;
		blockGap = 0.125f;
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
		DebugRenderer::SetReceiveShadows(false);
		DebugRenderer::SetCastShadows(false);
		DebugRenderer::SetColor(Float4::red);
		DebugRenderer::DrawSphere(Float4x4::TRS(pos0, Float3x3::identity, 3.0f * lineWidth));
		DebugRenderer::DrawSphere(Float4x4::TRS(pos1, Float3x3::identity, 3.0f * lineWidth));
		DebugRenderer::SetColor(0.8f * Float4::red);
		DebugRenderer::DrawLineSegment(pos0, pos1, lineWidth);
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
		DebugRenderer::SetReceiveShadows(false);
		DebugRenderer::SetCastShadows(false);
		DebugRenderer::SetColor(Float4::orange);
		DebugRenderer::DrawSphere(Float4x4::TRS(pos0, Float3x3::identity, 3.0f * lineWidth));
		DebugRenderer::DrawSphere(Float4x4::TRS(pos1, Float3x3::identity, 3.0f * lineWidth));
		DebugRenderer::SetColor(0.8f * Float4::orange);
		DebugRenderer::DrawLineSegment(pos0, pos1, lineWidth);
	}
	void BitonicSortGraph::DrawHorizontalLines()
	{
        DebugRenderer::SetReceiveShadows(false);
        DebugRenderer::SetCastShadows(false);
        DebugRenderer::SetColor(0.8f * Float4::white);
		for (int i = 0; i < count; i++)
		{
			float t = i / (count - 1.0f);	// €[0,1]
			float z = height * (0.5f - t);
			Float3 start(-0.5f * width, 0.0f, z);
			Float3 end(0.5f * width, 0.0f, z);
			DebugRenderer::DrawLineSegment(start, end, lineWidth);
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
}