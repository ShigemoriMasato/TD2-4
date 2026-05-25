struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    nointerpolation uint textureIndex : TEXCOORD1;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

Texture2D<float4> textures[] : register(t8);
SamplerState gSampler : register(s0);

PSOutput main(PSInput input)
{
    PSOutput o;

    // テクスチャ色
    float4 tex = textures[input.textureIndex].Sample(gSampler, input.texcoord);

	// テクスチャ色 * トレイル全体の色 * 頂点固有の色
    float4 c = tex * input.color;

    // alphaが極小なら捨てる
    clip(c.a - 0.001f);

    o.color = c;
    return o;
}