cbuffer PerObject : register(b0)
{
    matrix worldMatrix;
    matrix inverseTransposeWorldMatrix;
    matrix worldViewProjectionMatrix;
}

struct VertexInputType
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 texCoord : UV;
};

struct PixelInputType
{
    float4 positionWorld : TEXCOORD1;
    float3 normalWorld   : TEXCOORD2;
    float2 texCoord      : TEXCOORD0;
    float4 position      : SV_POSITION;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    input.position.w = 1.0f;
  
    output.position = mul(worldViewProjectionMatrix, input.position);
    output.positionWorld = mul(worldMatrix, input.position);
    output.normalWorld = mul((float3x3) inverseTransposeWorldMatrix, input.normal);
    output.texCoord = input.texCoord;

    return output;
}