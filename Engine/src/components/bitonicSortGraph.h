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
	private: // Members:
		Material* pMaterial;
		std::unique_ptr<BitonicSortGraphEditorWindow> editorWindow;


	public: // Methods:
		BitonicSortGraph();
		~BitonicSortGraph();

		// Overrides:
		void Update() override;
		const std::string ToString() const override;
	};
}



#endif // __INCLUDE_GUARD_bitonicSortGraph_h__