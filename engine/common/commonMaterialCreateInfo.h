#pragma once
#include "commonForwardRenderMode.h"
#include "commonGizmoRenderMode.h"
#include "commonMaterialPass.h"
#include <string>
#include <variant>



namespace emberCommon
{
	using MaterialRenderMode = std::variant<std::monostate, GizmoRenderMode, ForwardRenderMode>;

	struct MaterialCreateInfo
	{
		MaterialPass materialPass;
		MaterialRenderMode renderMode;
		std::string name;
	};
}