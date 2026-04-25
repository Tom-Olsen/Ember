Shader rules:
-Shader system has only been tested with HLSL shaders. Use GLSL at your own risk.
-Currently 3 types of push constants are supported:
    defaultPushConstant: (for basic rendering)
		alignas(4) uint32_t instanceCount;
		alignas(4) float time;
		alignas(4) float deltaTime;
    computePushConstant: (for compute shaders)
		alignas(4) Uint3 threadCount;
		alignas(4) float time;
		alignas(4) float deltaTime;
    shadowPushConstant: (for shadowMap shaders only)
		alignas(16) Float4x4 localToWorldMatrix;
		alignas(16) Float4x4 worldToClipMatrix;

-DescriptorSets are seperated by update frquency (use the macros from descriptorSetMacros.h):
    set0/space0: (GLOBAL_SET)   e.g global:     samplers, shadowMaps, global noise textures, etc
    set1/space1: (SCENE_SET)    e.g lighting:   dirLightCount, posLightsCount
    set2/space2: (FRAME_SET)    e.g camera:     view, proj, viewProj, cameraPos
    set3/space3: (SHADER_SET)   e.g shader:     textures, buffers, shared shader params
    set4/space4: (CALL_SET)     e.g call:       draw/dispatch-local parameters

-Some bindings are reserved for engine level resources or commonly used features (see vertex/fragmentShaderCommon.hlsli):

    cbuffer ModelMatrizes : register(b399, CALL_SET)
    {
        float4x4 cb_localToWorldMatrix;
        float4x4 cb_worldToLocalMatrix;
    };
    cbuffer Camera : register(b1399, FRAME_SET)
    {
        float4 cb_cameraPosition;
        float4x4 cb_viewMatrix;
        float4x4 cb_projMatrix;
    }
    struct InstanceData
    {
        float4x4 localToWorldMatrix;
        float4 color;
    };
    StructuredBuffer<InstanceData> b_instanceData : register(t199, SHADER_SET);
    cbuffer LightProperties : register(b2399, SCENE_SET)
    {
        int cb_dirLightCount;
        int cb_posLightCount;
    };

    SamplerState colorSampler : register(s3099, GLOBAL_SET);
    SamplerComparisonState shadowSampler : register(s3098, GLOBAL_SET);
    Texture2DArray<float> shadowMaps : register(t3199, GLOBAL_SET);
    ...