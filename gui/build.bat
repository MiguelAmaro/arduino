@echo off

if not exist build mkdir build

rem ====================    PROJECT/FILES      ====================
set PROJECT_NAME=acc_arduino
set SOURCES=^
F:\Dev\Embedded\Arduino\gui\src\%PROJECT_NAME%.cpp

rem ====================    COMPILER(MSVC)     ====================
set MSVC_WARNINGS= -wd4700

set MSVC_FLAGS= ^
%MSVC_WARNINGS% ^
-nologo ^
-Zi ^
-DRENDERER_OPENGL ^
-Gm- ^
-GR- ^
-EHa- ^
-Oi ^
-GS- ^
-Gs9999999

set MSVC_SEARCH_DIRS=

rem ====================     LINKER(MSVC)       ====================
set MSVC_LIBS= ^
user32.lib ^
gdi32.lib ^
dinput8.lib ^
dxguid.lib ^
opengl32.lib ^
kernel32.lib ^
ws2_32.lib ^
shell32.lib

set MSVC_LINK_FLAGS= ^
-nodefaultlib ^
-subsystem:windows ^
-stack:0x100000,0x100000


rem ************************************************************
rem **********              START BUILD               **********
rem ************************************************************
pushd build

set path=%path%;F:\Dev\Embedded\Arduino\gui\build

rem ====================     WINDOWS          ====================
call cl ^
%MSVC_FLAGS% ^
%MSVC_SEARCH_DIRS% ^
%SOURCES% ^
/link ^
%MSVC_LINK_FLAGS% ^
%MSVC_LIBS%

popd

pause
