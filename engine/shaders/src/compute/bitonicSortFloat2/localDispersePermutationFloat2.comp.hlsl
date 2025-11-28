#include "computePushConstant.hlsli"
#include "math.hlsli"



#define BLOCK_SIZE 128                  // max 2048 due to numthreads limit of 1024 (numthreads.x = BLOCK_SIZE/2)
RWStructuredBuffer<float2> dataBuffer : register(u0);
RWStructuredBuffer<uint> permutationBuffer : register(u1);
groupshared float2 localValue[BLOCK_SIZE]; // max 32kB = 8192 ints (4bytes) = 2046 float4s (16bytes)
groupshared uint localPermutationValue[BLOCK_SIZE];
cbuffer Values : register(b2)
{
    uint bufferSize; // number of elements in the data buffer.
};



void CompareAndSwap(uint i, uint j)
{
    float lenI = length(localValue[i]);
    float lenJ = length(localValue[j]);

    // Compare by length first:
    bool swap = false;
    if (lenI > lenJ)
        swap = true;
    else if (lenI == lenJ)
    {
        // Compute angles from (1,0) counterclockwise (0 ... 2pi):
        float angleA = atan2(localValue[i].y, localValue[i].x);
        float angleB = atan2(localValue[j].y, localValue[j].x);

        // atan2 returns [-pi, pi], convert to [0, 2pi]
        if (angleA < 0)
            angleA += 2.0 * math_PI;
        if (angleB < 0)
            angleB += 2.0 * math_PI;

        if (angleA > angleB)
            swap = true;
    }

    if (swap)
    {
        float2 tmp = localValue[i];
        localValue[i] = localValue[j];
        localValue[j] = tmp;
        
        uint tmpPermutation = localPermutationValue[i];
        localPermutationValue[i] = localPermutationValue[j];
        localPermutationValue[j] = tmpPermutation;
    }
}
void Disperse(uint disperseHeight, uint index)
{
    // Do not simplify these equations, as int floor rounding is required!
    uint halfDisperseHeight = disperseHeight / 2;
    uint block = index / halfDisperseHeight;
    uint i = index + block * halfDisperseHeight;
    uint j = i + halfDisperseHeight;
    CompareAndSwap(i, j);
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 localThreadID : SV_GroupThreadID, uint3 threadID : SV_DispatchThreadID)
{
    uint localIndex = localThreadID.x;  //  local thread index € [0,BLOCK_SIZE/2]
    uint index = threadID.x;            // global thread index € [0,bufferSize/2]
    
	// Load buffer into local memory (2 values per thread):
    localValue[2 * localIndex + 0] = (2 * index + 0 < bufferSize) ? dataBuffer[2 * index + 0] : 0x7FFFFFFF;
    localValue[2 * localIndex + 1] = (2 * index + 1 < bufferSize) ? dataBuffer[2 * index + 1] : 0x7FFFFFFF;
    localPermutationValue[2 * localIndex + 0] = (2 * index + 0 < bufferSize) ? permutationBuffer[2 * index + 0] : -1;
    localPermutationValue[2 * localIndex + 1] = (2 * index + 1 < bufferSize) ? permutationBuffer[2 * index + 1] : -1;
    
    // Execute local bitonic sort on local memory: (only sorts elements within the same block)
    for (uint disperseHeight = BLOCK_SIZE; disperseHeight > 1; disperseHeight /= 2)
    {
        GroupMemoryBarrierWithGroupSync();
        Disperse(disperseHeight, localIndex);
    }
    
	// Write local memory back to buffer (2 values per thread):
    GroupMemoryBarrierWithGroupSync();
    if (2 * index + 0 < bufferSize) dataBuffer[2 * index + 0] = localValue[2 * localIndex + 0];
    if (2 * index + 1 < bufferSize) dataBuffer[2 * index + 1] = localValue[2 * localIndex + 1];
    if (2 * index + 0 < bufferSize) permutationBuffer[2 * index + 0] = localPermutationValue[2 * localIndex + 0];
    if (2 * index + 1 < bufferSize) permutationBuffer[2 * index + 1] = localPermutationValue[2 * localIndex + 1];
}