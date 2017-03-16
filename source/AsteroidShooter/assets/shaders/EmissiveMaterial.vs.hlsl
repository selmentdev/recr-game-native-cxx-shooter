//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

//
// Emmisive Material.
//
// Fake. Just outputs vertex position & texture coords to PS :)
//


//
// Camera data. Holds view & projection matrices.
//
cbuffer CameraData : register(b0)
{
    float4x4 CameraData_View;
    float4x4 CameraData_Projection;
};

//
// Object data. Precomputed for each rendered object before actual drawcall.
//
// Note:
//      It may also use instancing to render them in single drawcall, but that
//      kind of optimizations aren't part of that test :)
//
//      This is really a balance between more ops on GPU (matrix multiplication)
//      and memory usage and drawcalls.
//
//      Yea, I know: inverse of orthogonal matrix is a transposition of that
//      matrix in this cases :)
//
cbuffer ObjectData : register(b1)
{
    float4x4 ObjectData_World;
    float4x4 ObjectData_InverseWorld;
};

//
// Common vertex input.
//
struct VS_INPUT
{
    float3 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

//
// Common vertex outpu :)
//
struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    //
    // Expand position to homogenous space.
    //
    float4 position = float4(input.Position, 1.0F);
    
    //
    // Compute WV transform.
    //
    float4x4 worldView = mul(CameraData_View, ObjectData_World);

    //
    // And final WVP transform.
    //
    float4x4 worldViewProjection = mul(CameraData_Projection, worldView);

    //
    // And transformed vertex position.
    //
    position = mul(worldViewProjection, position);

    //
    // Move forward position and texcoord.
    //
    VS_OUTPUT output;
    output.Position = position;
    output.TexCoord = input.TexCoord;
    return output;
}