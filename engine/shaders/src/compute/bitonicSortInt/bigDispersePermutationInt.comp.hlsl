#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<int> dataBuffer : register(u0);
RWStructuredBuffer<int> permutationBuffer : register(u1);
cbuffer Values : register(b2)
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
        
        int tmpIndex = permutationBuffer[i];
        permutationBuffer[i] = permutationBuffer[j];
        permutationBuffer[j] = tmpIndex;
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