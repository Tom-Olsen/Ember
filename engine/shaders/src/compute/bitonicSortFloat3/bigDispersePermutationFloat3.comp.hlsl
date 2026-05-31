#include "computeShaderCommon.hlsli"
#include "bitonicSortBlockSize.hlsli"
#include "sortCompareFloat3.hlsli"



RWStructuredBuffer<float3> dataBuffer : register(u200, CALL_SET);
RWStructuredBuffer<uint> permutationBuffer : register(u201, CALL_SET);
cbuffer Values : register(b300, CALL_SET)
{
    uint disperseHeight; // height of the disperse (number of elements involved in it).
    uint bufferSize; // number of elements in data buffer.
};



void CompareAndSwap(uint i, uint j)
{
    if (i >= bufferSize || j >= bufferSize)
        return;
    if (SortCompare(dataBuffer[i], dataBuffer[j]))
    {
        float3 tmp = dataBuffer[i];
        dataBuffer[i] = dataBuffer[j];
        dataBuffer[j] = tmp;

        uint tmpIndex = permutationBuffer[i];
        permutationBuffer[i] = permutationBuffer[j];
        permutationBuffer[j] = tmpIndex;
    }
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x; // thread index in [0,bufferSize/2]

    // Do not simplify these equations, as int floor rounding is required!
    uint halfDisperseHeight = disperseHeight / 2;
    uint block = index / halfDisperseHeight;
    uint i = index + block * halfDisperseHeight;
    uint j = i + halfDisperseHeight;
    CompareAndSwap(i, j);
}