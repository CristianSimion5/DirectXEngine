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
    float3 tangent  : TANGENT;
    float3 bitangent: BINORMAL;
    float2 texCoord : UV;
};

struct PixelInputType
{
    float4 positionWorld : TEXCOORD1;
    float3 normalWorld   : TEXCOORD2;
    float2 texCoord      : TEXCOORD0;
    float3x3 tbn         : TEXCOORD3;
    float3x3 invTbn      : COLOR;
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

    float3 T = normalize(mul((float3x3) worldMatrix, input.tangent));
    float3 B = normalize(mul((float3x3) worldMatrix, input.bitangent));
    
    output.tbn = float3x3(T, B, output.normalWorld);
    output.invTbn = transpose(output.tbn);
    
    return output;
}