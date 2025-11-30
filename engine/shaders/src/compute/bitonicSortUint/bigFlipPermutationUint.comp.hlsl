#include "computePushConstant.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<uint> dataBuffer : register(u0);
RWStructuredBuffer<uint> permutationBuffer : register(u1);
cbuffer Values : register(b2)
{
    uint flipHeight; // height of the flip (number of elements involved in it).
    uint bufferSize; // number of elements in data buffer.
};



void CompareAndSwap(uint i, uint j)
{
    if (i >= bufferSize || j >= bufferSize)
        return;
    if (dataBuffer[i] > dataBuffer[j])
    {
        uint tmp = dataBuffer[i];
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
    uint index = threadID.x; // thread index � [0,bufferSize/2]
    
    // Do not simplify these equations, as int floor rounding is required!
    uint halfFlipHeight = flipHeight / 2;
    uint block = index / halfFlipHeight;
    uint i = index + block * halfFlipHeight;
    uint j = flipHeight - 1 - index + 3 * halfFlipHeight * block;
    CompareAndSwap(i, j);
}