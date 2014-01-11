//-----------------------------------------------------------------------------
// AlphaTestEffect.fx
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Texture2D<float4> Diffuse : register(t0);
sampler DiffuseSampler : register(s0);

cbuffer Parameters : register(b0)
{
	row_major float4x4 WorldViewProj;
	float4 DiffuseColor;
    float4 AlphaTest;
    float3 FogColor;
    float4 FogVector;
};


#include "Structures.fxh"
#include "Common.fxh"


// Vertex shader: basic.
VSOutputTx VSAlphaTest(VSInputTx vin)
{
    VSOutputTx vout;
    
    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;
    
    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: no fog.
VSOutputTxNoFog VSAlphaTestNoFog(VSInputTx vin)
{
    VSOutputTxNoFog vout;
    
    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParamsNoFog;
    
    vout.TexCoord = vin.TexCoord;

    return vout;
}


// Vertex shader: vertex color.
VSOutputTx VSAlphaTestVc(VSInputTxVc vin)
{
    VSOutputTx vout;
    
    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParams;
    
    vout.TexCoord = vin.TexCoord;
    vout.Diffuse *= vin.Color;
    
    return vout;
}


// Vertex shader: vertex color, no fog.
VSOutputTxNoFog VSAlphaTestVcNoFog(VSInputTxVc vin)
{
    VSOutputTxNoFog vout;
    
    CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
    SetCommonVSOutputParamsNoFog;
    
    vout.TexCoord = vin.TexCoord;
    vout.Diffuse *= vin.Color;
    
    return vout;
}

#define SAMPLE_TEXTURE(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)

// Pixel shader: less/greater compare function.
float4 PSAlphaTestLtGt(VSOutputTx pin) : SV_Target0
{
    float4 color = SAMPLE_TEXTURE(Diffuse, pin.TexCoord) * pin.Diffuse;

    clip((color.a < AlphaTest.x) ? AlphaTest.z : AlphaTest.w);

    ApplyFog(color, pin.Specular.w);

    return color;
}


// Pixel shader: less/greater compare function, no fog.
float4 PSAlphaTestLtGtNoFog(VSOutputTxNoFog pin) : SV_Target0
{
    float4 color = SAMPLE_TEXTURE(Diffuse, pin.TexCoord) * pin.Diffuse;
    
    clip((color.a < AlphaTest.x) ? AlphaTest.z : AlphaTest.w);

    return color;
}


// Pixel shader: equal/notequal compare function.
float4 PSAlphaTestEqNe(VSOutputTx pin) : SV_Target0
{
    float4 color = SAMPLE_TEXTURE(Diffuse, pin.TexCoord) * pin.Diffuse;
    
    clip((abs(color.a - AlphaTest.x) < AlphaTest.y) ? AlphaTest.z : AlphaTest.w);

    ApplyFog(color, pin.Specular.w);
    
    return color;
}


// Pixel shader: equal/notequal compare function, no fog.
float4 PSAlphaTestEqNeNoFog(VSOutputTxNoFog pin) : SV_Target0
{
    float4 color = SAMPLE_TEXTURE(Diffuse, pin.TexCoord) * pin.Diffuse;
    
    clip((abs(color.a - AlphaTest.x) < AlphaTest.y) ? AlphaTest.z : AlphaTest.w);

    return color;
}


// NOTE: The order of the techniques here are
// defined to match the indexing in AlphaTestEffect.cs.

#define TECHNIQUE(name, vsname, psname ) \
technique name { pass { VertexShader = compile vs_4_0_level_9_1 vsname (); PixelShader = compile ps_4_0_level_9_1 psname(); } }


TECHNIQUE( AlphaTestEffect_LTGT, VSAlphaTest, PSAlphaTestLtGt );
TECHNIQUE( AlphaTestEffect_LTGT_NoFog, VSAlphaTestNoFog, PSAlphaTestLtGtNoFog );
TECHNIQUE( AlphaTestEffect_LTGT_VertexColor, VSAlphaTestVc, PSAlphaTestLtGt );
TECHNIQUE( AlphaTestEffect_LTGT_VertexColor_NoFog, VSAlphaTestVcNoFog, PSAlphaTestLtGtNoFog );

TECHNIQUE( AlphaTestEffect_EQNE, VSAlphaTest, PSAlphaTestEqNe );
TECHNIQUE( AlphaTestEffect_EQNE_NoFog, VSAlphaTestNoFog, PSAlphaTestEqNeNoFog );
TECHNIQUE( AlphaTestEffect_EQNE_VertexColor, VSAlphaTestVc, PSAlphaTestEqNe );
TECHNIQUE( AlphaTestEffect_EQNE_VertexColor_NoFog, VSAlphaTestVcNoFog, PSAlphaTestEqNeNoFog );