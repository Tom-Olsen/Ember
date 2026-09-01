#include "fragmentShaderCommon.hlsli"



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
	float4 surface_diffuseColor;	// (1.0, 1.0, 1.0)
	int surface_isLit;				// true
};



struct FragmentInput
{
	float4 clipPosition : SV_POSITION;  // position in clip space: x,y in [-1,1] z in[0,1]
	float3 localNormal : NORMAL;        // normal in local/model space
	float4 vertexColor : COLOR;         // vertex color
};



float4 main(FragmentInput input) : SV_TARGET
{
	// Directional lighting in model space:
	if (surface_isLit)
	{
		float3 normal = normalize(input.localNormal);
		float3 lightDirection = normalize(float3(0.4f, 0.5f, 0.6f));
		float diffuseLight = saturate(dot(normal, lightDirection));
		float ambientLight = 0.35f;
		float light = ambientLight + (1.0f - ambientLight) * diffuseLight;

		float4 color = input.vertexColor * surface_diffuseColor;
		return float4(color.rgb * light, color.a);
	}
	else
		return input.vertexColor * surface_diffuseColor;
}