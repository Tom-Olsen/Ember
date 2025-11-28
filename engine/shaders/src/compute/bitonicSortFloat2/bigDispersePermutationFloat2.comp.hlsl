#include "computePushConstant.hlsli"
#include "math.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<float2> dataBuffer : register(u0);
RWStructuredBuffer<uint> permutationBuffer : register(u1);
cbuffer Values : register(b2)
{
    uint disperseHeight; // height of the disperse (number of elements involved in it).
    uint bufferSize; // number of elements in data buffer.
};



void CompareAndSwap(uint i, uint j)
{
    if (i >= bufferSize || j >= bufferSize)
        return;
    
    float lenI = length(dataBuffer[i]);
    float lenJ = length(dataBuffer[j]);

    // Compare by length first:
    bool swap = false;
    if (lenI > lenJ)
        swap = true;
    else if (lenI == lenJ)
    {
        // Compute angles from (1,0) counterclockwise (0 ... 2pi):
        float angleA = atan2(dataBuffer[i].y, dataBuffer[i].x);
        float angleB = atan2(dataBuffer[j].y, dataBuffer[j].x);

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
        float2 tmp = dataBuffer[i];
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
    uint index = threadID.x; // thread index € [0,bufferSize/2]
    
    // Do not simplify these equations, as int floor rounding is required!
    uint halfDisperseHeight = disperseHeight / 2;
    uint block = index / halfDisperseHeight;
    uint i = index + block * halfDisperseHeight;
    uint j = i + halfDisperseHeight;
    CompareAndSwap(i, j);
}