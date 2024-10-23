#ifndef __INCLUDE_GUARD_lightBuffer_hlsli__
#define __INCLUDE_GUARD_lightBuffer_hlsli__



// Maximum number of lights
static const uint MAX_DIRECTIONAL_LIGHTS = 2;   // sun, moon, etc.
static const uint MAX_SPOT_LIGHTS = 5;          // car headlights, etc.
static const uint MAX_POINT_LIGHTS = 10;        // candles, etc.



// Structure definitions for each light type
struct DirectionalLight
{
    float3 direction; // Direction of the light
    float3 color; // Color of the light
    float intensity; // Intensity of the light
};
struct SpotLight
{
    float3 position; // Position of the light
    float3 direction; // Direction of the light
    float3 color; // Color of the light
    float intensity; // Intensity of the light
    float innerCone; // Inner cone angle in radians (full intensity)
    float outerCone; // Outer cone angle in radians (fall-off)
};
struct PointLight
{
    float3 position;    // Position of the light
    float3 color;       // Color of the light
    float intensity;    // Intensity of the light
    float radius;       // Attenuation radius
};



// Constant buffer for light data
cbuffer LightData : register(b1)
{
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS]; // Array of directional lights
    uint numDirectionalLights; // Number of active directional lights

    SpotLight spotLights[MAX_SPOT_LIGHTS]; // Array of spot lights
    uint numSpotLights; // Number of active spot lights

    PointLight pointLights[MAX_POINT_LIGHTS]; // Array of point lights
    uint numPointLights; // Number of active point lights
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