#ifndef __INCLUDE_GUARD_lightBuffer_hlsli__
#define __INCLUDE_GUARD_lightBuffer_hlsli__



// Maximum number of lights
static const uint MAX_D_LIGHTS = 2;     // directional lights: sun, moon, etc.
static const uint MAX_S_LIGHTS = 5;     // spot lights: car headlights, etc.
static const uint MAX_P_LIGHTS = 10;    // point lights: candles, etc.



// Structure definitions for each light type
struct DLight
{
    float intensity;        // Intensity of the light
    float4 color;           // Color of the light
    float4x4 viewMatrix;    // Light world to local matrix
    float4x4 projMatrix;    // Light projection matrix
};
struct SLight
{
    float intensity;        // Intensity of the light
    float innerCone;        // Inner cone angle in radians (full intensity)
    float outerCone;        // Outer cone angle in radians (fall-off)
    float4 color;           // Color of the light
    float4x4 viewMatrix;    // Light world to local matrix
    float4x4 projMatrix;    // Light projection matrix
};
struct PLight
{
    float intensity;        // Intensity of the light
    float radius;           // Attenuation radius
    float4 color;           // Color of the light
    float4x4 viewMatrix;    // Light world to local matrix
    float4x4 projMatrix;    // Light projection matrix
};



// Constant buffer for light data
cbuffer LightData : register(b1)
{
    DLight dLights[MAX_D_LIGHTS];   // Array of directional lights
    uint dLightsCound;              // Number of active directional lights

    SLight sLights[MAX_S_LIGHTS];   // Array of spot lights
    uint sLightsCound;              // Number of active spot lights

    PLight pLights[MAX_P_LIGHTS];   // Array of point lights
    uint pLightsCound;              // Number of active point lights
};



void ProcessLights()
{
    for (uint i = 0; i < numDirectionalLights; ++i)
    {
        DirectionalLight light = directionalLights[i];
        // Use light.direction, light.color, etc.
    }

    for (uint i = 0; i < numSpotLights; ++i)
    {
        SpotLight light = spotLights[i];
        // Use light.position, light.direction, etc.
    }

    for (uint i = 0; i < numPointLights; ++i)
    {
        PointLight light = pointLights[i];
        // Use light.position, light.color, etc.
    }
}



#endif //__INCLUDE_GUARD_lightBuffer_hlsli__