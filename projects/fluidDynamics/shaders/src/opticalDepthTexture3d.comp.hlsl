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
    float3 extinction;
    int opticalDepthTextureDepth;
};
Texture3D<float> densityTexture : register(t100, CALL_SET);
[[vk::image_format("rgba16f")]] RWTexture3D<float4> opticalDepthTexture : register(u200, CALL_SET);



float3 PositionLightToSimulation(float3 positionLight)
{
    float3 lightBoundsCenter = 0.5f * (lightBoundsMin + lightBoundsMax);
    float3 offsetLight = (positionLight - 0.5f) * (lightBoundsMax - lightBoundsMin);
    return lightBoundsCenter + mul(lightToSimulationRotation, float4(offsetLight, 0.0f)).xyz;
}
float3 PositionSimulationToFluid(float3 positionSimulation)
{
    float3 fluidBoundsCenter = 0.5f * (fluidBoundsMin + fluidBoundsMax);
    float3 offsetFluid = mul(simulationToFluidRotation, float4(positionSimulation - fluidBoundsCenter, 0.0f)).xyz;
    return (fluidBoundsCenter + offsetFluid - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
}
float SampleDensity(float3 positionFluid)
{
    if (any(positionFluid < 0.0f) || any(positionFluid > 1.0f))
        return 0.0f;
    return densityTexture.SampleLevel(colorSamplerClampEdge, positionFluid, 0).r;
}



[numthreads(16, 16, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    // Geometry:
    float3 textureResolution = float3(pc.threadCount.xy, opticalDepthTextureDepth);
    float stepLengthSimulation = (lightBoundsMax.z - lightBoundsMin.z) / textureResolution.z;

    // Position in light/simulation/fluid frame:
    float3 positionLight = (float3(threadID.xy, 0.0f) + 0.5f) / textureResolution;
    float3 positionSimulation = PositionLightToSimulation(positionLight);
    float3 positionFluid = PositionSimulationToFluid(positionSimulation);

    // Step in light/simulation/fluid frame:
    float3 stepLight = float3(0.0f, 0.0f, 1.0f / textureResolution.z);
    float3 stepSimulation = mul(lightToSimulationRotation, float4(stepLight * (lightBoundsMax - lightBoundsMin), 0.0f)).xyz;
    float3 stepFluid = mul(simulationToFluidRotation, float4(stepSimulation, 0.0f)).xyz / (fluidBoundsMax - fluidBoundsMin);

    // Integrate RGB optical depth through the light volume using the trapezoid rule:
    float previousDensity = SampleDensity(positionFluid);
    float3 opticalDepth = 0.5f * extinction * previousDensity * stepLengthSimulation;
    opticalDepthTexture[uint3(threadID.xy, 0)] = float4(opticalDepth, 0.0f);
    for (int z = 1; z < opticalDepthTextureDepth; z++)
    {
        positionFluid += stepFluid;
        float density = SampleDensity(positionFluid);
        opticalDepth += 0.5f * extinction * (previousDensity + density) * stepLengthSimulation;
        opticalDepthTexture[uint3(threadID.xy, z)] = float4(opticalDepth, 0.0f);
        previousDensity = density;
    }
}