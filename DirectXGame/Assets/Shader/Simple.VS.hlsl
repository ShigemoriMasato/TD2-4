struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    return output;
}
