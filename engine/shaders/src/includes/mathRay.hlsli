#ifndef __INCLUDE_GUARD_mathRay_hlsli__
#define __INCLUDE_GUARD_mathRay_hlsli__



// Structs:
struct math_WorldRay
{
	float3 origin;
	float3 direction;
};
struct math_ScreenRay
{
    float3 origin;     // xy = pixels, z = NDC depth [0,1]
    float3 direction;  // xy = pixel delta, z = NDC depth delta
};




math_ScreenRay math_ProjectRayToScreen(math_WorldRay worldRay, float2 screenSize, float4x4 worldToClipMatrix)
{
    // Second point on the same world-space line:
    float3 rayPoint = worldRay.origin + worldRay.direction;

    // World -> Clip xy[-w, w], z[0,w]:
    float4 originClip = mul(worldToClipMatrix, float4(worldRay.origin, 1.0f));
    float4 pointClip  = mul(worldToClipMatrix, float4(rayPoint,  1.0f));

    // Clip -> NDC xy[-1, 1], z[0,1]:
    float3 originNdc = originClip.xyz / originClip.w;
    float3 pointNdc  = pointClip.xyz  / pointClip.w;

    // NDC -> pixels [0, width/height]:
    float3 originScreen;
    originScreen.xy = (originNdc.xy * 0.5f + 0.5f) * screenSize;
    originScreen.z  = originNdc.z;
    float3 pointScreen;
    pointScreen.xy = (pointNdc.xy * 0.5f + 0.5f) * screenSize;
    pointScreen.z  = pointNdc.z;

    math_ScreenRay screenRay;
    screenRay.origin = originScreen;
    screenRay.direction = pointScreen - originScreen;
    return screenRay;
}



#endif // __INCLUDE_GUARD_mathRay_hlsli__