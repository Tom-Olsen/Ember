#pragma once
#include "commonAccessMask.h"
#include "emberMath.h"



namespace emberBackendInterface
{
    // Forward declerations:
    class IBuffer;
    class IComputeShader;
    class IShaderProperties;



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
            virtual void RecordComputeShader(uint32_t sessionID, IComputeShader* pIComputeShader, IShaderProperties* pIShaderProperties, Uint3 threadCount) = 0;
            virtual IShaderProperties* RecordComputeShader(uint32_t sessionID, IComputeShader* pIComputeShader, Uint3 threadCount) = 0;
            virtual void RecordBarrier(uint32_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask) = 0;
        };



        class IImmediate
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IImmediate() = default;

            // Immediate dispatch call:
            virtual void Dispatch(IComputeShader* pIComputeShader, IShaderProperties* pIShaderProperties, Uint3 threadCount, float time, float deltaTime) = 0;
        };



        class IPostRender
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IPostRender() = default;

            // Workload recording:
            virtual void RecordComputeShader(IComputeShader* pIComputeShader, IShaderProperties* pIShaderProperties) = 0;
            virtual IShaderProperties* RecordComputeShader(IComputeShader* pIComputeShader) = 0;
        };



        class IPreRender
        {
        public: // Methods:
            // Virtual destructor for v-table:
            virtual ~IPreRender() = default;

            // Workload recording:
            virtual void RecordComputeShader(IComputeShader* pIComputeShader, IShaderProperties* pIShaderProperties, Uint3 threadCount) = 0;
            virtual IShaderProperties* RecordComputeShader(IComputeShader* pIComputeShader, Uint3 threadCount) = 0;
            virtual void RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask) = 0;
        };
    };
}