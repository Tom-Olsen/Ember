#include "vulkanUniformBuffer.h"
#include "gameobject.h"
#include "macros.h"



// Constructor:
VulkanUniformBuffer::VulkanUniformBuffer(VulkanContext* context, uint64_t size)
{
	this->size = size;
	this->context = context;

	// Create buffer:
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	allocInfo.requiredFlags = 0; 
	allocInfo.preferredFlags = 0;

	this->buffer = std::make_unique<VmaBuffer>(context, bufferInfo, allocInfo);

	// get data pointer:
	VmaAllocationInfo info;
	vmaGetAllocationInfo(context->Allocator(), buffer->allocation, &info);
	data = info.pMappedData;
}



// Destructor:
VulkanUniformBuffer::~VulkanUniformBuffer()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
}



// Public:
template<typename T>
void VulkanUniformBuffer::UpdateBuffer(const T& datastruct)
{
	memcpy(data, &datastruct, sizeof(datastruct));
}



// Uniform buffer structs:
RenderMatrizes::RenderMatrizes()
{
	modelMatrix = Float4x4(1.0f);
	viewMatrix = Float4x4(1.0f);
	projMatrix = Float4x4(1.0f);
	normalMatrix = Float4x4(1.0f);
	localToClipMatrix = Float4x4(1.0f);
}
void RenderMatrizes::UpdateLocalToClipMatrix()
{
	localToClipMatrix = projMatrix * viewMatrix * modelMatrix;
}
LightData::LightData()
{
	viewMatrix = Float4x4(1.0f);
	projMatrix = Float4x4(1.0f);
	worldToClipMatrix = Float4x4(1.0f);
	directionIntensity = Float4(0.0f);
	color = Float4(0.0f);
}
LightData::LightData(DirectionalLight* directionalLights)
{
	if (directionalLights != nullptr)
	{
		viewMatrix = directionalLights->GetViewMatrix();
		projMatrix = directionalLights->GetProjectionMatrix();
		worldToClipMatrix = projMatrix * viewMatrix;
		directionIntensity = Float4(directionalLights->gameObject->transform->GetForward(), directionalLights->GetIntensity());
		color = directionalLights->GetColor();
	}
}
//LightData::LightData()
//{
//	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
//	{
//		dLights[i].viewMatrix = Float4x4(1.0f);
//		dLights[i].projMatrix = Float4x4(1.0f);
//		dLights[i].worldToClipMatrix = Float4x4(1.0f);
//		dLights[i].directionIntensity = Float4(0.0f);
//		dLights[i].color = Float4(0.0f);
//	}
//}
//LightData::LightData(std::array<DirectionalLight*, MAX_D_LIGHTS> directionalLights)
//{
//	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
//	{
//		if (directionalLights[i] != nullptr)
//		{
//			dLights[i].viewMatrix = directionalLights[i]->GetViewMatrix();
//			dLights[i].projMatrix = directionalLights[i]->GetProjectionMatrix();
//			dLights[i].worldToClipMatrix = dLights[i].projMatrix * dLights[i].viewMatrix;
//			dLights[i].directionIntensity = Float4(directionalLights[i]->gameObject->transform->GetForward(), directionalLights[i]->GetIntensity());
//			dLights[i].color = directionalLights[i]->GetColor();
//		}
//		else
//		{
//			dLights[i].viewMatrix = Float4x4(1.0f);
//			dLights[i].projMatrix = Float4x4(1.0f);
//			dLights[i].worldToClipMatrix = Float4x4(1.0f);
//			dLights[i].directionIntensity = Float4(0.0f);
//			dLights[i].color = Float4(0.0f);
//		}
//	}
//}


// Explicit instantiations:
template void VulkanUniformBuffer::UpdateBuffer<RenderMatrizes>(const RenderMatrizes& datastruct);
template void VulkanUniformBuffer::UpdateBuffer<LightData>(const LightData& datastruct);
