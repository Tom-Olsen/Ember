#include "materialProperties.h"



// Consructor:
MaterialProperties::MaterialProperties()
{
	this->context = nullptr;
}
MaterialProperties::MaterialProperties(VulkanContext* context)
{
	this->context = context;
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public initializers:
void MaterialProperties::InitUniformObjectResourceBinding(std::string name, uint32_t binding, const UniformObject& uniformObject)
{
	auto it = uniformBufferMap.find(name);
	if (it == uniformBufferMap.end())	// name does not exist yet.
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(UniformObject));
		uniformBuffer.UpdateBuffer(uniformObject);
		uniformBufferMap.emplace(name, ResourceBinding<VulkanUniformBuffer>(binding, uniformBuffer));
	}
}
void MaterialProperties::InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler)
{
	auto it = samplerMap.find(name);
	if (it == samplerMap.end())			// name does not exist yet.
		samplerMap.emplace(name, ResourceBinding<VulkanSampler*>(binding, sampler));
}
void MaterialProperties::InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d)
{
	auto it = texture2dMap.find(name);
	if (it == texture2dMap.end())		// name does not exist yet.
		texture2dMap.emplace(name, ResourceBinding<Texture2d*>(binding, texture2d));
}



// Setters:
void MaterialProperties::SetContext(VulkanContext* context)
{
	if (this->context == nullptr)
		this->context = context;
}
void MaterialProperties::SetUniformBuffer(const std::string& name, const UniformObject& data)
{
	// if key exists, replace its value
	auto it = uniformBufferMap.find(name);
	if (it != uniformBufferMap.end())
		it->second.resource.UpdateBuffer(data);
}
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMap.find(name);
	if (it != samplerMap.end())
		it->second.resource = sampler;
}
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMap.find(name);
	if (it != texture2dMap.end())
		it->second.resource = texture;
}



// Getters:
MaterialProperties MaterialProperties::GetCopy()
{
	MaterialProperties copy(context);
	for (const auto& pair : uniformBufferMap)
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(UniformObject));
		copy.uniformBufferMap.emplace(pair.first, pair.second);
	}
	for (const auto& pair : samplerMap)
		copy.samplerMap.emplace(pair.first, pair.second);
	for (const auto& pair : texture2dMap)
		copy.texture2dMap.emplace(pair.first, pair.second);
	return copy;
}



// Explicit instantiations:
//template void MaterialProperties::SetUniformBuffer<UniformObject>(const std::string& name, const UniformObject& data);
//template void MaterialProperties::SetUniformBuffer<float>(const std::string& name, const float& data);
//template void MaterialProperties::SetUniformBuffer<Float2>(const std::string& name, const Float2& data);
//template void MaterialProperties::SetUniformBuffer<Float3>(const std::string& name, const Float3& data);
//template void MaterialProperties::SetUniformBuffer<Float4>(const std::string& name, const Float4& data);
//template void MaterialProperties::SetUniformBuffer<Float2x2>(const std::string& name, const Float2x2& data);
//template void MaterialProperties::SetUniformBuffer<Float2x3>(const std::string& name, const Float2x3& data);
//template void MaterialProperties::SetUniformBuffer<Float2x4>(const std::string& name, const Float2x4& data);
//template void MaterialProperties::SetUniformBuffer<Float3x2>(const std::string& name, const Float3x2& data);
//template void MaterialProperties::SetUniformBuffer<Float3x3>(const std::string& name, const Float3x3& data);
//template void MaterialProperties::SetUniformBuffer<Float3x4>(const std::string& name, const Float3x4& data);
//template void MaterialProperties::SetUniformBuffer<Float4x2>(const std::string& name, const Float4x2& data);
//template void MaterialProperties::SetUniformBuffer<Float4x3>(const std::string& name, const Float4x3& data);
//template void MaterialProperties::SetUniformBuffer<Float4x4>(const std::string& name, const Float4x4& data);