#include "computePushConstant.hlsli"
#include "math.hlsli"



#define BLOCK_SIZE 128
RWStructuredBuffer<float2> dataBuffer : register(u0);
cbuffer Values : register(b1)
{
    uint flipHeight; // height of the flip (number of elements involved in it).
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
    }
}



[numthreads(BLOCK_SIZE / 2, 1, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x; // thread index € [0,bufferSize/2]
    
    // Do not simplify these equations, as int floor rounding is required!
    uint halfFlipHeight = flipHeight / 2;
    uint block = index / halfFlipHeight;
    uint i = index + block * halfFlipHeight;
    uint j = flipHeight - 1 - index + 3 * halfFlipHeight * block;
    CompareAndSwap(i, j);
}