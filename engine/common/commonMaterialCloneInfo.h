#pragma once
#include <string>



namespace emberCommon
{
	enum class MaterialBindingCloneMode
	{
		copyBindings,
		defaultBindings
	};

	struct MaterialCloneInfo
	{
		MaterialBindingCloneMode bindingCloneMode;
		std::string name;
	};
}