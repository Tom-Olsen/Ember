#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128                  // max 2048 due to numthreads limit of 1024 (numthreads.x = BLOCK_SIZE/2)
RWStructuredBuffer<int> cellKeyBuffer : register(u0);
RWStructuredBuffer<float3> positionBuffer : register(u1);
RWStructuredBuffer<float3> velocityBuffer : register(u2);
groupshared int localCellKeyBuffer[BLOCK_SIZE]; 
groupshared float3 localPositionBuffer[BLOCK_SIZE];
groupshared float3 localVelocityBuffer[BLOCK_SIZE];
cbuffer Values : register(b3)
{
    int bufferSize; // number of elements in the data buffer.
};



void CompareAndSwap(int i, int j)
{
    if (localCellKeyBuffer[i] > localCellKeyBuffer[j])
    {
        int tempCellKey = localCellKeyBuffer[i];
        float3 tempPosition = localPositionBuffer[i];
        float3 tempVelocity = localVelocityBuffer[i];
        localCellKeyBuffer [i] = localCellKeyBuffer [j];
        localPositionBuffer[i] = localPositionBuffer[j];
        localVelocityBuffer[i] = localVelocityBuffer[j];
        localCellKeyBuffer [j] = tempCellKey;
        localPositionBuffer[j] = tempPosition;
        localVelocityBuffer[j] = tempVelocity;
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
    uint localIndex = localThreadID.x;  // local thead index € [0,BLOCK_SIZE/2]
    uint index = threadID.x;            // thread index € [0,bufferSize/2]
    
	// Load buffer into local memory (2 values per thread):
    localCellKeyBuffer [2 * localIndex + 0] = (2 * index + 0 < bufferSize) ?  cellKeyBuffer[2 * index + 0] : 0x7FFFFFFF;
    localPositionBuffer[2 * localIndex + 0] = (2 * index + 0 < bufferSize) ? positionBuffer[2 * index + 0] : 0;
    localVelocityBuffer[2 * localIndex + 0] = (2 * index + 0 < bufferSize) ? velocityBuffer[2 * index + 0] : 0;
    localCellKeyBuffer [2 * localIndex + 1] = (2 * index + 1 < bufferSize) ?  cellKeyBuffer[2 * index + 1] : 0x7FFFFFFF;
    localPositionBuffer[2 * localIndex + 1] = (2 * index + 1 < bufferSize) ? positionBuffer[2 * index + 1] : 0;
    localVelocityBuffer[2 * localIndex + 1] = (2 * index + 1 < bufferSize) ? velocityBuffer[2 * index + 1] : 0;
    
    // Execute local bitonic sort on local memory: (only sorts elements within the same block)
    for (uint disperseHeight = BLOCK_SIZE / 2; disperseHeight > 1; disperseHeight /= 2)
    {
        GroupMemoryBarrierWithGroupSync();
        Disperse(disperseHeight, localIndex);
    }
    
	// Write local memory back to buffer (2 values per thread):
    GroupMemoryBarrierWithGroupSync();
    if (2 * index + 0 < bufferSize)
    {
        cellKeyBuffer [2 * index + 0] = localCellKeyBuffer [2 * localIndex + 0];
        positionBuffer[2 * index + 0] = localPositionBuffer[2 * localIndex + 0];
        velocityBuffer[2 * index + 0] = localVelocityBuffer[2 * localIndex + 0];
    }
    if (2 * index + 1 < bufferSize)
    {
        cellKeyBuffer [2 * index + 1] = localCellKeyBuffer [2 * localIndex + 1];
        positionBuffer[2 * index + 1] = localPositionBuffer[2 * localIndex + 1];
        velocityBuffer[2 * index + 1] = localVelocityBuffer[2 * localIndex + 1];
    }
}