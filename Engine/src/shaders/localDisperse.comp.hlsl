#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128                  // max 2048 due to numthreads limit of 1024 (numthreads.x = BLOCK_SIZE/2)
RWStructuredBuffer<int> dataBuffer : register(u0);
groupshared int localValue[BLOCK_SIZE]; // max 32kB = 8192 ints (4bytes) = 2046 float4s (16bytes)
cbuffer Values : register(b1)
{
    int startDisperseHeight;    // height of first disperse.
    int bufferSize;             // number of elements in the data buffer.
};



void CompareAndSwap(int i, int j)
{
    if (localValue[i] > localValue[j])
    {
        uint tmp = localValue[i];
        localValue[i] = localValue[j];
        localValue[j] = tmp;
    }
}
void Disperse(int disperseHeight, uint index)
{
    // Do not simplify these equations, as int floor rounding is required!
    int halfDisperseHeight = disperseHeight / 2;
    int block = index / halfDisperseHeight;
    int i = index + block * halfDisperseHeight;
    int j = i + halfDisperseHeight;
    CompareAndSwap(i, j);
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 localThreadID : SV_GroupThreadID, uint3 threadID : SV_DispatchThreadID)
{
    uint localIndex = localThreadID.x;  // local thead index � [0,BLOCK_SIZE/2]
    uint index = threadID.x;            // thread index � [0,bufferSize/2]
    
	// Load buffer into local memory (2 values per thread):
    localValue[2 * localIndex + 0] = (2 * index + 0 < bufferSize) ? dataBuffer[2 * index + 0] : 0x7FFFFFFF;
    localValue[2 * localIndex + 1] = (2 * index + 1 < bufferSize) ? dataBuffer[2 * index + 1] : 0x7FFFFFFF;
    
    // Execute local bitonic sort on local memory: (only sorts elements within the same block)
    for (uint disperseHeight = startDisperseHeight; disperseHeight > 1; disperseHeight /= 2)
    {
        GroupMemoryBarrierWithGroupSync();
        Disperse(disperseHeight, localIndex);
    }
    
	// Write local memory back to buffer (2 values per thread):
    GroupMemoryBarrierWithGroupSync();
    if (2 * index + 0 < bufferSize) dataBuffer[2 * index + 0] = localValue[2 * localIndex + 0];
    if (2 * index + 1 < bufferSize) dataBuffer[2 * index + 1] = localValue[2 * localIndex + 1];
}