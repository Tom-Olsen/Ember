#pragma once
#include <cstdint>



namespace emberCommon
{
    enum class PipelineStage : uint32_t
    {
        none = 0,
        vertexShader = 1 << 0,
        fragmentShader = 1 << 1,
        computeShader = 1 << 2,
        copy = 1 << 3,
        allGraphics = 1 << 4,
        allCommands = 1 << 5
    };
    inline constexpr PipelineStage operator|(PipelineStage lhs, PipelineStage rhs)
    {
        return static_cast<PipelineStage>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
    }
    inline constexpr bool HasFlag(PipelineStage stages, PipelineStage stage)
    {
        return (static_cast<uint32_t>(stages) & static_cast<uint32_t>(stage)) != 0;
    }
}