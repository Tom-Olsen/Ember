#include "gui.h"
#include "commonGuiFlags.h"
#include "emberMath.h"
#include "editor.h"
#include "iGui.h"
#include "iTexture.h"
#include "texture2d.h"
#include <algorithm>
#include <limits>



namespace emberCore
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



	// Public methods:
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
	Float2 Gui::GetMousePos()
	{
		return s_pIGui->GetMousePos();
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
	void Gui::FocusCurrentWindow()
	{
		s_pIGui->FocusCurrentWindow();
	}
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return s_pIGui->IsWindowFocused(flags);
	}
	bool Gui::IsWindowHovered(emberCommon::GuiHoveredFlags flags)
	{
		return s_pIGui->IsWindowHovered(flags);
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
	bool Gui::IsMouseClicked(emberCommon::GuiMouseButton button)
	{
		return s_pIGui->IsMouseClicked(button);
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
	void Gui::SeparatorText(const char* label)
	{
		s_pIGui->SeparatorText(label);
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
	bool Gui::Selectable(const char* label, bool selected)
	{
		return s_pIGui->Selectable(label, selected);
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
	bool Gui::ColorEdit(const std::string& label, Float4* color)
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

			// Draw color swatch (opens picker popup on click):
			Gui::SetNextItemWidth(inputWidth);
			changed = s_pIGui->ColorEdit(("##" + label).c_str(), &color->x);
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputInt(const std::string& label, int* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
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
	bool Gui::InputInt2(const std::string& label, Int2* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		int values[] = { value->x, value->y };
		bool changed = InputIntComponents(label, values, 2, step, stepFast, flags, false);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::InputInt3(const std::string& label, Int3* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		int values[] = { value->x, value->y, value->z };
		bool changed = InputIntComponents(label, values, 3, step, stepFast, flags, false);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::DragInt(const std::string& label, int* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		static float accumulator;
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Float2 textSize = Gui::CalcTextSize(label.c_str());
			Float2 cursorPos = Gui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			static_cast<void>(Gui::InvisibleButton("##drag_text", textSize));
			bool mouseDown = Gui::IsItemActivated();
			bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);

			// Draw text above invisible button:
			Gui::SetCursorScreenPos(cursorPos);
			Gui::TextUnformatted(label.c_str());

			// On initial interaction get int value:
			if (mouseDown)
				accumulator = *value;

			// Change current value on dragging motion on label:
			if (dragging)
			{
				Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);

				float sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				accumulator += sign * sensitivity;
				*value = static_cast<int>(std::round(accumulator));

				Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
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
	bool Gui::DragInt2(const std::string& label, Int2* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		int values[] = { value->x, value->y };
		bool changed = InputIntComponents(label, values, 2, step, stepFast, flags, true);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::DragInt3(const std::string& label, Int3* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		int values[] = { value->x, value->y, value->z };
		bool changed = InputIntComponents(label, values, 3, step, stepFast, flags, true);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::InputUint(const std::string& label, uint32_t* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		Gui::TextUnformatted(label.c_str());

		Gui::SameLine();
		Float2 cursorPos = Gui::GetCursorPos();
		cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
		Gui::SetCursorPos(cursorPos);

		bool changed = false;
		Gui::PushID(label.c_str());
		{
			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);

			Gui::SetNextItemWidth(inputWidth);
			changed = s_pIGui->InputUint("##Input", value, step, stepFast, flags);
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputUint2(const std::string& label, Uint2* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		uint32_t values[] = { value->x, value->y };
		bool changed = InputUintComponents(label, values, 2, step, stepFast, flags, false);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::InputUint3(const std::string& label, Uint3* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		uint32_t values[] = { value->x, value->y, value->z };
		bool changed = InputUintComponents(label, values, 3, step, stepFast, flags, false);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::DragUint(const std::string& label, uint32_t* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		static double accumulator;
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Float2 textSize = Gui::CalcTextSize(label.c_str());
			Float2 cursorPos = Gui::GetCursorScreenPos();

			static_cast<void>(Gui::InvisibleButton("##drag_text", textSize));
			bool mouseDown = Gui::IsItemActivated();
			bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);

			Gui::SetCursorScreenPos(cursorPos);
			Gui::TextUnformatted(label.c_str());

			if (mouseDown)
				accumulator = *value;
			if (dragging)
			{
				Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);
				double sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				double signedSensitivity = (delta.x >= 0.0f) ? sensitivity : -sensitivity;
				accumulator = std::clamp(accumulator + signedSensitivity, 0.0, static_cast<double>(std::numeric_limits<uint32_t>::max()));
				*value = static_cast<uint32_t>(std::round(accumulator));
				Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
			}

			Gui::SameLine();
			cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX();
			inputWidth = math::Max(inputWidth, s_minWidgetWidth);
			Gui::SetNextItemWidth(inputWidth);
			bool interaction = s_pIGui->InputUint("##Input", value, step, stepFast, flags);

			changed = dragging || interaction;
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::DragUint2(const std::string& label, Uint2* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		uint32_t values[] = { value->x, value->y };
		bool changed = InputUintComponents(label, values, 2, step, stepFast, flags, true);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::DragUint3(const std::string& label, Uint3* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		uint32_t values[] = { value->x, value->y, value->z };
		bool changed = InputUintComponents(label, values, 3, step, stepFast, flags, true);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::InputFloat(const std::string& label, float* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
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
	bool Gui::InputFloat2(const std::string& label, Float2* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y };
		bool changed = InputFloatComponents(label, values, 2, step, stepFast, format, flags, false);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::InputFloat3(const std::string& label, Float3* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y, value->z };
		bool changed = InputFloatComponents(label, values, 3, step, stepFast, format, flags, false);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::InputFloat4(const std::string& label, Float4* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y, value->z, value->w };
		bool changed = InputFloatComponents(label, values, 4, step, stepFast, format, flags, false);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		value->w = values[3];
		return changed;
	}
	bool Gui::DragFloat(const std::string& label, float* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Float2 textSize = Gui::CalcTextSize(label.c_str());
			Float2 cursorPos = Gui::GetCursorScreenPos();

			// Invisible interactive area for click and drag detection:
			bool clicked = Gui::InvisibleButton("##drag_text", textSize);
			bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);

			// Draw text above invisible button:
			Gui::SetCursorScreenPos(cursorPos);
			Gui::TextUnformatted(label.c_str());

			// Change current value on dragging motion on label:
			if (dragging)
			{
				Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);

				float sensitivity = s_floatDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
				float sign = (delta.x >= 0.0f) ? 1.0f : -1.0f;
				*value += sign * sensitivity;

				Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
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
	bool Gui::DragFloat2(const std::string& label, Float2* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y };
		bool changed = InputFloatComponents(label, values, 2, step, stepFast, format, flags, true);
		value->x = values[0];
		value->y = values[1];
		return changed;
	}
	bool Gui::DragFloat3(const std::string& label, Float3* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y, value->z };
		bool changed = InputFloatComponents(label, values, 3, step, stepFast, format, flags, true);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		return changed;
	}
	bool Gui::DragFloat4(const std::string& label, Float4* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		float values[] = { value->x, value->y, value->z, value->w };
		bool changed = InputFloatComponents(label, values, 4, step, stepFast, format, flags, true);
		value->x = values[0];
		value->y = values[1];
		value->z = values[2];
		value->w = values[3];
		return changed;
	}



	// Private methods:
	bool Gui::InputIntComponents(const std::string& label, int* values, int componentCount, int step, int stepFast, emberCommon::GuiInputTextFlags flags, bool drag)
	{
		static double accumulators[4];
		static const char* componentIDs[] = { "##X", "##Y", "##Z", "##W" };
		static const char* componentDragIDs[] = { "##DragX", "##DragY", "##DragZ", "##DragW" };
		static const char* componentLabels[] = { "X", "Y", "Z", "W" };
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Gui::TextUnformatted(label.c_str());
			Gui::SameLine();
			Float2 cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX() * (2 * componentCount - 1);
			for (int i = 0; i < componentCount; i++)
				inputWidth -= Gui::CalcTextSize(componentLabels[i]).x;
			inputWidth = math::Max(inputWidth / componentCount, s_minWidgetWidth);
			for (int i = 0; i < componentCount; i++)
			{
				if (i > 0)
					Gui::SameLine();
				if (drag)
				{
					Float2 axisCursorPos = Gui::GetCursorScreenPos();
					static_cast<void>(Gui::InvisibleButton(componentDragIDs[i], Gui::CalcTextSize(componentLabels[i])));
					bool mouseDown = Gui::IsItemActivated();
					bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);
					Gui::SetCursorScreenPos(axisCursorPos);
					Gui::TextUnformatted(componentLabels[i]);
					if (mouseDown)
						accumulators[i] = values[i];
					if (dragging)
					{
						Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);
						double sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
						double signedSensitivity = (delta.x >= 0.0f) ? sensitivity : -sensitivity;
						accumulators[i] = std::clamp(accumulators[i] + signedSensitivity, static_cast<double>(std::numeric_limits<int>::min()), static_cast<double>(std::numeric_limits<int>::max()));
						values[i] = static_cast<int>(std::round(accumulators[i]));
						Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
						changed = true;
					}
				}
				else
					Gui::TextUnformatted(componentLabels[i]);
				Gui::SameLine();
				Gui::SetNextItemWidth(inputWidth);
				changed |= s_pIGui->InputInt(componentIDs[i], &values[i], step, stepFast, flags);
			}
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputUintComponents(const std::string& label, uint32_t* values, int componentCount, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags, bool drag)
	{
		static double accumulators[4];
		static const char* componentIDs[] = { "##X", "##Y", "##Z", "##W" };
		static const char* componentDragIDs[] = { "##DragX", "##DragY", "##DragZ", "##DragW" };
		static const char* componentLabels[] = { "X", "Y", "Z", "W" };
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Gui::TextUnformatted(label.c_str());
			Gui::SameLine();
			Float2 cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX() * (2 * componentCount - 1);
			for (int i = 0; i < componentCount; i++)
				inputWidth -= Gui::CalcTextSize(componentLabels[i]).x;
			inputWidth = math::Max(inputWidth / componentCount, s_minWidgetWidth);
			for (int i = 0; i < componentCount; i++)
			{
				if (i > 0)
					Gui::SameLine();
				if (drag)
				{
					Float2 axisCursorPos = Gui::GetCursorScreenPos();
					static_cast<void>(Gui::InvisibleButton(componentDragIDs[i], Gui::CalcTextSize(componentLabels[i])));
					bool mouseDown = Gui::IsItemActivated();
					bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);
					Gui::SetCursorScreenPos(axisCursorPos);
					Gui::TextUnformatted(componentLabels[i]);
					if (mouseDown)
						accumulators[i] = values[i];
					if (dragging)
					{
						Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);
						double sensitivity = s_intDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
						double signedSensitivity = (delta.x >= 0.0f) ? sensitivity : -sensitivity;
						accumulators[i] = std::clamp(accumulators[i] + signedSensitivity, 0.0, static_cast<double>(std::numeric_limits<uint32_t>::max()));
						values[i] = static_cast<uint32_t>(std::round(accumulators[i]));
						Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
						changed = true;
					}
				}
				else
					Gui::TextUnformatted(componentLabels[i]);
				Gui::SameLine();
				Gui::SetNextItemWidth(inputWidth);
				changed |= s_pIGui->InputUint(componentIDs[i], &values[i], step, stepFast, flags);
			}
		}
		Gui::PopID();
		return changed;
	}
	bool Gui::InputFloatComponents(const std::string& label, float* values, int componentCount, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags, bool drag)
	{
		static const char* componentIDs[] = { "##X", "##Y", "##Z", "##W" };
		static const char* componentDragIDs[] = { "##DragX", "##DragY", "##DragZ", "##DragW" };
		static const char* componentLabels[] = { "X", "Y", "Z", "W" };
		bool changed = false;
		Gui::PushID(label.c_str());
		{
			Gui::TextUnformatted(label.c_str());
			Gui::SameLine();
			Float2 cursorPos = Gui::GetCursorPos();
			cursorPos.x = math::Max(cursorPos.x, s_labelPercentile * Editor::GetWindowWidth());
			Gui::SetCursorPos(cursorPos);

			float inputWidth = Editor::GetRemainingWidth() - Editor::GetSpacingX() * (2 * componentCount - 1);
			for (int i = 0; i < componentCount; i++)
				inputWidth -= Gui::CalcTextSize(componentLabels[i]).x;
			inputWidth = math::Max(inputWidth / componentCount, s_minWidgetWidth);
			for (int i = 0; i < componentCount; i++)
			{
				if (i > 0)
					Gui::SameLine();
				if (drag)
				{
					Float2 axisCursorPos = Gui::GetCursorScreenPos();
					static_cast<void>(Gui::InvisibleButton(componentDragIDs[i], Gui::CalcTextSize(componentLabels[i])));
					bool dragging = Gui::IsItemActive() && Gui::IsMouseDragging(emberCommon::GuiMouseButton::left);
					Gui::SetCursorScreenPos(axisCursorPos);
					Gui::TextUnformatted(componentLabels[i]);
					if (dragging)
					{
						Float2 delta = Gui::GetMouseDragDelta(emberCommon::GuiMouseButton::left);
						float sensitivity = s_floatDragSensitivityBase * std::pow(std::abs(delta.x), s_dragSensitivityExponent);
						values[i] += (delta.x >= 0.0f) ? sensitivity : -sensitivity;
						Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton::left);
						changed = true;
					}
				}
				else
					Gui::TextUnformatted(componentLabels[i]);
				Gui::SameLine();
				Gui::SetNextItemWidth(inputWidth);
				changed |= s_pIGui->InputFloat(componentIDs[i], &values[i], step, stepFast, format, flags);
			}
		}
		Gui::PopID();
		return changed;
	}
}