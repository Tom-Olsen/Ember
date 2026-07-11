#pragma once
#include "commonTextureFormat.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <cstddef>
#include <memory>
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
		friend class ShaderProperties;

    private: // Members:
		static uint32_t s_unnamedTextureCounter;

	protected: // Members:
		bool m_ownsITexture;
		emberBackendInterface::ITexture* m_pITexture; // conditional ownership, depending on usecase.
        std::string m_name;

    private: // Methods:
        emberBackendInterface::ITexture* GetInterfaceHandle();
		static std::string CreateUnnamedTextureName();

	public: // Methods:
		// Constructor/Destructor:
		Texture(emberBackendInterface::ITexture* pITexture, bool ownsTexture);
		virtual ~Texture();

		// Non-copyable:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Movable:
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetChannels() const;
		const emberCommon::TextureFormat GetFormat() const;

		// Setters:
		void SetData(std::span<const float> data);
		void SetData(std::span<const Float2> data);
		void SetData(std::span<const Float3> data);
		void SetData(std::span<const Float4> data);
		void SetRawData(std::span<const std::byte> data);

	protected: // Methods:
		Texture();
		virtual uint64_t GetExpectedTexelCount() const;
		std::vector<std::byte> ConvertToTextureFormat(std::span<const float> data, uint32_t sourceChannels) const;

    private: // Methods:
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