#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128                  // max 2048 due to numthreads limit of 1024 (numthreads.x = BLOCK_SIZE/2)
RWStructuredBuffer<int> dataBuffer : register(u0);
RWStructuredBuffer<uint> permutationBuffer : register(u1);
groupshared int localValue[BLOCK_SIZE]; // max 32kB = 8192 ints (4bytes) = 2046 float4s (16bytes)
groupshared uint localPermutationValue[BLOCK_SIZE];
cbuffer Values : register(b2)
{
    uint bufferSize; // number of elements in the data buffer.
};



void CompareAndSwap(uint i, uint j)
{
    if (localValue[i] > localValue[j])
    {
        int tmp = localValue[i];
        localValue[i] = localValue[j];
        localValue[j] = tmp;
        
        uint tmpIndex = localPermutationValue[i];
        localPermutationValue[i] = localPermutationValue[j];
        localPermutationValue[j] = tmpIndex;
    }
}
void Flip(uint flipHeight, uint index)
{
    // Do not simplify these equations, as int floor rounding is required!
    uint halfFlipHeight = flipHeight / 2;
    uint block = index / halfFlipHeight;
    uint i = index + block * halfFlipHeight;
    uint j = flipHeight - 1 - index + 3 * halfFlipHeight * block;
    CompareAndSwap(i, j);
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
    for (uint flipHeight = 2; flipHeight <= BLOCK_SIZE; flipHeight *= 2)
    {
        GroupMemoryBarrierWithGroupSync();
        Flip(flipHeight, localIndex);
        for (uint disperseHeight = flipHeight / 2; disperseHeight > 1; disperseHeight /= 2)
        {
            GroupMemoryBarrierWithGroupSync();
            Disperse(disperseHeight, localIndex);
        }
    }
    
	// Write local memory back to buffer (2 values per thread):
    GroupMemoryBarrierWithGroupSync();
    if (2 * index + 0 < bufferSize) dataBuffer[2 * index + 0] = localValue[2 * localIndex + 0];
    if (2 * index + 1 < bufferSize) dataBuffer[2 * index + 1] = localValue[2 * localIndex + 1];
    if (2 * index + 0 < bufferSize) permutationBuffer[2 * index + 0] = localPermutationValue[2 * localIndex + 0];
    if (2 * index + 1 < bufferSize) permutationBuffer[2 * index + 1] = localPermutationValue[2 * localIndex + 1];
}