#ifndef __INCLUDE_GUARD_vertexShaderCommon_hlsli__
#define __INCLUDE_GUARD_vertexShaderCommon_hlsli__
#include "descriptorSetMacros.h"
#include "defaultPushConstant.hlsli"
#include "math.hlsli"
#include "frameSet.hlsli"



// Including this into your shader has no negative side effects if you dont't use the resources defined here.
// The DXC hlsl compiler will optimize them out of your binary shader code.
// However, by using these pre defined resource you automatically follow the best practices in the readme.txt,
// which allows for ideal descriptorSet reusage across shader stages, which will increase performance.



// Per draw call resources:
cbuffer ModelMatrizes : register(b399, CALL_SET)
{
    float4x4 model_localToWorldMatrix; // TRS matrix.
    float4x4 model_worldToLocalMatrix; // inverse TRS matrix.
};



// Per material/shaderProperites resources:
struct InstanceData
{
    float4x4 localToWorldMatrix;    // per instance TRS matrix (relative to group transform).
    float4 color;                   // per instance color.
};
StructuredBuffer<InstanceData> instanceBuffer : register(t199, SHADER_SET);



// Model world position:
float3 Model_GetWorldPosition()
{
    return LinAlg_GetTranslation(model_localToWorldMatrix);
}
float3 Model_GetWorldPosition(uint instanceID)
{
    if (pc.instanceCount != 0 && instanceID < pc.instanceCount)
        return LinAlg_GetTranslation(mul(model_localToWorldMatrix, instanceBuffer[instanceID].localToWorldMatrix));
    return Model_GetWorldPosition();
}

// Model local to world matrix:
float4x4 Model_GetLocalToWorldMatrix()
{
    return model_localToWorldMatrix;
}
float4x4 Model_GetLocalToWorldMatrix(uint instanceID)
{
    if (pc.instanceCount != 0 && instanceID < pc.instanceCount)
        return mul(model_localToWorldMatrix, instanceBuffer[instanceID].localToWorldMatrix);
    return model_localToWorldMatrix;
}

// Model local to clip matrix:
float4x4 Model_GetLocalToClipMatrix()
{
    return mul(camera_worldToClipMatrix, model_localToWorldMatrix);
}
float4x4 Model_GetLocalToClipMatrix(uint instanceID)
{
    return mul(camera_worldToClipMatrix, Model_GetLocalToWorldMatrix(instanceID));
}
float4x4 Model_GetLocalToClipMatrix(uint instanceID, float4x4 localToWorldMatrix)
{
    return mul(camera_worldToClipMatrix, localToWorldMatrix);
}



#endif // __INCLUDE_GUARD_vertexShaderCommon_hlsli__