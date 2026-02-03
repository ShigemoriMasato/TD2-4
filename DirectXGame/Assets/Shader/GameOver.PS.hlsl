struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float progress : PROGRESS0;
    float ratio : RATIO0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

Texture2D<float4> gTexture[] : register(t8);
SamplerState gSampler : register(s0);

cbuffer TextureIndexBuffer : register(b0)
{
    uint textureIndex;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    
    if (input.ratio > input.progress)
    {
        discard;
    }
    output.color = gTexture[textureIndex].Sample(gSampler, input.texCoord);
    output.color.a = 1.0f;
    if (output.color.a < 0.1f)
    {
        discard;
    }
    return output;
}
