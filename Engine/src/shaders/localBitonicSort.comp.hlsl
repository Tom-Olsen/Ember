#include "computePushConstant.hlsli"



#define COUNT 512   // max 2048 due to numthreads limit of 1024 (numthreads.else = COUNT/2)
RWStructuredBuffer<int> dataBuffer : register(u0);
groupshared int localValue[COUNT]; // max 32kB = 8192 ints (4bytes) = 2046 float4s (16bytes)
cbuffer Values : register(b1)
{
    int offset;
    int bufferSize;
};



void LocalCompareAndSwap(int x, int y)
{
    if (localValue[x] > localValue[y])
    {
        uint tmp = localValue[x];
        localValue[x] = localValue[y];
        localValue[y] = tmp;
    }
}
void Flip(int flipHeight, uint localIndex)
{
    int q = ((2 * localIndex) / flipHeight) * flipHeight;
    int flipHeightHalf = flipHeight / 2;
    int mod = localIndex % flipHeightHalf;
    int x = q + mod;
    int y = q - mod - 1 + flipHeight;
    LocalCompareAndSwap(x, y);
}
void Disperse(int k, uint localIndex)
{
    int q = ((2 * localIndex) / k) * k;
    int kHalf = k / 2;
    int mod = localIndex % kHalf;
    int x = q + mod;
    int y = q + mod + kHalf;
    LocalCompareAndSwap(x, y);
}



[numthreads(COUNT / 2, 1, 1)]
void main(uint3 localThreadID : SV_GroupThreadID)
{
    uint localIndex = localThreadID.x;
    
	// Each local thread hadels two elements, as there are twice as many elments as threads.
    localValue[2 * localIndex    ] = (2 * localIndex     < bufferSize) ? dataBuffer[2 * localIndex     + offset] : 0x7FFFFFFF;
    localValue[2 * localIndex + 1] = (2 * localIndex + 1 < bufferSize) ? dataBuffer[2 * localIndex + 1 + offset] : 0x7FFFFFFF;
    
    for (uint flipHeight = 2; flipHeight <= COUNT; flipHeight *= 2)
    {
        GroupMemoryBarrierWithGroupSync();
        Flip(flipHeight, localIndex);
    
        for (uint kk = flipHeight / 2; kk > 1; kk /= 2)
        {
            GroupMemoryBarrierWithGroupSync();
            Disperse(kk, localIndex);
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
	// Write local memory back to buffer:
    if (2 * localIndex     < bufferSize)
        dataBuffer[2 * localIndex     + offset] = localValue[2 * localIndex    ];
    if (2 * localIndex + 1 < bufferSize)
        dataBuffer[2 * localIndex + 1 + offset] = localValue[2 * localIndex + 1];
}