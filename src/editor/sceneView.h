#ifndef __INCLUDE_GUARD_sceneView_h__
#define __INCLUDE_GUARD_sceneView_h__



namespace emberEditor
{
	class SceneView
	{
	public: // Members

	private: // Members

	public: // Methods
		SceneView();
		~SceneView();

		void DrawWindow(bool* pOpen);

	private: // Methods
		void MultiSelect();
	};
}



#endif // __INCLUDE_GUARD_sceneView_h__