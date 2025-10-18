#include "nullGui.h"



namespace nullGuiBackend
{
	// Public methods:
	// Constructor/Destructor:
	Gui::Gui()
	{

	}
	Gui::~Gui()
	{

	}



	// Render Logic:
	void Gui::Update()
	{

	}
	void Gui::ProcessEvent(const void* pWindowEvent)
	{

	}
	void Gui::Render(VkCommandBuffer vkCommandBuffer)
	{

	}



	// Getters:
	bool Gui::WantCaptureKeyboard()
	{
		return false;
	}
	bool Gui::WantCaptureMouse()
	{
		return false;
	}
	uintptr_t Gui::GetTextureID(emberBackendInterface::ITexture* pITexture)
	{
		return 0;
	}
	Float2 Gui::GetWindowSize()
	{
		return Float2::zero;
	}
	Float2 Gui::GetContentRegionAvail()
	{
		return Float2::zero;
	}
	Float2 Gui::GetCursorPos()
	{
		return Float2::zero;
	}
	Float2 Gui::GetCursorScreenPos()
	{
		return Float2::zero;
	}
	Float2 Gui::GetMouseDragDelta(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		return Float2::zero;
	}
	emberCommon::GuiStyle Gui::GetStyle() const
	{
		return emberCommon::GuiStyle();
	}



	// Setters:
	void Gui::SetEditorCallbacks(emberBackendInterface::EditorRenderCallback renderCallback, emberBackendInterface::EditorCaptureQueryCallback captureCallback)
	{

	}
	void Gui::SetCursorPos(const Float2& localPos)
	{

	}
	void Gui::SetCursorScreenPos(const Float2& pos)
	{

	}
	void Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton button)
	{

	}



	// Window management:
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return false;
	}
	void Gui::End()
	{

	}
	void Gui::PushID(const char* strID)
	{

	}
	void Gui::PopID()
	{

	}
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return false;
	}



	// Layout:
	void Gui::SameLine(float offsetFromStartX, float spacingW)
	{

	}
	void Gui::SetNextItemWidth(float itemWidth)
	{

	}
	Float2 Gui::CalcTextSize(const char* text, const char* textEnd, bool hideTextAfterDoubleHash, float wrapWidth)
	{
		return Float2::zero;
	}



	// State checks:
	bool Gui::IsItemActive()
	{
		return false;
	}
	bool Gui::IsItemActivated()
	{
		return false;
	}
	bool Gui::IsMouseDragging(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		return false;
	}



	// Widgets:
	bool Gui::Checkbox(const char* label, bool* value)
	{
		return false;
	}
	bool Gui::InputInt(const char* label, int* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		return false;
	}
	bool Gui::InputFloat(const char* label, float* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		return false;
	}
	void Gui::TextUnformatted(const char* text, const char* textEnd)
	{

	}
	void Gui::TextV(const char* format, va_list args)
	{

	}
	bool Gui::Button(const char* label, const Float2& size)
	{
		return false;
	}
	bool Gui::InvisibleButton(const char* strId, const Float2& size, emberCommon::GuiButtonFlags flags)
	{
		return false;
	}
	void Gui::Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0, const Float2& uv1)
	{

	}
}