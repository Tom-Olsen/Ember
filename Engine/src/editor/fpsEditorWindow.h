#ifndef __INCLUDE_GUARD_fpsEditorWindow_h__
#define __INCLUDE_GUARD_fpsEditorWindow_h__
#include "editorWindow.h"



namespace emberEngine
{
	class FpsEditorWindow : EditorWindow
	{
	public: // Methods:
		void Render() const override;
	};
}



#endif // __INCLUDE_GUARD_fpsEditorWindow_h__