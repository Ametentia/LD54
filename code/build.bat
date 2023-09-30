@echo off
setlocal

if not exist "..\build" (mkdir "..\build")

pushd "..\build" > nul

del *.pdb > nul 2> nul

if "%1" equ "release" (
    set CL=-O2 -DNDEBUG -GS-
    set LINK=-OPT:REF -OPT:ICF
) else (
    set CL=-MDd -Od -Zi -D_DEBUG -RTC1
    set LINK=-DEBUG
)

cl -nologo -W3 -WX -DUNICODE -I"include" -FC "..\code\ld_main.c" -Fe"run.exe" -link -incremental:no -subsystem:windows -libpath:. xid.lib
cl -nologo -W3 -WX -DUNICODE -I"include" -FC -LD "..\code\ld_game.c" -Fe"game.dll" -Fd"game_%random%.pdb" -link -incremental:no -subsystem:windows -libpath:. xid.lib

del *.obj *.res > nul 2> nul

popd > nul

endlocal
