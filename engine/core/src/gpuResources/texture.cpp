#include "texture.h"
#include "iTexture.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>



namespace emberCore
{
	// Static members:
	uint32_t Texture::s_unnamedTextureCounter = 0;



	// Private Methods:
	emberBackendInterface::ITexture* Texture::GetInterfaceHandle()
	{
		return m_pITexture;
	}
	std::string Texture::CreateUnnamedTextureName()
	{
		return "#unnamed" + std::to_string(s_unnamedTextureCounter++);
	}



	// Public Methods:
	// Constructor/Destructor:
	Texture::Texture(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsITexture = ownsTexture;
        m_name = CreateUnnamedTextureName();
		m_pITexture = pITexture;
	}
	Texture::~Texture()
	{
		if (m_ownsITexture && m_pITexture)
			delete m_pITexture;
	}
	


	// Movable:
	Texture::Texture(Texture&& other) noexcept
	{
		m_ownsITexture = other.m_ownsITexture;
		m_name = std::move(other.m_name);
		m_pITexture = other.m_pITexture;

		other.m_ownsITexture = false;
		other.m_name = CreateUnnamedTextureName();
		other.m_pITexture = nullptr;
	}
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ownsITexture)
				delete m_pITexture;

			m_ownsITexture = other.m_ownsITexture;
			m_name = std::move(other.m_name);
			m_pITexture = other.m_pITexture;

			other.m_ownsITexture = false;
			other.m_name = CreateUnnamedTextureName();
			other.m_pITexture = nullptr;
		}
		return *this;
	}



	// Getters:
	const std::string& Texture::GetName() const
	{
		return m_name;
	}
	uint32_t Texture::GetWidth() const
	{
		return m_pITexture->GetWidth();
	}
	uint32_t Texture::GetHeight() const
	{
		return m_pITexture->GetHeight();
	}
	uint32_t Texture::GetDepth() const
	{
		return m_pITexture->GetDepth();
	}
	uint32_t Texture::GetChannels() const
	{
		return m_pITexture->GetChannels();
	}
	const emberCommon::TextureFormat Texture::GetFormat() const
	{
		return m_pITexture->GetTextureFormat();
	}



	// Setters:
	void Texture::SetData(std::span<const float> data)
	{
		std::vector<std::byte> bytes = ConvertToTextureFormat(data, 1);
		SetRawData(bytes);
	}
	void Texture::SetData(std::span<const Float2> data)
	{
		static_assert(sizeof(Float2) == 2 * sizeof(float));
		std::span<const float> floatData(reinterpret_cast<const float*>(data.data()), 2 * data.size());
		std::vector<std::byte> bytes = ConvertToTextureFormat(floatData, 2);
		SetRawData(bytes);
	}
	void Texture::SetData(std::span<const Float3> data)
	{
		static_assert(sizeof(Float3) == 3 * sizeof(float));
		std::span<const float> floatData(reinterpret_cast<const float*>(data.data()), 3 * data.size());
		std::vector<std::byte> bytes = ConvertToTextureFormat(floatData, 3);
		SetRawData(bytes);
	}
	void Texture::SetData(std::span<const Float4> data)
	{
		static_assert(sizeof(Float4) == 4 * sizeof(float));
		std::span<const float> floatData(reinterpret_cast<const float*>(data.data()), 4 * data.size());
		std::vector<std::byte> bytes = ConvertToTextureFormat(floatData, 4);
		SetRawData(bytes);
	}
	void Texture::SetRawData(std::span<const std::byte> data)
	{
		uint64_t expectedByteCount = GetExpectedTexelCount() * GetChannels() * GetFormat().bytesPerChannel;
		std::vector<std::byte> bytes(expectedByteCount);
		uint64_t copyByteCount = std::min<uint64_t>(expectedByteCount, data.size());
		if (copyByteCount > 0)
			std::memcpy(bytes.data(), data.data(), copyByteCount);
		m_pITexture->SetData(bytes.data());
	}



	// Protected methods:
	Texture::Texture()
	{
		m_ownsITexture = false;
        m_name = CreateUnnamedTextureName();
		m_pITexture = nullptr;
	}
	uint64_t Texture::GetExpectedTexelCount() const
	{
		return static_cast<uint64_t>(GetWidth()) * GetHeight() * GetDepth();
	}
	std::vector<std::byte> Texture::ConvertToTextureFormat(std::span<const float> data, uint32_t sourceChannels) const
	{
		emberCommon::TextureFormat format = GetFormat();
		uint64_t texelCount = GetExpectedTexelCount();
		std::vector<std::byte> bytes(texelCount * format.channels * format.bytesPerChannel);
		uint64_t offset = 0;

        // Unsigned integer formats:
		if (emberCommon::IsUnsignedIntegerFormat(format.flag))
		{
			double maxValue = MaxUnsignedValue(format.bytesPerChannel);
			for (uint64_t texel = 0; texel < texelCount; texel++)
			{
				for (uint32_t channel = 0; channel < format.channels; channel++)
				{
					float sourceValue = GetSourceValue(data, texel, channel, sourceChannels);
					if (emberCommon::IsSrgbFormat(format.flag))
					    sourceValue = LinearToSrgb(sourceValue);
					double normalized = std::clamp(static_cast<double>(sourceValue), 0.0, 1.0);
					uint64_t value = static_cast<uint64_t>(normalized * maxValue + 0.5); // +0.5 for rounding up.

					if (format.bytesPerChannel == 1)
						WriteValue(bytes, offset, static_cast<uint8_t>(value));
					else if (format.bytesPerChannel == 2)
						WriteValue(bytes, offset, static_cast<uint16_t>(value));
					else if (format.bytesPerChannel == 4)
						WriteValue(bytes, offset, static_cast<uint32_t>(value));
					else if (format.bytesPerChannel == 8)
						WriteValue(bytes, offset, static_cast<uint64_t>(value));
				}
			}
			return bytes;
        }

	    // Signed integer formats:
		if (emberCommon::IsSignedIntegerFormat(format.flag))
		{
			double absMinValue = std::abs(static_cast<double>(MinSignedValue(format.bytesPerChannel)));
			double maxValue = static_cast<double>(MaxSignedValue(format.bytesPerChannel));
			for (uint64_t texel = 0; texel < texelCount; texel++)
			{
				for (uint32_t channel = 0; channel < format.channels; channel++)
				{
					float sourceValue = GetSourceValue(data, texel, channel, sourceChannels);
					double normalized = std::clamp(static_cast<double>(sourceValue), -1.0, 1.0);
					int64_t value = normalized < 0.0 ? static_cast<int64_t>(normalized * absMinValue - 0.5) : static_cast<int64_t>(normalized * maxValue + 0.5);

					if (format.bytesPerChannel == 1)
						WriteValue(bytes, offset, static_cast<int8_t>(value));
					else if (format.bytesPerChannel == 2)
						WriteValue(bytes, offset, static_cast<int16_t>(value));
					else if (format.bytesPerChannel == 4)
						WriteValue(bytes, offset, static_cast<int32_t>(value));
					else if (format.bytesPerChannel == 8)
						WriteValue(bytes, offset, static_cast<int64_t>(value));
				}
			}
			return bytes;
		}

        // Float formats:
		if (emberCommon::IsFloatFormat(format.flag))
		{
			for (uint64_t texel = 0; texel < texelCount; texel++)
			{
				for (uint32_t channel = 0; channel < format.channels; channel++)
				{
					float sourceValue = GetSourceValue(data, texel, channel, sourceChannels);

					if (format.bytesPerChannel == 2)
						WriteValue(bytes, offset, FloatToHalf(sourceValue));
					else if (format.bytesPerChannel == 4)
						WriteValue(bytes, offset, sourceValue);
					else if (format.bytesPerChannel == 8)
						WriteValue(bytes, offset, static_cast<double>(sourceValue));
				}
			}
			return bytes;
		}
		throw std::runtime_error("Texture::ConvertToTextureFormat(...) failed. Unsupported texture format: " + std::string(emberCommon::FormatFlagToString(format.flag)));
	}



    // Private methods:
	double Texture::MaxUnsignedValue(uint32_t bytesPerChannel)
	{
		switch (bytesPerChannel)
		{
			case 1: return static_cast<double>(std::numeric_limits<uint8_t>::max());
			case 2: return static_cast<double>(std::numeric_limits<uint16_t>::max());
			case 4: return static_cast<double>(std::numeric_limits<uint32_t>::max());
			case 8: return static_cast<double>(std::numeric_limits<uint64_t>::max());
			default: throw std::runtime_error("Texture::MaxUnsignedValue(...) failed. Unsupported unsigned texture format channel size.");
		}
	}
	int64_t Texture::MinSignedValue(uint32_t bytesPerChannel)
	{
		switch (bytesPerChannel)
		{
			case 1: return static_cast<int64_t>(std::numeric_limits<int8_t>::min());
			case 2: return static_cast<int64_t>(std::numeric_limits<int16_t>::min());
			case 4: return static_cast<int64_t>(std::numeric_limits<int32_t>::min());
			case 8: return static_cast<int64_t>(std::numeric_limits<int64_t>::min());
			default: throw std::runtime_error("Texture::MinSignedValue(...) failed. Unsupported signed texture format channel size.");
		}
	}
	int64_t Texture::MaxSignedValue(uint32_t bytesPerChannel)
	{
		switch (bytesPerChannel)
		{
			case 1: return static_cast<int64_t>(std::numeric_limits<int8_t>::max());
			case 2: return static_cast<int64_t>(std::numeric_limits<int16_t>::max());
			case 4: return static_cast<int64_t>(std::numeric_limits<int32_t>::max());
			case 8: return static_cast<int64_t>(std::numeric_limits<int64_t>::max());
			default: throw std::runtime_error("Texture::MaxSignedValue(...) failed. Unsupported signed texture format channel size.");
		}
	}
	float Texture::LinearToSrgb(float value)
	{
		value = std::clamp(value, 0.0f, 1.0f);
        float sRGBLo = 12.92f * value;
        float sRGBHi = 1.055f * math::Pow(value, 1.0f / 2.4f) - 0.055f;
        return (value <= 0.0031308f) ? sRGBLo : sRGBHi;
	}
	float Texture::GetSourceValue(std::span<const float> data, uint64_t texel, uint32_t channel, uint32_t sourceChannels)
	{
		uint64_t sourceIndex = texel * sourceChannels + channel;
		if (channel < sourceChannels && sourceIndex < data.size())
			return data[sourceIndex];
		return channel == 3 ? 1.0f : 0.0f;
	}
	template<typename T>
	void Texture::WriteValue(std::vector<std::byte>& bytes, uint64_t& offset, T value)
	{
		std::memcpy(bytes.data() + offset, &value, sizeof(T));
		offset += sizeof(T);
	}
    uint16_t Texture::FloatToHalf(float value)
    {
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        uint32_t sign = (bits >> 16) & 0x8000;
        uint32_t exponent = (bits >> 23) & 0xff;
        uint32_t mantissa = bits & 0x7fffff;

        // NaN or Inf:
        if (exponent == 0xff)
        {
            if (mantissa == 0)
                return static_cast<uint16_t>(sign | 0x7c00); // inf.
            // Preserve some NaN payload, ensure mantissa is non-zero:
            uint16_t nanMantissa = static_cast<uint16_t>(mantissa >> 13);
            if (nanMantissa == 0)
                nanMantissa = 1;
            return static_cast<uint16_t>(sign | 0x7c00 | nanMantissa);
        }

        int32_t halfExponent = static_cast<int32_t>(exponent) - 127 + 15;

        // Overflow -> Inf:
        if (halfExponent >= 31)
            return static_cast<uint16_t>(sign | 0x7c00);

        // Underflow / subnormal:
        if (halfExponent <= 0)
        {
            if (halfExponent < -10)
                return static_cast<uint16_t>(sign); // too small -> signed zero.

            mantissa |= 0x800000; // restore implicit leading 1.
            uint32_t shift = static_cast<uint32_t>(14 - halfExponent);
            uint32_t halfMantissa = mantissa >> shift;

            // Round to nearest:
            uint32_t roundBit = (mantissa >> (shift - 1)) & 1;
            halfMantissa += roundBit;
            return static_cast<uint16_t>(sign | halfMantissa);
        }

        // Normal half:
        uint32_t halfMantissa = mantissa >> 13;

        // Round to nearest, ties not perfectly even but good enough for texture use:
        if (mantissa & 0x1000)
        {
            halfMantissa++;
            if (halfMantissa == 0x400)
            {
                halfMantissa = 0;
                halfExponent++;
                if (halfExponent >= 31)
                    return static_cast<uint16_t>(sign | 0x7c00);
            }
        }

        return static_cast<uint16_t>(sign | (static_cast<uint32_t>(halfExponent) << 10) | halfMantissa);
    }
}