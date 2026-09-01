#include "fragmentShaderCommon.hlsli"
#include "shadowMapping.hlsli"



cbuffer Values : register(b300, SHADER_SET)
{
    float targetDensity;
    float maxVelocity;
    int colorMode;
    float renderWidth;
};



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 surface_diffuseColor;    // (1.0, 1.0, 1.0)
    float4 surface_scaleOffset;     // .xy = scale, .zw = offset
    float surface_roughness;        // 0.5
    float surface_metallicity;      // 0 = dielectric, 1 = metal
    float surface_ambientOcclusion; // 1 = no occlusion
};



struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in [0,1]
    float4 vertexColor : COLOR;         // vertex color
    float4 uv : TEXCOORD0;              // texture coordinates
    float3 worldPosition : TEXCOORD1;   // position in world space
};



struct FragmentOutput
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};



FragmentOutput main(FragmentInput input)
{
    // Turn square into circle:
    float2 offset = 2.0f * (input.uv.xy - 0.5f);
    float offsetSquared = dot(offset, offset);
    if (offsetSquared > 1.0f)
        discard;

    // Reconstruct sphere surface:
    float normalDepth = sqrt(1.0f - offsetSquared);
    float3 worldNormal = normalize(
        Camera_GetRight() * offset.x
        + Camera_GetUp() * offset.y
        - Camera_GetForward() * normalDepth);
    float3 worldPosition = input.worldPosition - 0.5f * renderWidth * Camera_GetForward() * normalDepth;
    float4 clipPosition = mul(camera_worldToClipMatrix, float4(worldPosition, 1.0f));

    // Shading:
    float4 color = input.vertexColor * surface_diffuseColor;
    
    // Lighting:
    float3 finalColor = PhysicalLighting(worldPosition, worldNormal, color.xyz, surface_roughness, surface_metallicity, pc.receiveShadows != 0);
    
    FragmentOutput output;
    output.color = float4(finalColor, 1.0f);
    output.depth = clipPosition.z / clipPosition.w;
    return output;
}