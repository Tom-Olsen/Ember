#include "fragmentShaderCommon.hlsli"
#include "vertexShaderCommon.hlsli"
Texture3D<float> densityTexture : register(t100, SHADER_SET);
Texture3D<float4> opticalDepthTexture : register(t101, SHADER_SET);



cbuffer Values : register(b300, SHADER_SET)
{
    float3 fluidSize;
    float absorption;
    float3 scattering;
    float rayStepLengthSimulation;
    float4x4 fluidToLightMatrix;
    int renderVolumetricLight;
};



struct FragmentInput
{
    float4 positionClip : SV_POSITION;  // position in clip space: x,y in [-1,1] z in[0,1]
    float3 positionWorld : TEXCOORD0;   // position in world space
    float3 positionLocal : TEXCOORD1;   // position in local/model space
};



bool RayBoxIntersection(float3 rayOrigin, float3 rayDirection, float3 boxMin, float3 boxMax, out float t0, out float t1)
{
    float3 inverseDirection = 1.0f / rayDirection;
    float3 vect0 = (boxMin - rayOrigin) * inverseDirection;
    float3 vect1 = (boxMax - rayOrigin) * inverseDirection;
    float3 tMin = min(vect0, vect1);
    float3 tMax = max(vect0, vect1);

    t0 = max(max(tMin.x, tMin.y), tMin.z);
    t1 = min(min(tMax.x, tMax.y), tMax.z);
    return t1 > max(t0, 0.0f);
}
float GetNearPlaneRayStartLocal(float3 cameraPositionLocal, float3 rayDirectionLocal, float3 rayDirectionWorld)
{
    float3 rayDirectionCamera = mul(camera_viewMatrix, float4(rayDirectionWorld, 0.0f)).xyz;
    float rayStartWorld = -GetCameraNearClip() / rayDirectionCamera.z;
    float3 nearPlanePositionWorld = camera_position.xyz + rayDirectionWorld * rayStartWorld;
    float3 nearPlanePositionLocal = mul(model_worldToLocalMatrix, float4(nearPlanePositionWorld, 1.0f)).xyz;
    return dot(nearPlanePositionLocal - cameraPositionLocal, rayDirectionLocal);
}



float4 main(FragmentInput input) : SV_TARGET
{
    static const uint maxStepCount = 256;

    // Compute ray enter and exit t in local space:
    static const float3 boxMinLocal = -0.5f;
    static const float3 boxMaxLocal = 0.5f;
    float3 cameraPositionLocal = mul(model_worldToLocalMatrix, camera_position).xyz;
    float3 rayDirectionLocal = normalize(input.positionLocal - cameraPositionLocal);
    float t0;
    float t1;
    if (!RayBoxIntersection(cameraPositionLocal, rayDirectionLocal, boxMinLocal, boxMaxLocal, t0, t1))
        discard;

    // Clamp t0 to camera near plane if its inside the fluid:
    float3 rayDirectionWorld = normalize(input.positionWorld - camera_position.xyz);
    t0 = max(t0, GetNearPlaneRayStartLocal(cameraPositionLocal, rayDirectionLocal, rayDirectionWorld));
    float deltaT = t1 - t0;
    if (deltaT <= 0.0f)
        discard;

    // Fluid coordinates are normalized texture coordinates. fluidSize converts their distances to simulation units:
    float rayLengthSimulation = deltaT * length(fluidSize * rayDirectionLocal);
    uint stepCount = clamp((uint)ceil(rayLengthSimulation / rayStepLengthSimulation), 1u, maxStepCount);
    float stepLengthLocal = deltaT / stepCount;
    float stepLengthSimulation = rayLengthSimulation / stepCount;

    // Ray casting:
    float3 incidentLightIntensity = light_dirCount <= 0 ? 0.0f
        : light_directionData[0].colorIntensity.xyz * light_directionData[0].colorIntensity.w;
    float3 extinction = absorption + scattering;
    float3 transmittance = 1.0f;
    float3 scatteredLight = 0.0f;
    for (uint i = 0; i < stepCount; i++)
    {
        // Sample density:
        float t = t0 + (i + 0.5f) * stepLengthLocal;
        float3 positionLocal = cameraPositionLocal + rayDirectionLocal * t;
        float3 positionFluid = positionLocal + 0.5f;
        float density = densityTexture.SampleLevel(colorSamplerClampEdge, positionFluid, 0).r;

        // Sample the first directional light after attenuation through the fluid:
        float3 lightIntensity = incidentLightIntensity;
        if (hasOpticalDepthTexture != 0)
        {
            float3 positionLight = mul(fluidToLightMatrix, float4(positionFluid, 1.0f)).xyz;
            float3 opticalDepth = opticalDepthTexture.SampleLevel(colorSamplerClampEdge, positionLight, 0).rgb;
            lightIntensity *= exp(-opticalDepth);
        }

        // Analytic integration over a constant-density step of the radiative transfer equation:
        float3 stepTransmittance = exp(-density * extinction * stepLengthSimulation);
        float3 inScatteredLight = lightIntensity * scattering / extinction * (1.0f - stepTransmittance);
        scatteredLight += transmittance * inScatteredLight;
        transmittance *= stepTransmittance;
        if (all(transmittance < 0.01f))
            break;
    }

    // Fixed-function blending has one alpha channel, so use perceived transmittance as coverage:
    float alpha = 1.0f - dot(transmittance, float3(0.2126f, 0.7152f, 0.0722f));
    float3 color = alpha > 0.0f ? scatteredLight / alpha : 0.0f;
    return float4(color, alpha);
}