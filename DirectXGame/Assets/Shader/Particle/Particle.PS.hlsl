struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

cbuffer Color : register(b0)
{
    float4 gColor;
};

cbuffer TextureIndex : register(b1)
{
    int gTextureIndex;
};

Texture2D<float4> gTextures[] : register(t8); // Engine側のテクスチャ配列規約に合わせる
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput o;

    float4 tex = gTextures[gTextureIndex].Sample(gSampler, input.texcoord);
    o.color = tex * gColor;

    if (o.color.a < 0.02f)
    {
        discard;
    }

    return o;
}