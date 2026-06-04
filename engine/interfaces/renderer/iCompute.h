#pragma once
#include "emberMath.h"
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>



namespace emberBackendInterface
{
    enum class ComputeBarrierFlag : uint16_t
    {
        none = 0,
        uniformRead = 1 << 0,
        shaderRead = 1 << 1,
        shaderWrite = 1 << 2,
        sampledRead = 1 << 3,
        storageRead = 1 << 4,
        storageWrite = 1 << 5,
        memoryRead = 1 << 6,
        memoryWrite = 1 << 7,
        accelerationStructureRead = 1 << 8,
        descriptorBufferRead = 1 << 9
    };



    inline constexpr ComputeBarrierFlag operator|(ComputeBarrierFlag lhs, ComputeBarrierFlag rhs)
    {
        using T = std::underlying_type_t<ComputeBarrierFlag>;
        return static_cast<ComputeBarrierFlag>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }
    inline constexpr ComputeBarrierFlag operator&(ComputeBarrierFlag lhs, ComputeBarrierFlag rhs)
    {
        using T = std::underlying_type_t<ComputeBarrierFlag>;
        return static_cast<ComputeBarrierFlag>(static_cast<T>(lhs) & static_cast<T>(rhs));
    }
    inline constexpr ComputeBarrierFlag& operator|=(ComputeBarrierFlag& lhs, ComputeBarrierFlag rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }



    inline constexpr bool HasFlag(ComputeBarrierFlag flags, ComputeBarrierFlag flag)
    {
        return (flags & flag) != ComputeBarrierFlag::none;
    }
    inline std::string ComputeBarrierFlagsToString(ComputeBarrierFlag barrierFlags)
    {
        if (barrierFlags == ComputeBarrierFlag::none)
            return "none";

        std::string result;
        auto appendFlag = [&result, barrierFlags](ComputeBarrierFlag flag, std::string_view name)
        {
            if (!HasFlag(barrierFlags, flag))
                return;
            if (!result.empty())
                result += "|";
            result += name;
        };

        appendFlag(ComputeBarrierFlag::uniformRead, "uniformRead");
        appendFlag(ComputeBarrierFlag::shaderRead, "shaderRead");
        appendFlag(ComputeBarrierFlag::shaderWrite, "shaderWrite");
        appendFlag(ComputeBarrierFlag::sampledRead, "sampledRead");
        appendFlag(ComputeBarrierFlag::storageRead, "storageRead");
        appendFlag(ComputeBarrierFlag::storageWrite, "storageWrite");
        appendFlag(ComputeBarrierFlag::memoryRead, "memoryRead");
        appendFlag(ComputeBarrierFlag::memoryWrite, "memoryWrite");
        appendFlag(ComputeBarrierFlag::accelerationStructureRead, "accelerationStructureRead");
        appendFlag(ComputeBarrierFlag::descriptorBufferRead, "descriptorBufferRead");
        return result.empty() ? "unknown" : result;
    }



    // Forward declerations:
    class IBuffer;
    class IComputeShader;
    class IDescriptorSetBinding;



    class ICompute
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~ICompute() = default;

        class IAsync
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IAsync() = default;

            // Dispatch logic:
            virtual uint32_t CreateComputeSession() = 0;
            virtual void DispatchComputeSession(uint32_t sessionID, float time, float deltaTime) = 0;
            virtual bool IsFinished(uint32_t sessionID) = 0;
            virtual void WaitForFinish(uint32_t sessionID) = 0;

            // Workload recording:
            virtual IDescriptorSetBinding* RecordComputeShader(uint32_t sessionID, IComputeShader* pIComputeShader, Uint3 threadCount) = 0;
            virtual void RecordBarrier(uint32_t sessionID, ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags) = 0;
        };

        class IPostRender
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IPostRender() = default;

            // Workload recording:
            virtual IDescriptorSetBinding* RecordComputeShader(IComputeShader* pIComputeShader) = 0;
        };



        class IPreRender
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IPreRender() = default;

            // Workload recording:
            virtual IDescriptorSetBinding* RecordComputeShader(IComputeShader* pIComputeShader, Uint3 threadCount) = 0;
            virtual void RecordBarrier(ComputeBarrierFlag srcBarrierFlags, ComputeBarrierFlag dstBarrierFlags) = 0;
        };



        virtual IAsync* GetAsyncComputeInterfaceHandle() = 0;
        virtual IPostRender* GetPostRenderComputeInterfaceHandle() = 0;
        virtual IPreRender* GetPreRenderComputeInterfaceHandle() = 0;
    };
}