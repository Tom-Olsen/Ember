#include "vulkanConvertTextureFormat.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // Format -> emberCommon::DepthStencilFormat:
    const emberCommon::DepthStencilFormat& VulkanFormatToDepthStencilFormat(vulkanRendererBackend::Format format)
    {
        const static std::unordered_map<vulkanRendererBackend::Format, const emberCommon::DepthStencilFormat*> map =
        {
            { vulkanRendererBackend::Formats::d16_unorm,          &emberCommon::DepthStencilFormats::d00_s8 },
            { vulkanRendererBackend::Formats::d32_sfloat,         &emberCommon::DepthStencilFormats::d16_s0 },
            { vulkanRendererBackend::Formats::s8_uint,            &emberCommon::DepthStencilFormats::d16_s8 },
            { vulkanRendererBackend::Formats::d16_unorm_s8_uint,  &emberCommon::DepthStencilFormats::d24_s8 },
            { vulkanRendererBackend::Formats::d24_unorm_s8_uint,  &emberCommon::DepthStencilFormats::d32_s0 },
            { vulkanRendererBackend::Formats::d32_sfloat_s8_uint, &emberCommon::DepthStencilFormats::d32_s8 }
        };
        auto it = map.find(format);
        return it != map.end() ? *it->second : emberCommon::DepthStencilFormats::undefined;
    }
    // emberCommon::DepthStencilFormat -> Format:
    const vulkanRendererBackend::Format DepthStencilFormatToVulkanFormat(const emberCommon::DepthStencilFormat& format)
    {
        const static std::unordered_map<uint32_t, vulkanRendererBackend::Format> map =
        {
            { emberCommon::DepthStencilFormats::d00_s8.GetId(), vulkanRendererBackend::Formats::d16_unorm          },
            { emberCommon::DepthStencilFormats::d16_s0.GetId(), vulkanRendererBackend::Formats::d32_sfloat         },
            { emberCommon::DepthStencilFormats::d16_s8.GetId(), vulkanRendererBackend::Formats::s8_uint            },
            { emberCommon::DepthStencilFormats::d24_s8.GetId(), vulkanRendererBackend::Formats::d16_unorm_s8_uint  },
            { emberCommon::DepthStencilFormats::d32_s0.GetId(), vulkanRendererBackend::Formats::d24_unorm_s8_uint  },
            { emberCommon::DepthStencilFormats::d32_s8.GetId(), vulkanRendererBackend::Formats::d32_sfloat_s8_uint }
        };
        auto it = map.find(format.GetId());
        return it != map.end() ? it->second : vulkanRendererBackend::Formats::undefined;
    }



    // Format -> emberCommon::TextureFormat:
    const emberCommon::TextureFormat& VulkanFormatToTextureFormat(vulkanRendererBackend::Format format)
    {
        const static std::unordered_map<vulkanRendererBackend::Format, const emberCommon::TextureFormat*> map =
        {
            // Single Channel:
            { vulkanRendererBackend::Formats::r8_srgb,              &emberCommon::TextureFormats::r08_srgb       },
            { vulkanRendererBackend::Formats::r8_uint,              &emberCommon::TextureFormats::r08_uint       },
            { vulkanRendererBackend::Formats::r8_sint,              &emberCommon::TextureFormats::r08_sint       },
            { vulkanRendererBackend::Formats::r8_uscaled,           &emberCommon::TextureFormats::r08_uscaled    },
            { vulkanRendererBackend::Formats::r8_sscaled,           &emberCommon::TextureFormats::r08_sscaled    },
            { vulkanRendererBackend::Formats::r8_unorm,             &emberCommon::TextureFormats::r08_unorm      },
            { vulkanRendererBackend::Formats::r8_snorm,             &emberCommon::TextureFormats::r08_snorm      },
            { vulkanRendererBackend::Formats::r16_uint,             &emberCommon::TextureFormats::r16_uint       },
            { vulkanRendererBackend::Formats::r16_sint,             &emberCommon::TextureFormats::r16_sint       },
            { vulkanRendererBackend::Formats::r16_uscaled,          &emberCommon::TextureFormats::r16_uscaled    },
            { vulkanRendererBackend::Formats::r16_sscaled,          &emberCommon::TextureFormats::r16_sscaled    },
            { vulkanRendererBackend::Formats::r16_unorm,            &emberCommon::TextureFormats::r16_unorm      },
            { vulkanRendererBackend::Formats::r16_snorm,            &emberCommon::TextureFormats::r16_snorm      },
            { vulkanRendererBackend::Formats::r16_sfloat,           &emberCommon::TextureFormats::r16_sfloat     },
            { vulkanRendererBackend::Formats::r32_uint,             &emberCommon::TextureFormats::r32_uint       },
            { vulkanRendererBackend::Formats::r32_sint,             &emberCommon::TextureFormats::r32_sint       },
            { vulkanRendererBackend::Formats::r32_sfloat,           &emberCommon::TextureFormats::r32_sfloat     },
            { vulkanRendererBackend::Formats::r64_uint,             &emberCommon::TextureFormats::r64_uint       },
            { vulkanRendererBackend::Formats::r64_sint,             &emberCommon::TextureFormats::r64_sint       },
            { vulkanRendererBackend::Formats::r64_sfloat,           &emberCommon::TextureFormats::r64_sfloat     },
                                                                                                                
            // Dual Channel:                                                                                    
            { vulkanRendererBackend::Formats::r8g8_srgb,            &emberCommon::TextureFormats::rg08_srgb      },
            { vulkanRendererBackend::Formats::r8g8_uint,            &emberCommon::TextureFormats::rg08_uint      },
            { vulkanRendererBackend::Formats::r8g8_sint,            &emberCommon::TextureFormats::rg08_sint      },
            { vulkanRendererBackend::Formats::r8g8_uscaled,         &emberCommon::TextureFormats::rg08_uscaled   },
            { vulkanRendererBackend::Formats::r8g8_sscaled,         &emberCommon::TextureFormats::rg08_sscaled   },
            { vulkanRendererBackend::Formats::r8g8_unorm,           &emberCommon::TextureFormats::rg08_unorm     },
            { vulkanRendererBackend::Formats::r8g8_snorm,           &emberCommon::TextureFormats::rg08_snorm     },
            { vulkanRendererBackend::Formats::r16g16_uint,          &emberCommon::TextureFormats::rg16_uint      },
            { vulkanRendererBackend::Formats::r16g16_sint,          &emberCommon::TextureFormats::rg16_sint      },
            { vulkanRendererBackend::Formats::r16g16_uscaled,       &emberCommon::TextureFormats::rg16_uscaled   },
            { vulkanRendererBackend::Formats::r16g16_sscaled,       &emberCommon::TextureFormats::rg16_sscaled   },
            { vulkanRendererBackend::Formats::r16g16_unorm,         &emberCommon::TextureFormats::rg16_unorm     },
            { vulkanRendererBackend::Formats::r16g16_snorm,         &emberCommon::TextureFormats::rg16_snorm     },
            { vulkanRendererBackend::Formats::r16g16_sfloat,        &emberCommon::TextureFormats::rg16_sfloat    },
            { vulkanRendererBackend::Formats::r32g32_uint,          &emberCommon::TextureFormats::rg32_uint      },
            { vulkanRendererBackend::Formats::r32g32_sint,          &emberCommon::TextureFormats::rg32_sint      },
            { vulkanRendererBackend::Formats::r32g32_sfloat,        &emberCommon::TextureFormats::rg32_sfloat    },
            { vulkanRendererBackend::Formats::r64g64_uint,          &emberCommon::TextureFormats::rg64_uint      },
            { vulkanRendererBackend::Formats::r64g64_sint,          &emberCommon::TextureFormats::rg64_sint      },
            { vulkanRendererBackend::Formats::r64g64_sfloat,        &emberCommon::TextureFormats::rg64_sfloat    },
                                                                                                                
            // Triple Channel:                                                                                  
            { vulkanRendererBackend::Formats::r8g8b8_srgb,          &emberCommon::TextureFormats::rgb08_srgb     },
            { vulkanRendererBackend::Formats::r8g8b8_uint,          &emberCommon::TextureFormats::rgb08_uint     },
            { vulkanRendererBackend::Formats::r8g8b8_sint,          &emberCommon::TextureFormats::rgb08_sint     },
            { vulkanRendererBackend::Formats::r8g8b8_uscaled,       &emberCommon::TextureFormats::rgb08_uscaled  },
            { vulkanRendererBackend::Formats::r8g8b8_sscaled,       &emberCommon::TextureFormats::rgb08_sscaled  },
            { vulkanRendererBackend::Formats::r8g8b8_unorm,         &emberCommon::TextureFormats::rgb08_unorm    },
            { vulkanRendererBackend::Formats::r8g8b8_snorm,         &emberCommon::TextureFormats::rgb08_snorm    },
            { vulkanRendererBackend::Formats::r16g16b16_uint,       &emberCommon::TextureFormats::rgb16_uint     },
            { vulkanRendererBackend::Formats::r16g16b16_sint,       &emberCommon::TextureFormats::rgb16_sint     },
            { vulkanRendererBackend::Formats::r16g16b16_uscaled,    &emberCommon::TextureFormats::rgb16_uscaled  },
            { vulkanRendererBackend::Formats::r16g16b16_sscaled,    &emberCommon::TextureFormats::rgb16_sscaled  },
            { vulkanRendererBackend::Formats::r16g16b16_unorm,      &emberCommon::TextureFormats::rgb16_unorm    },
            { vulkanRendererBackend::Formats::r16g16b16_snorm,      &emberCommon::TextureFormats::rgb16_snorm    },
            { vulkanRendererBackend::Formats::r16g16b16_sfloat,     &emberCommon::TextureFormats::rgb16_sfloat   },
            { vulkanRendererBackend::Formats::r32g32b32_uint,       &emberCommon::TextureFormats::rgb32_uint     },
            { vulkanRendererBackend::Formats::r32g32b32_sint,       &emberCommon::TextureFormats::rgb32_sint     },
            { vulkanRendererBackend::Formats::r32g32b32_sfloat,     &emberCommon::TextureFormats::rgb32_sfloat   },
            { vulkanRendererBackend::Formats::r64g64b64_uint,       &emberCommon::TextureFormats::rgb64_uint     },
            { vulkanRendererBackend::Formats::r64g64b64_sint,       &emberCommon::TextureFormats::rgb64_sint     },
            { vulkanRendererBackend::Formats::r64g64b64_sfloat,     &emberCommon::TextureFormats::rgb64_sfloat   },

            // Quad Channel:
            { vulkanRendererBackend::Formats::r8g8b8a8_srgb,        &emberCommon::TextureFormats::rgba08_srgb    },
            { vulkanRendererBackend::Formats::r8g8b8a8_uint,        &emberCommon::TextureFormats::rgba08_uint    },
            { vulkanRendererBackend::Formats::r8g8b8a8_sint,        &emberCommon::TextureFormats::rgba08_sint    },
            { vulkanRendererBackend::Formats::r8g8b8a8_uscaled,     &emberCommon::TextureFormats::rgba08_uscaled },
            { vulkanRendererBackend::Formats::r8g8b8a8_sscaled,     &emberCommon::TextureFormats::rgba08_sscaled },
            { vulkanRendererBackend::Formats::r8g8b8a8_unorm,       &emberCommon::TextureFormats::rgba08_unorm   },
            { vulkanRendererBackend::Formats::r8g8b8a8_snorm,       &emberCommon::TextureFormats::rgba08_snorm   },
            { vulkanRendererBackend::Formats::r16g16b16a16_uint,    &emberCommon::TextureFormats::rgba16_uint    },
            { vulkanRendererBackend::Formats::r16g16b16a16_sint,    &emberCommon::TextureFormats::rgba16_sint    },
            { vulkanRendererBackend::Formats::r16g16b16a16_uscaled, &emberCommon::TextureFormats::rgba16_uscaled },
            { vulkanRendererBackend::Formats::r16g16b16a16_sscaled, &emberCommon::TextureFormats::rgba16_sscaled },
            { vulkanRendererBackend::Formats::r16g16b16a16_unorm,   &emberCommon::TextureFormats::rgba16_unorm   },
            { vulkanRendererBackend::Formats::r16g16b16a16_snorm,   &emberCommon::TextureFormats::rgba16_snorm   },
            { vulkanRendererBackend::Formats::r16g16b16a16_sfloat,  &emberCommon::TextureFormats::rgba16_sfloat  },
            { vulkanRendererBackend::Formats::r32g32b32a32_uint,    &emberCommon::TextureFormats::rgba32_uint    },
            { vulkanRendererBackend::Formats::r32g32b32a32_sint,    &emberCommon::TextureFormats::rgba32_sint    },
            { vulkanRendererBackend::Formats::r32g32b32a32_sfloat,  &emberCommon::TextureFormats::rgba32_sfloat  },
            { vulkanRendererBackend::Formats::r64g64b64a64_uint,    &emberCommon::TextureFormats::rgba64_uint    },
            { vulkanRendererBackend::Formats::r64g64b64a64_sint,    &emberCommon::TextureFormats::rgba64_sint    },
            { vulkanRendererBackend::Formats::r64g64b64a64_sfloat,  &emberCommon::TextureFormats::rgba64_sfloat  }
        };
        auto it = map.find(format);
        return it != map.end() ? *it->second : emberCommon::TextureFormats::undefined;
    }



    // emberCommon::TextureFormat -> Format:
    const vulkanRendererBackend::Format TextureFormatToVulkanFormat(const emberCommon::TextureFormat& format)
    {
        // Continue here: cant use emberCommon::TextureFormat as the key as it does not reduce to a int value like vulkanRendererBackend::Format does.
        // Need to add some hashing function to emberCommon::TextureFormat and use the hash isntead.
        const static std::unordered_map<uint32_t, vulkanRendererBackend::Format> map =
        {
            // Single Channel:
            { emberCommon::TextureFormats::r08_srgb.GetId(),    vulkanRendererBackend::Formats::r8_srgb     },
            { emberCommon::TextureFormats::r08_uint.GetId(),    vulkanRendererBackend::Formats::r8_uint     },
            { emberCommon::TextureFormats::r08_sint.GetId(),    vulkanRendererBackend::Formats::r8_sint     },
            { emberCommon::TextureFormats::r08_uscaled.GetId(), vulkanRendererBackend::Formats::r8_uscaled  },
            { emberCommon::TextureFormats::r08_sscaled.GetId(), vulkanRendererBackend::Formats::r8_sscaled  },
            { emberCommon::TextureFormats::r08_unorm.GetId(),   vulkanRendererBackend::Formats::r8_unorm    },
            { emberCommon::TextureFormats::r08_snorm.GetId(),   vulkanRendererBackend::Formats::r8_snorm    },
            { emberCommon::TextureFormats::r16_uint.GetId(),    vulkanRendererBackend::Formats::r16_uint    },
            { emberCommon::TextureFormats::r16_sint.GetId(),    vulkanRendererBackend::Formats::r16_sint    },
            { emberCommon::TextureFormats::r16_uscaled.GetId(), vulkanRendererBackend::Formats::r16_uscaled },
            { emberCommon::TextureFormats::r16_sscaled.GetId(), vulkanRendererBackend::Formats::r16_sscaled },
            { emberCommon::TextureFormats::r16_unorm.GetId(),   vulkanRendererBackend::Formats::r16_unorm   },
            { emberCommon::TextureFormats::r16_snorm.GetId(),   vulkanRendererBackend::Formats::r16_snorm   },
            { emberCommon::TextureFormats::r16_sfloat.GetId(),  vulkanRendererBackend::Formats::r16_sfloat  },
            { emberCommon::TextureFormats::r32_uint.GetId(),    vulkanRendererBackend::Formats::r32_uint    },
            { emberCommon::TextureFormats::r32_sint.GetId(),    vulkanRendererBackend::Formats::r32_sint    },
            { emberCommon::TextureFormats::r32_sfloat.GetId(),  vulkanRendererBackend::Formats::r32_sfloat  },
            { emberCommon::TextureFormats::r64_uint.GetId(),    vulkanRendererBackend::Formats::r64_uint    },
            { emberCommon::TextureFormats::r64_sint.GetId(),    vulkanRendererBackend::Formats::r64_sint    },
            { emberCommon::TextureFormats::r64_sfloat.GetId(),  vulkanRendererBackend::Formats::r64_sfloat  },

            // Dual Channel:                                                                                    
            { emberCommon::TextureFormats::rg08_srgb.GetId(),    vulkanRendererBackend::Formats::r8g8_srgb      },
            { emberCommon::TextureFormats::rg08_uint.GetId(),    vulkanRendererBackend::Formats::r8g8_uint      },
            { emberCommon::TextureFormats::rg08_sint.GetId(),    vulkanRendererBackend::Formats::r8g8_sint      },
            { emberCommon::TextureFormats::rg08_uscaled.GetId(), vulkanRendererBackend::Formats::r8g8_uscaled   },
            { emberCommon::TextureFormats::rg08_sscaled.GetId(), vulkanRendererBackend::Formats::r8g8_sscaled   },
            { emberCommon::TextureFormats::rg08_unorm.GetId(),   vulkanRendererBackend::Formats::r8g8_unorm     },
            { emberCommon::TextureFormats::rg08_snorm.GetId(),   vulkanRendererBackend::Formats::r8g8_snorm     },
            { emberCommon::TextureFormats::rg16_uint.GetId(),    vulkanRendererBackend::Formats::r16g16_uint    },
            { emberCommon::TextureFormats::rg16_sint.GetId(),    vulkanRendererBackend::Formats::r16g16_sint    },
            { emberCommon::TextureFormats::rg16_uscaled.GetId(), vulkanRendererBackend::Formats::r16g16_uscaled },
            { emberCommon::TextureFormats::rg16_sscaled.GetId(), vulkanRendererBackend::Formats::r16g16_sscaled },
            { emberCommon::TextureFormats::rg16_unorm.GetId(),   vulkanRendererBackend::Formats::r16g16_unorm   },
            { emberCommon::TextureFormats::rg16_snorm.GetId(),   vulkanRendererBackend::Formats::r16g16_snorm   },
            { emberCommon::TextureFormats::rg16_sfloat.GetId(),  vulkanRendererBackend::Formats::r16g16_sfloat  },
            { emberCommon::TextureFormats::rg32_uint.GetId(),    vulkanRendererBackend::Formats::r32g32_uint    },
            { emberCommon::TextureFormats::rg32_sint.GetId(),    vulkanRendererBackend::Formats::r32g32_sint    },
            { emberCommon::TextureFormats::rg32_sfloat.GetId(),  vulkanRendererBackend::Formats::r32g32_sfloat  },
            { emberCommon::TextureFormats::rg64_uint.GetId(),    vulkanRendererBackend::Formats::r64g64_uint    },
            { emberCommon::TextureFormats::rg64_sint.GetId(),    vulkanRendererBackend::Formats::r64g64_sint    },
            { emberCommon::TextureFormats::rg64_sfloat.GetId(),  vulkanRendererBackend::Formats::r64g64_sfloat  },

            // Triple Channel:                                                                                  
            { emberCommon::TextureFormats::rgb08_srgb.GetId(),    vulkanRendererBackend::Formats::r8g8b8_srgb       },
            { emberCommon::TextureFormats::rgb08_uint.GetId(),    vulkanRendererBackend::Formats::r8g8b8_uint       },
            { emberCommon::TextureFormats::rgb08_sint.GetId(),    vulkanRendererBackend::Formats::r8g8b8_sint       },
            { emberCommon::TextureFormats::rgb08_uscaled.GetId(), vulkanRendererBackend::Formats::r8g8b8_uscaled    },
            { emberCommon::TextureFormats::rgb08_sscaled.GetId(), vulkanRendererBackend::Formats::r8g8b8_sscaled    },
            { emberCommon::TextureFormats::rgb08_unorm.GetId(),   vulkanRendererBackend::Formats::r8g8b8_unorm      },
            { emberCommon::TextureFormats::rgb08_snorm.GetId(),   vulkanRendererBackend::Formats::r8g8b8_snorm      },
            { emberCommon::TextureFormats::rgb16_uint.GetId(),    vulkanRendererBackend::Formats::r16g16b16_uint    },
            { emberCommon::TextureFormats::rgb16_sint.GetId(),    vulkanRendererBackend::Formats::r16g16b16_sint    },
            { emberCommon::TextureFormats::rgb16_uscaled.GetId(), vulkanRendererBackend::Formats::r16g16b16_uscaled },
            { emberCommon::TextureFormats::rgb16_sscaled.GetId(), vulkanRendererBackend::Formats::r16g16b16_sscaled },
            { emberCommon::TextureFormats::rgb16_unorm.GetId(),   vulkanRendererBackend::Formats::r16g16b16_unorm   },
            { emberCommon::TextureFormats::rgb16_snorm.GetId(),   vulkanRendererBackend::Formats::r16g16b16_snorm   },
            { emberCommon::TextureFormats::rgb16_sfloat.GetId(),  vulkanRendererBackend::Formats::r16g16b16_sfloat  },
            { emberCommon::TextureFormats::rgb32_uint.GetId(),    vulkanRendererBackend::Formats::r32g32b32_uint    },
            { emberCommon::TextureFormats::rgb32_sint.GetId(),    vulkanRendererBackend::Formats::r32g32b32_sint    },
            { emberCommon::TextureFormats::rgb32_sfloat.GetId(),  vulkanRendererBackend::Formats::r32g32b32_sfloat  },
            { emberCommon::TextureFormats::rgb64_uint.GetId(),    vulkanRendererBackend::Formats::r64g64b64_uint    },
            { emberCommon::TextureFormats::rgb64_sint.GetId(),    vulkanRendererBackend::Formats::r64g64b64_sint    },
            { emberCommon::TextureFormats::rgb64_sfloat.GetId(),  vulkanRendererBackend::Formats::r64g64b64_sfloat  },

            // Quad Channel:
            { emberCommon::TextureFormats::rgba08_srgb.GetId(),    vulkanRendererBackend::Formats::r8g8b8a8_srgb        },
            { emberCommon::TextureFormats::rgba08_uint.GetId(),    vulkanRendererBackend::Formats::r8g8b8a8_uint        },
            { emberCommon::TextureFormats::rgba08_sint.GetId(),    vulkanRendererBackend::Formats::r8g8b8a8_sint        },
            { emberCommon::TextureFormats::rgba08_uscaled.GetId(), vulkanRendererBackend::Formats::r8g8b8a8_uscaled     },
            { emberCommon::TextureFormats::rgba08_sscaled.GetId(), vulkanRendererBackend::Formats::r8g8b8a8_sscaled     },
            { emberCommon::TextureFormats::rgba08_unorm.GetId(),   vulkanRendererBackend::Formats::r8g8b8a8_unorm       },
            { emberCommon::TextureFormats::rgba08_snorm.GetId(),   vulkanRendererBackend::Formats::r8g8b8a8_snorm       },
            { emberCommon::TextureFormats::rgba16_uint.GetId(),    vulkanRendererBackend::Formats::r16g16b16a16_uint    },
            { emberCommon::TextureFormats::rgba16_sint.GetId(),    vulkanRendererBackend::Formats::r16g16b16a16_sint    },
            { emberCommon::TextureFormats::rgba16_uscaled.GetId(), vulkanRendererBackend::Formats::r16g16b16a16_uscaled },
            { emberCommon::TextureFormats::rgba16_sscaled.GetId(), vulkanRendererBackend::Formats::r16g16b16a16_sscaled },
            { emberCommon::TextureFormats::rgba16_unorm.GetId(),   vulkanRendererBackend::Formats::r16g16b16a16_unorm   },
            { emberCommon::TextureFormats::rgba16_snorm.GetId(),   vulkanRendererBackend::Formats::r16g16b16a16_snorm   },
            { emberCommon::TextureFormats::rgba16_sfloat.GetId(),  vulkanRendererBackend::Formats::r16g16b16a16_sfloat  },
            { emberCommon::TextureFormats::rgba32_uint.GetId(),    vulkanRendererBackend::Formats::r32g32b32a32_uint    },
            { emberCommon::TextureFormats::rgba32_sint.GetId(),    vulkanRendererBackend::Formats::r32g32b32a32_sint    },
            { emberCommon::TextureFormats::rgba32_sfloat.GetId(),  vulkanRendererBackend::Formats::r32g32b32a32_sfloat  },
            { emberCommon::TextureFormats::rgba64_uint.GetId(),    vulkanRendererBackend::Formats::r64g64b64a64_uint    },
            { emberCommon::TextureFormats::rgba64_sint.GetId(),    vulkanRendererBackend::Formats::r64g64b64a64_sint    },
            { emberCommon::TextureFormats::rgba64_sfloat.GetId(),  vulkanRendererBackend::Formats::r64g64b64a64_sfloat  }
        };
        auto it = map.find(format.GetId());
        return it != map.end() ? it->second : vulkanRendererBackend::Formats::undefined;
    }
}