#include "gui.h"
#include "commonGuiFlags.h"
#include "emberMath.h"
#include "editor.h"
#include "iGui.h"
#include "iTexture.h"
#include "texture2d.h"



namespace emberEngine
{
	// Static members:
	bool Gui::s_isInitialized = false;
	float Gui::s_intDragSensitivityBase = 0.1f;
	float Gui::s_floatDragSensitivityBase = 0.01f;
	float Gui::s_dragSensitivityExponent = 1.5f;
	float Gui::s_minWidgetWidth = 50.0f;
	float Gui::s_labelPercentile = 0.4f;			// percentile of window width used for labels.
	std::unique_ptr<emberBackendInterface::IGui> Gui::s_pIGui;
	emberBackendInterface::IGui* Gui::GetInterfaceHandle()
	{
		return s_pIGui.get();
	}



	// Initialization/Cleanup:
	void Gui::Init(emberBackendInterface::IGui* pIGui)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pIGui = std::unique_ptr<emberBackendInterface::IGui>(pIGui);
	}
	void Gui::Clear()
	{
		s_pIGui.reset();
		s_isInitialized = false;
	}



	// Render Logic:
	void Gui::Update()
	{
		s_pIGui->Update();
	}
	void Gui::ProcessEvent(const void* pEvent)
	{
		s_pIGui->ProcessEvent(pEvent);
	}
	void Gui::Render(VkCommandBuffer vkCommandBuffer)
	{
		s_pIGui->Render(vkCommandBuffer);
	}



	// Getters:
	bool Gui::WantCaptureKeyboard()
	{
		return s_pIGui->WantCaptureKeyboard();
	}
	bool Gui::WantCaptureMouse()
	{
		return s_pIGui->WantCaptureMouse();
	}
	uintptr_t Gui::GetTextureID(Texture& texture)
	{
		emberBackendInterface::ITexture* pITexture = texture.GetInterfaceHandle();
		return s_pIGui->GetTextureID(pITexture);
	}
	Float2 Gui::GetWindowSize()
	{
		return s_pIGui->GetWindowSize();
	}
	Float2 Gui::GetContentRegionAvail()
	{
		return s_pIGui->GetContentRegionAvail();
	}
	Float2 Gui::GetCursorPos()
	{
		return s_pIGui->GetCursorPos();
	}
	Float2 Gui::GetCursorScreenPos()
	{
		return s_pIGui->GetCursorScreenPos();
	}
	Float2 Gui::GetMouseDragDelta(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		return s_pIGui->GetMouseDragDelta(button, lockThreshold);
	}
	emberCommon::GuiStyle Gui::GetStyle()
	{
		return s_pIGui->GetStyle();
	}



	// Setters:
	void Gui::SetCursorPos(const Float2& localPos)
	{
		s_pIGui->SetCursorPos(localPos);
	}
	void Gui::SetCursorScreenPos(const Float2& pos)
	{
		s_pIGui->SetCursorScreenPos(pos);
	}
	void Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton button)
	{
		s_pIGui->ResetMouseDragDelta(button);
	}



	// Window management:
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return s_pIGui->Begin(name, pOpen, flags);
	}
	void Gui::End()
	{
		s_pIGui->End();
	}
	void Gui::PushID(const char* strId)
	{
		s_pIGui->PushID(strId);
	}
	void Gui::PopID()
	{
		s_pIGui->PopID();
	}
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return s_pIGui->IsWindowFocused(flags);
	}



	// Layout:
	void Gui::SameLine(float offsetFromStartX, float spacingW)
	{
		s_pIGui->SameLine(offsetFromStartX, spacingW);
	}
	void Gui::SetNextItemWidth(float itemWidth)
	{
		s_pIGui->SetNextItemWidth(itemWidth);
	}
	Float2 Gui::CalcTextSize(const char* text, const char* textEnd, bool hideTextAfterDoubleHash, float wrapWidth)
	{
		return s_pIGui->CalcTextSize(text, textEnd, hideTextAfterDoubleHash, wrapWidth);
	}



	// State checks:
	bool Gui::IsItemActive()
	{
		return s_pIGui->IsItemActive();
	}
	bool Gui::IsItemActivated()
	{
		return s_pIGui->IsItemActivated();
	}
	bool Gui::IsMouseDragging(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		return s_pIGui->IsMouseDragging(button, lockThreshold);
	}



	// Basic Widgets:
	void Gui::TextUnformatted(const char* text, const char* textEnd)
	{
		return s_pIGui->TextUnformatted(text, textEnd);
	}
	//void TextV(const char* format, va_list args); 	// not sure if i want to expose this yet.
	void Gui::Text(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		s_pIGui->TextV(format, args); // forward via TextV, which accepts va_list
		va_end(args);
	}
	bool Gui::Button(const char* label, const Float2& size)
	{
		return s_pIGui->Button(label, size);
	}
	bool Gui::InvisibleButton(const char* strID, const Float2& size, emberCommon::GuiButtonFlags flags)
	{
		return s_pIGui->InvisibleButton(strID, size, flags);
	}
	void Gui::Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0, const Float2& uv1)
	{
		s_pIGui->Image(textureID, imageSize, uv0, uv1);
	}



	// Custom Widgets:
	bool Gui::Checkbox(const std::string& label, bool* value)
	{
		// Draw label:
		TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		Gui::SameLine();
		Float2 cursorPos = Gui::GetCursorPos();
		cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
		Gui::SetCursorPos(cursorPos);

		bool changed = false;
		Gui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			Gui::SetNextItemWidth(inputWidth);
			changed = s_pIGui->Checkbox(("##" + label).c_str(), value);
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputInt(const std::string& label, int* value, int step, int stepFast, emberCommon::GuiWindowFlags flags)
	{
		// Draw label:
		Gui::TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		Gui::SameLine();
		Float2 cursorPos = Gui::GetCursorPos();
		cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
		Gui::SetCursorPos(cursorPos);

		bool changed = false;
		Gui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			Gui::SetNextItemWidth(inputWidth);
			changed = s_pIGui->InputInt("##Input", value, step, stepFast, flags);
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::DragInt(const std::string& label, int* value, int step, int stepFast, emberCommon::GuiWindowFlags flags)
	{
		static float accumulator;
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Float2 textSize = Gui::CalcTextSize(label.c_str());
			Float2 cursorPos = Gui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			bool mouseUp = Gui::InvisibleButton("##drag_text", textSize);
			bool mouseDown = Gui::IsItemActivated();
			bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton_Left);

			// Draw text above invisible button:
			Gui::SetCursorScreenPos(cursorPos);
			Gui::TextUnformatted(label.c_str());

			// On initial interaction get int value:
			if (mouseDown)
				accumulator = *value;

			// Change current value on dragging motion on label:
			if (dragging)
			{
				Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton_Left);

				float sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				accumulator += sign * sensitivity;
				*value = static_cast<int>(std::round(accumulator));

				Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton_Left);
			}

			// Move cursor to label|fields seperator:
			Gui::SameLine();
			cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			// Float input field:
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);
			Gui::SetNextItemWidth(inputWidth);
			bool interaction = s_pIGui->InputInt("##Input", value, step, stepFast, flags);

			changed = dragging || interaction;
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputFloat(const std::string& label, float* value, float step, float stepFast, const char* format, emberCommon::GuiWindowFlags flags)
	{
		// Draw label:
		Gui::TextUnformatted(label.c_str());

		// Move cursor to label|fields seperator:
		Gui::SameLine();
		Float2 cursorPos = Gui::GetCursorPos();
		cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
		Gui::SetCursorPos(cursorPos);

		bool changed = false;
		Gui::PushID(label.c_str());
		{
			// Clamp input width:
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);

			// Draw input field:
			Gui::SetNextItemWidth(inputWidth);
			changed = s_pIGui->InputFloat("##Input", value, step, stepFast, format, flags);
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::DragFloat(const std::string& label, float* value, float step, float stepFast, const char* format, emberCommon::GuiWindowFlags flags)
	{
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Float2 textSize = Gui::CalcTextSize(label.c_str());
			Float2 cursorPos = Gui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			bool clicked = Gui::InvisibleButton("##drag_text", textSize);
			bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton_Left);

			// Draw text above invisible button:
			Gui::SetCursorScreenPos(cursorPos);
			Gui::TextUnformatted(label.c_str());

			// Change current value on dragging motion on label:
			if (dragging)
			{
				Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton_Left);

				float sensitivity = s_floatDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				*value += sign * sensitivity;

				Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton_Left);
			}

			// Move cursor to label|fields seperator:
			Gui::SameLine();
			cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			// Float input field:
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);
			Gui::SetNextItemWidth(inputWidth);
			bool interaction = s_pIGui->InputFloat("##Input", value, step, stepFast, format, flags);

			changed = dragging || interaction;
		}
		Gui::PopID();
		return changed;
	}
}