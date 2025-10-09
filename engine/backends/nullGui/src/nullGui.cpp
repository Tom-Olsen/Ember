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



	// Public methods:
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



	// Wrappers:
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return false;
	}
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return false;
	}
	void Gui::End()
	{

	}
	void Gui::PushID(const char* str_id)
	{

	}
	void Gui::PopID()
	{

	}
	Float2 Gui::GetWindowSize()
	{
		return Float2::zero;
	}
	Float2 Gui::GetContentRegionalAvail()
	{
		return Float2::zero;
	}
	Float2 Gui::GetCursorPos()
	{
		return Float2::zero;
	}
	void Gui::SetCursorPos(const Float2& localPos)
	{

	}
	void Gui::Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1)
	{

	}
}