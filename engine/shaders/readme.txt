Shader rules:
-Shader system has only been tested with HLSL shaders. Use GLSL at your own risk.
-Currently 3 types of push constants are supported:
    defaultPushConstant: (for basic rendering)
		alignas(4) uint32_t instanceCount;
		alignas(4) float time;
		alignas(4) float deltaTime;
		alignas(4) int dirLightsCount;      (Ember::ToDo: moved to set1)
		alignas(4) int posLightsCount;      (Ember::ToDo: moved to set1)
		alignas(16) Float4 cameraPosition;  (Ember::ToDo: moved to set1)
    computePushConstant: (for compute shaders)
		alignas(4) Uint3 threadCount;
		alignas(4) float time;
		alignas(4) float deltaTime;
    shadowPushConstant: (for shadowMap shaders only)
		alignas(16) Float4x4 localToWorldMatrix;    (Ember::ToDo: move to set0?)
		alignas(16) Float4x4 worldToClipMatrix;     (Ember::ToDo: move to set0?)
-DescriptorSets are seperated by update frquency:
    set0/space0: (per object cbuffers)
        transform dependent: localToWorld, localToClip
    set1/space1: (per frame cbuffers)
        view, proj, viewProj, cameraPos
    set2/space2: (per material or shaderProperties)
        textures, buffers, shaderParams (metallicity etc.)
    set3/space3: (frequent engine level)
        dirLightCount, posLightsCount
    set4/space4: (infrequent engine level)
        samplers, global noise textures, etc

-Binding indexes of engine resources (set3+4/space3+4) must follow this structure:
    cbuffer LightProperties : register(b0)
    {
        int dirLightCount;
        int posLightCount;
    };
    SamplerState colorSampler : register(s100);
    SamplerComparisonState shadowSampler : register(s101);
    Texture2DArray<float> shadowMaps : register(t200);
    ...