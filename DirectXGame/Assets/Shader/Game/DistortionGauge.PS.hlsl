struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint instanceID : INSTANCE0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer GlobalData : register(b0)
{
    int textureIndex;
    float time;
    float2 globalPadding;
};

struct InstanceData
{
    float4 color;
    float intensity;
    float width;
    float height;
    float padding;
};

StructuredBuffer<InstanceData> instanceData : register(t0);
Texture2D<float4> textures[] : register(t8);
SamplerState gSampler : register(s0);

float3 RGBtoHSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = (c.g < c.b) ? float4(c.bg, K.wz) : float4(c.gb, K.xy);
    float4 q = (c.r < p.x) ? float4(p.xyw, c.r) : float4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float e = 1e-10;

    return float3(
        abs(q.z + (q.w - q.y) / (6.0 * d + e)),
        d / (q.x + e),
        q.x
    );
}

float3 HSVtoRGB(float3 hsv)
{
    float3 rgb = clamp(abs(frac(hsv.x + float3(0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - 3.0) - 1.0, 0.0, 1.0);
    return hsv.z * lerp(float3(1.0, 1.0, 1.0), rgb, hsv.y);
}

PSOutput main(PSInput input)
{
    PSOutput output;

    InstanceData inst = instanceData[input.instanceID];

    float4 baseColor = inst.color;
    float intensity = inst.intensity;

    float2 uv = input.texCoord;
    float4 texColor = textures[textureIndex].Sample(gSampler, uv);

    float3 color = (baseColor * texColor).rgb;

    float3 hsv = RGBtoHSV(color);

    float baseHue = hsv.x;

    // 色相の最大変化幅
    float hueRange = 0.3;

    float offset = (intensity - 0.5) * hueRange;

    hsv.x = baseHue + offset;

    hsv.x = clamp(hsv.x, 0.0, 1.0);

    float3 shifted = HSVtoRGB(hsv);

    output.color = float4(shifted, texColor.a * baseColor.a);
    return output;
}
