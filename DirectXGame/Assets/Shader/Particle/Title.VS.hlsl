struct VSInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    uint instanceID : INSTANCE0;
};

cbuffer SizeBuffer : register(b0)
{
    float size;
}

cbuffer MatrixBuffer : register(b1)
{
    float4x4 vp;
    float4x4 billboard;
}

StructuredBuffer<float3> positions : register(t0);

VSOutput main(VSInput input, uint id : SV_InstanceID)
{
    VSOutput output;
    
    float4x4 scale = float4x4(
        size, 0, 0, 0,
        0, size, 0, 0,
        0, 0, size, 0,
        0, 0, 0, 1
    );
    float4x4 world = mul(scale, billboard);
    world[3].xyz += positions[id];
    float4x4 wvp = mul(world, vp);
    
    output.position = mul(input.position, wvp);
    output.instanceID = id;
	return output;
}