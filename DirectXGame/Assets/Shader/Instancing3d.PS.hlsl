struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    uint textureHandle : TEXCOORD1;
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture[input.textureHandle].Sample(gSampler, input.texcoord);
    output.color = textureColor * input.color;
    return output;
}