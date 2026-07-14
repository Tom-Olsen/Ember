#include "fragmentShaderCommon.hlsli"
#include "vertexShaderCommon.hlsli"
Texture3D<float> densityTexture : register(t100, SHADER_SET);



cbuffer Values : register(b300, SHADER_SET)
{
    float3 volumeHalfSize;
    float densityScale;
    float absorption;
    float rayStepLength;
};



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 diffuseColor;    // (1.0, 1.0, 1.0)
    float4 scaleOffset;     // .xy = scale, .zw = offset
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float3 worldNormal : NORMAL;        // normal in world space
    float3 worldTangent : TANGENT;      // tangent in world space
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
    float3 localPosition : TEXCOORD2;   // position in local/model space
};



bool RayBoxIntersection(float3 rayOrigin, float3 rayDirection, float3 boxMin, float3 boxMax, out float tEnter, out float tExit)
{
    float3 invDirection = 1.0f / rayDirection;
    float3 t0 = (boxMin - rayOrigin) * invDirection;
    float3 t1 = (boxMax - rayOrigin) * invDirection;
    float3 tMin = min(t0, t1);
    float3 tMax = max(t0, t1);

    tEnter = max(max(tMin.x, tMin.y), tMin.z);
    tExit = min(min(tMax.x, tMax.y), tMax.z);
    return tExit > max(tEnter, 0.0f);
}
float GetCameraNearClip()
{
    return camera_projMatrix[2][3] / camera_projMatrix[2][2];
}
float GetLocalNearPlaneRayStart(float3 localCameraPosition, float3 localRayDirection, float3 worldRayDirection)
{
    float3 cameraRayDirection = mul(camera_viewMatrix, float4(worldRayDirection, 0.0f)).xyz;
    float worldRayStart = -GetCameraNearClip() / cameraRayDirection.z;
    float3 worldNearPlanePosition = camera_position.xyz + worldRayDirection * worldRayStart;
    float3 localNearPlanePosition = mul(model_worldToLocalMatrix, float4(worldNearPlanePosition, 1.0f)).xyz;
    return dot(localNearPlanePosition - localCameraPosition, localRayDirection);
}



float4 main(FragmentInput input) : SV_TARGET
{
    static const uint maxStepCount = 256;

    // Compute ray segment inside the density cube:
    float3 boxMax = volumeHalfSize;
    float3 boxMin = -volumeHalfSize;
    float3 localCameraPosition = mul(model_worldToLocalMatrix, camera_position).xyz;
    float3 worldRayDirection = normalize(input.worldPosition - camera_position.xyz);
    float3 rayDirection = normalize(input.localPosition - localCameraPosition);
    float tEnter;
    float tExit;
    if (!RayBoxIntersection(localCameraPosition, rayDirection, boxMin, boxMax, tEnter, tExit))
        discard;

    float rayStart = max(tEnter, GetLocalNearPlaneRayStart(localCameraPosition, rayDirection, worldRayDirection));
    float rayLength = tExit - rayStart;
    if (rayLength <= 0.0f)
        discard;

    float3 volumeSize = 2 * volumeHalfSize;
    uint stepCount = clamp((uint)ceil(rayLength / rayStepLength), 1u, maxStepCount);
    float stepLength = rayLength / stepCount;

    // Integrate front-to-back volume color:
    float transmittance = 1.0f;
    float3 color = 0.0f;
    for (uint i = 0; i < stepCount; i++)
    {
        float t = rayStart + (i + 0.5f) * stepLength;
        float3 localPosition = localCameraPosition + rayDirection * t;
        float3 uvw = (localPosition - boxMin) / volumeSize;
        float density = densityTexture.SampleLevel(colorSamplerClampEdge, uvw, 0).r;
        float sampleAlpha = 1.0f - exp(-density * absorption * stepLength);
        float densityT = saturate(density / densityScale);
        float3 sampleColor = lerp(float3(0.05f, 0.25f, 0.9f), float3(1.0f, 0.95f, 0.75f), densityT);

        color += transmittance * sampleAlpha * sampleColor;
        transmittance *= 1.0f - sampleAlpha;
        if (transmittance < 0.01f)
            break;
    }

    float alpha = 1.0f - transmittance;
    return float4(color, alpha) * diffuseColor * input.vertexColor;
}