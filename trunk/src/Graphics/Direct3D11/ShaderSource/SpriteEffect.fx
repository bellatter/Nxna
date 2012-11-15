// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://create.msdn.com/en-US/education/catalog/sample/stock_effects


//Texture2D<float4> Diffuse : register(t0);
//sampler DiffuseSampler : register(s0);


cbuffer Parameters : register(b0)
{
   row_major float4x4 ModelViewProjection;
};


void SpriteVertexShader(inout float4 color    : COLOR0,
                       inout float2 texCoord : TEXCOORD0,
                       inout float4 position : SV_Position)
{
   position = mul(position, ModelViewProjection);
}


float4 SpritePixelShader(float4 color    : COLOR0,
                        float2 texCoord : TEXCOORD0) : SV_Target0
{
	return color;
   //return Diffuse.Sample(DiffuseSampler, texCoord) * color + float4(1.0, 0, 0, 1.0);
}