@echo off
set build_type=Release
if "%1"=="" echo Please specific the SDL2 directory && exit /b
set SDL2DIR=%1
if "%2"=="debug" set build_type=Debug
echo Building chipbox for windows, type %build_type%

rd /s /q build\
md build && cd build
cmake ..\src
cmake --build . --config %build_type%

cd ..
rd /s /q dist\
md dist\bin
copy build\bin\%build_type%\chipbox_sdl.exe dist\bin\
copy build\bin\%build_type%\testcore.exe dist\bin\
copy build\bin\%build_type%\chipbox_das.exe dist\bin\
copy build\bin\%build_type%\testdas.exe dist\bin\
copy %SDL2DIR%\lib\x86\SDL2.dll dist\bin\
for %%I in (README.md LICENSE TODO.md) do copy %%I dist\
