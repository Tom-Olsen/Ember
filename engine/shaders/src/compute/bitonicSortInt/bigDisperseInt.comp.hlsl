#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<int> dataBuffer : register(u0);
cbuffer Values : register(b1)
{
    int disperseHeight; // height of the disperse (number of elements involved in it).
    int bufferSize;     // number of elements in data buffer.
};



void CompareAndSwap(int i, int j)
{
    if (i >= bufferSize || j >= bufferSize)
        return;
    if (dataBuffer[i] > dataBuffer[j])
    {
        uint tmp = dataBuffer[i];
        dataBuffer[i] = dataBuffer[j];
        dataBuffer[j] = tmp;
    }
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x; // thread index € [0,bufferSize/2]
    
    // Do not simplify these equations, as int floor rounding is required!
    int halfDisperseHeight = disperseHeight / 2;
    int block = index / halfDisperseHeight;
    int i = index + block * halfDisperseHeight;
    int j = i + halfDisperseHeight;
    CompareAndSwap(i, j);
}