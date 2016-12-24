@echo off
set xxdcmd="c:\program files (x86)\vim\vim74\xxd"

call:compile BasicEffect.vert
call:compile BasicEffect.frag
call:compile SpriteEffect.vert
call:compile SpriteEffect.frag
call:compile AlphaTestEffect.vert
call:compile AlphaTestEffect.frag
call:compile DualTextureEffect.vert
call:compile DualTextureEffect.frag
goto:eof

:compile
set str=%~1
set str=%str:.=_%
echo.const char %str%[]={>%~1.inc
%xxdcmd% -i < %~1 >> %~1.inc
echo., 0}; >> %~1.inc
goto:eof