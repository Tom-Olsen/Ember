RWTexture2D<float4> image : register(u0);

cbuffer SurfaceProperties : register(b1)
{
    float4 diffuseColor; // (1.0, 1.0, 1.0)
    float roughness; // 0.5
    float3 reflectivity; // 0.4
    float metallicity; // 0 = dielectric, 1 = metal
    float4 scaleOffset; // .xy = scale, .zw = offset
};

SamplerState colorSampler : register(s10);
Texture2D colorMap : register(t21); // format = VK_FORMAT_R8G8B8A8_SRGB,

[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    float2 uv = threadID.xy / 100.0f;
    float4 color = diffuseColor * colorMap.Sample(colorSampler, uv);
    image[threadID.xy] = float4(1.0, 0.0, 1.0, 1.0) * diffuseColor * color;
}