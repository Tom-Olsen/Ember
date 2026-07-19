#include "computeShaderCommon.hlsli"
#include "globalSet.hlsli"



cbuffer CallValues : register(b300, CALL_SET)
{
    float3 lightBoundsMin;
    float3 lightBoundsMax;
    float4x4 lightToSimulationRotation;
    float3 fluidBoundsMin;
    float3 fluidBoundsMax;
    float4x4 simulationToFluidRotation;
    float extinctionCoefficient;    // = absorption + scattering
    int opticalDepthTextureDepth;
};
Texture3D<float> densityTexture : register(t100, CALL_SET);
[[vk::image_format("r16f")]] RWTexture3D<float> opticalDepthTexture : register(u200, CALL_SET);



float SampleDensity(float3 positionFluid)
{
    float3 uvw = (positionFluid - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
    if (any(uvw < 0.0f) || any(uvw > 1.0f))
        return 0.0f;
    return densityTexture.SampleLevel(colorSamplerClampEdge, uvw, 0).r;
}



[numthreads(16, 16, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    // Geometry:
    float3 textureResolution = float3(pc.threadCount.xy, opticalDepthTextureDepth);
    float3 voxelSize = (lightBoundsMax - lightBoundsMin) / textureResolution;
    float3 lightBoundsCenter = 0.5f * (lightBoundsMin + lightBoundsMax);
    float3 fluidBoundsCenter = 0.5f * (fluidBoundsMin + fluidBoundsMax);

    // Position in light/simulation/fluid frame:
    float3 positionLight = lightBoundsMin + (float3(threadID.xy, 0.0f) + 0.5f) * voxelSize;
    float3 positionSimulation = lightBoundsCenter + mul(lightToSimulationRotation, float4(positionLight - lightBoundsCenter, 0.0f)).xyz;
    float3 positionFluid = fluidBoundsCenter + mul(simulationToFluidRotation, float4(positionSimulation - fluidBoundsCenter, 0.0f)).xyz;

    // Step size in light/simulation/fluid frame:
    float3 stepLight = float3(0.0f, 0.0f, voxelSize.z);
    float3 stepSimulation = mul(lightToSimulationRotation, float4(stepLight, 0.0f)).xyz;
    float3 stepFluid = mul(simulationToFluidRotation, float4(stepSimulation, 0.0f)).xyz;

    // Step through the fluid and accumulate optical depth via trapezoid integration:
    float previousDensity = SampleDensity(positionFluid);
    float opticalDepth = 0.5f * extinctionCoefficient * previousDensity * voxelSize.z;
    opticalDepthTexture[uint3(threadID.xy, 0)] = opticalDepth;
    for (int z = 1; z < opticalDepthTextureDepth; z++)
    {
        positionFluid += stepFluid;
        float density = SampleDensity(positionFluid);
        opticalDepth += 0.5f * extinctionCoefficient * (previousDensity + density) * voxelSize.z;
        opticalDepthTexture[uint3(threadID.xy, z)] = opticalDepth;
        previousDensity = density;
    }
}