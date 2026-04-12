struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    nointerpolation uint textureIndex : TEXCOORD1;
    float4 color : COLOR0;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

cbuffer Color : register(b0)
{
    float4 gColor;
};

Texture2D<float4> gTextures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput o;

    float4 tex = gTextures[input.textureIndex].Sample(gSampler, input.texcoord);
    o.color = tex * gColor * input.color;

    if (o.color.a < 0.02f)
    {
        discard;
    }

    return o;
}