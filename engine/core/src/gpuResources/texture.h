#pragma once
#include "commonTextureFormat.h"
#include "commonTextureId.h"
#include "commonTextureImageCountMode.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstddef>
#include <span>
#include <string>
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class ITexture;
}



namespace emberCore
{
	class EMBER_CORE_API Texture
	{
		// Friends:
		friend class Gui;
		friend class Shader;
		friend class CallProperties;
		friend class TextureManager;

    protected: // Members:
		emberCommon::TextureId m_textureId;

	public: // Methods:
		// Constructor/Destructor:
		Texture();
		virtual ~Texture();

		// Copyable:
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;

		// Movable:
		Texture(Texture&& other) noexcept = default;
		Texture& operator=(Texture&& other) noexcept = default;

		// Deleter:
		void Destroy();

		// Getters:
		std::string GetName() const;
		bool IsValid() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetChannels() const;
		const emberCommon::TextureFormat GetFormat() const;
		emberCommon::TextureImageCountMode GetImageCountMode() const;

		// Setters:
		void SetData(std::span<const float> data);
		void SetData(std::span<const Float2> data);
		void SetData(std::span<const Float3> data);
		void SetData(std::span<const Float4> data);
		void SetRawData(std::span<const std::byte> data);

	protected: // Methods:
		explicit Texture(emberCommon::TextureId textureId);
		virtual uint64_t GetExpectedTexelCount() const;
		std::vector<std::byte> ConvertToTextureFormat(std::span<const float> data, uint32_t sourceChannels) const;

    private: // Methods:
        emberBackendInterface::ITexture* GetInterfaceHandle() const;
        static double MaxUnsignedValue(uint32_t bytesPerChannel);
        static int64_t MinSignedValue(uint32_t bytesPerChannel);
        static int64_t MaxSignedValue(uint32_t bytesPerChannel);
        static float LinearToSrgb(float value);
	    static float GetSourceValue(std::span<const float> data, uint64_t texel, uint32_t channel, uint32_t sourceChannels);
	    template<typename T>
        static void WriteValue(std::vector<std::byte>& bytes, uint64_t& offset, T value);
        static uint16_t FloatToHalf(float value);
	};
}