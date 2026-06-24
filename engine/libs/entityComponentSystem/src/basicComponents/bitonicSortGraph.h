#pragma once
#include "component.h"



namespace emberEcs
{
	// Renders bitonic sort graph for debugging.
	class BitonicSortGraph : public Component
	{
	public: // Members:
		int count;
		float width;
		float height;
		float lineWidth;
		float lineGap;
		float blockGap;

	private: // Methods:
		void DrawFlip(int k, int index, float x);
		void DrawDisperse(int k, int index, float x);
		void DrawHorizontalLines();
		void DrawLocalBitonicSort(float& x, int startIndex, int localCount);
		void DrawBigFlip(float& x);

	public: // Methods:
		BitonicSortGraph();
		~BitonicSortGraph();

		// Overrides:
		void Update() override;
	};
}