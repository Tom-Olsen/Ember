#ifndef __INCLUDE_GUARD_bitonicSortGraph_h__
#define __INCLUDE_GUARD_bitonicSortGraph_h__
#include "emberEngine.h"



namespace emberEngine
{
	// Forward declerations:
	struct BitonicSortGraphEditorWindow;



	class BitonicSortGraph : public Component
	{
	public: // Members:
		int count;
		float width;
		float height;
		float lineWidth;
		float lineGap;
		float blockGap;
	private: // Members:
		Material* pMaterial;
		std::unique_ptr<BitonicSortGraphEditorWindow> editorWindow;
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
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_bitonicSortGraph_h__