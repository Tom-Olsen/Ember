struct FragmentInput
{
    float4 vertexColor : COLOR;
};

float4 main(FragmentInput input) : SV_TARGET
{
    return input.vertexColor;
}