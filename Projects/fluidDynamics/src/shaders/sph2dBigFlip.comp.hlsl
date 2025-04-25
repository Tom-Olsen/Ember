#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<int> cellKeyBuffer : register(u0);
RWStructuredBuffer<float2> positionBuffer : register(u1);
RWStructuredBuffer<float2> velocityBuffer : register(u2);
cbuffer Values : register(b3)
{
    int flipHeight; // height of the flip (number of elements involved in it).
    int bufferSize; // number of elements in data buffer.
};



void CompareAndSwap(int i, int j)
{
    if (i >= bufferSize || j >= bufferSize)
        return;
    if (cellKeyBuffer[i] > cellKeyBuffer[j])
    {
        int tempCellKey = cellKeyBuffer[i];
        float2 tempPosition = positionBuffer[i];
        float2 tempVelocity = velocityBuffer[i];
        cellKeyBuffer [i] = cellKeyBuffer [j];
        positionBuffer[i] = positionBuffer[j];
        velocityBuffer[i] = velocityBuffer[j];
        cellKeyBuffer [j] = tempCellKey;
        positionBuffer[j] = tempPosition;
        velocityBuffer[j] = tempVelocity;
    }
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x; // thread index € [0,bufferSize/2]
    
    // Do not simplify these equations, as int floor rounding is required!
    int halfFlipHeight = flipHeight / 2;
    int block = index / halfFlipHeight;
    int i = index + block * halfFlipHeight;
    int j = flipHeight - 1 - index + 3 * halfFlipHeight * block;
    CompareAndSwap(i, j);
}