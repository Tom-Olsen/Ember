#include "sampler.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Constructor/Destructors:
	Sampler::Sampler(const std::string& name)
	{
		m_name = name;
	}
	Sampler::~Sampler()
	{
		Cleanup();
	}



	// Move semantics:
	Sampler::Sampler(Sampler&& other) noexcept
	{
		MoveFrom(other);
	}
	Sampler& Sampler::operator=(Sampler&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	VkSampler& Sampler::GetVkSampler()
	{
		return m_sampler;
	}
	const std::string& Sampler::GetName() const
	{
		return m_name;
	}
	VkPhysicalDeviceProperties Sampler::GetVkPhysicalDeviceProperties() const
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(Context::GetVkPhysicalDevice(), &properties);
		return properties;
	}



	// Static specialised constructors:
	Sampler* Sampler::ColorSampler(const std::string& name)
	{
		Sampler* pSampler = new Sampler(name);
		VkPhysicalDeviceProperties properties = pSampler->GetVkPhysicalDeviceProperties();

		VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerInfo.magFilter = VK_FILTER_LINEAR;							// magnified texels filter
		samplerInfo.minFilter = VK_FILTER_LINEAR;							// minified texels filter
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture u coordinates outside the image
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture v coordinates outside the image
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;			// texture w coordinates outside the image
		samplerInfo.anisotropyEnable = VK_TRUE;								// anisotropic filtering
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;	// lower values = better performance but lower quality
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;			// border color for: addressModeU/V/W = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE/BORDER
		samplerInfo.unnormalizedCoordinates = VK_FALSE;						// VK_FALSE: uvw € [0,1], VK_TRUE: uvw € [0, width/height/depth]
		samplerInfo.compareEnable = VK_FALSE;								// enable comparison against a reference value
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;						// comparison function to apply
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;				// how to handle mipmapping
		samplerInfo.mipLodBias = 0.0f;										// level of detail bias for mipmap level
		samplerInfo.minLod = 0.0f;											// minimum level of detail to pick
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;								// maximum level of detail to pick. No clamping = maximum mip level supported by the image
		VKA(vkCreateSampler(Context::GetVkDevice(), &samplerInfo, nullptr, &pSampler->GetVkSampler()));

		return pSampler;
	}
	Sampler* Sampler::ShadowSampler(const std::string& name)
	{
		Sampler* pSampler = new Sampler(name);
		VkPhysicalDeviceProperties properties = pSampler->GetVkPhysicalDeviceProperties();

		VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerInfo.magFilter = VK_FILTER_LINEAR;							// magnified texels filter
		samplerInfo.minFilter = VK_FILTER_LINEAR;							// minified texels filter
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture u coordinates outside the image
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture v coordinates outside the image
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;	// texture w coordinates outside the image
		samplerInfo.anisotropyEnable = VK_FALSE;							// no anisotropic filtering for depth mapping
		samplerInfo.maxAnisotropy = 0.0f;									// lower values = better performance but lower quality
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;	// border color for: addressModeU/V/W = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE/BORDER
		samplerInfo.unnormalizedCoordinates = VK_FALSE;						// VK_FALSE: uvw € [0,1], VK_TRUE: uvw € [0, width/height/depth]
		samplerInfo.compareEnable = VK_TRUE;								// enable comparison against a reference value
		samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;				// comparison function to apply
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;				// how to handle mipmapping
		samplerInfo.mipLodBias = 0.0f;										// level of detail bias for mipmap level
		samplerInfo.minLod = 0.0f;											// minimum level of detail to pick
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;								// maximum level of detail to pick. No clamping = maximum mip level supported by the image
		VKA(vkCreateSampler(Context::GetVkDevice(), &samplerInfo, nullptr, &pSampler->GetVkSampler()));

		return pSampler;
	}



	// Private methods:
	void Sampler::Cleanup()
	{
		vkDestroySampler(Context::GetVkDevice(), m_sampler, nullptr);
	}
	void Sampler::MoveFrom(Sampler& other) noexcept
	{
		m_sampler = other.m_sampler;
		m_name = other.m_name;

		other.m_sampler = VK_NULL_HANDLE;
		other.m_name = "";
	}
}