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

-DescriptorSets are seperated by update frquency (use the macros from descriptorSpaces.hlsli):
    set0/space0: (DRAW_SET)     e.g transform:  localToWorld, worldToLocal
    set1/space1: (FRAME_SET)    e.g camera:     view, proj, viewProj, cameraPos
    set2/space2: (MATERIAL_SET) e.g material:   textures, buffers, instanceData, shaderParams (metallicity etc.)
    set3/space3: (SCENE_SET)    e.g lighting:   dirLightCount, posLightsCount
    set4/space4: (ENGINE_SET)   e.g engine:     samplers, global noise textures, etc

-Some bindings are reserved for engine level resources or commonly used features (see vertex/fragmentShaderCommon.hlsli):

    cbuffer ModelMatrizes : register(b100, DRAW_SET)
    {
        float4x4 cb_localToWorldMatrix;
        float4x4 cb_worldToLocalMatrix;
    };
    cbuffer Camera : register(b100, FRAME_SET)
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
    StructuredBuffer<InstanceData> b_instanceData : register(t100, MATERIAL_SET);
    cbuffer LightProperties : register(b100, SCENE_SET)
    {
        int cb_dirLightCount;
        int cb_posLightCount;
    };

    SamplerState colorSampler : register(s100, ENGINE_SET);
    SamplerComparisonState shadowSampler : register(s101, ENGINE_SET);
    Texture2DArray<float> shadowMaps : register(t200, ENGINE_SET);
    ...