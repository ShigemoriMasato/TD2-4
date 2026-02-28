StructuredBuffer<float4> colors : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
    uint instanceID : SV_InstanceID;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = colors[input.instanceID];
    return output;
}