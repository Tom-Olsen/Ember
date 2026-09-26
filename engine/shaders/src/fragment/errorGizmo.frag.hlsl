#include "fragmentShaderCommon.hlsli"



struct FragmentInput
{
	float4 vertexColor : COLOR;		// vertex color
};



float4 main(FragmentInput input) : SV_TARGET
{
	return input.vertexColor;
}