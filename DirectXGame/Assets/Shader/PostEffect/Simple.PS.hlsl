#include "PostEffect.hlsli"

Texture2D<float4> gTexture[1024] : register(t8);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.color = gTexture[0].Sample(gSampler, input.texcoord);
    return output;
}
