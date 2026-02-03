struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float progress : PROGRESS0;
    float ratio : RATIO0;
};

struct VSData
{
    float4x4 world;
    float4x4 vp;
    float4x4 uv;
    float3 scale;
    float progress;
};
StructuredBuffer<VSData> vsData : register(t0);

VSOutput main(VSInput input, uint id : SV_InstanceID)
{
    VSOutput output;
    VSData data = vsData[id];
    
    float3 localPos = input.position.xyz * data.scale;
    const float textureX = 1.0f;
    
    output.position = mul(float4(localPos, 1.0f), mul(data.world, data.vp));
    output.texCoord.x = localPos.x / 200.0f;
    //output.texCoord.x = input.texCoord.x;
    output.texCoord.y = input.texCoord.y;
    output.texCoord = mul(float4(output.texCoord, 0.0f, 1.0f), data.uv).xy;
    output.normal = input.normal;
    output.progress = data.progress;
    const float modelLength = 30.0f;
    output.ratio = (input.position.x + modelLength / 2) / modelLength;
	return output;
}
