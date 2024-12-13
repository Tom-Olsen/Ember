struct FragmentInput
{
    float4 clipPosition : SV_POSITION;  // position in clip space: x,y€[-1,1] z€[0,1]
};



float4 main(FragmentInput input) : SV_TARGET
{
    // return error pink:
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}