rem This assumes that EffectTool has been built and exists in [NXNAFolder]\Debug
set et=..\..\..\Debug\EffectTool.exe

rem build the full shaders
%et% -h -hn AlphaTestEffect_bytecode AlphaTestEffect.nxfx AlphaTestEffect.inc
%et% -h -hn BasicEffect_bytecode BasicEffect.nxfx" Effects\BasicEffect.inc
%et% -h -hn DualTextureEffect_bytecodeDualTextureEffect.nxfx" DualTextureEffect.inc
%et% -h -hn SpriteEffect_bytecode SpriteEffect.nxfx" SpriteEffect.inc

rem build the shaders without HLSL (for pretty much every platform except Windows)
%et% -ip ANY_HLSL -h -hn AlphaTestEffect_bytecode AlphaTestEffect.nxfx AlphaTestEffect_nohlsl.inc
%et% -ip ANY_HLSL -h -hn BasicEffect_bytecode BasicEffect.nxfx BasicEffect_nohlsl.inc
%et% -ip ANY_HLSL -h -hn DualTextureEffect_bytecode DualTextureEffect.nxfx DualTextureEffect_nohlsl.inc
%et% -ip ANY_HLSL -h -hn SpriteEffect_bytecode SpriteEffect.nxfx SpriteEffect_nohlsl.inc