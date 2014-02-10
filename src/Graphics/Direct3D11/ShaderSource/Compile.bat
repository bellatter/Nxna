@echo off
rem THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
rem ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
rem THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
rem PARTICULAR PURPOSE.
rem
rem Copyright (c) Microsoft Corporation. All rights reserved.

setlocal
set error=0

call :CompileShader SpriteEffect vs SpriteVertexShader
call :CompileShader SpriteEffect ps SpritePixelShader

call :CompileShader DualTextureEffect vs DualTextureVS
call :CompileShader DualTextureEffect vs DualTextureNoColorVS
call :CompileShader DualTextureEffect ps DualTexturePS
call :CompileShader DualTextureEffect ps DualTextureNoColorPS

call :CompileShader BasicEffect vs BasicEffectVS
call :CompileShader BasicEffect vs BasicEffectTextureVS
call :CompileShader BasicEffect vs BasicEffectColorVS
call :CompileShader BasicEffect vs BasicEffectColorTextureVS
call :CompileShader BasicEffect ps BasicEffectPS
call :CompileShader BasicEffect ps BasicEffectTexturePS
call :CompileShader BasicEffect ps BasicEffectColorPS
call :CompileShader BasicEffect ps BasicEffectColorTexturePS

call :CompileShader AlphaTestEffect vs VSAlphaTest
call :CompileShader AlphaTestEffect vs VSAlphaTestNoFog
call :CompileShader AlphaTestEffect vs VSAlphaTestVc
call :CompileShader AlphaTestEffect vs VSAlphaTestVcNoFog
call :CompileShader AlphaTestEffect ps PSAlphaTestLtGt
call :CompileShader AlphaTestEffect ps PSAlphaTestLtGtNoFog
call :CompileShader AlphaTestEffect ps PSAlphaTestEqNe
call :CompileShader AlphaTestEffect ps PSAlphaTestEqNeNoFog

echo.

if %error% == 0 (
   echo Shaders compiled ok
) else (
   echo There were shader compilation errors!
)

endlocal
exit /b

:CompileShader
set fxc=fxc /nologo %1.fx /T%2_4_0_level_9_1 /Zpc /Qstrip_reflect /Qstrip_debug /E%3 /FoCompiled\%1_%3.o /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b