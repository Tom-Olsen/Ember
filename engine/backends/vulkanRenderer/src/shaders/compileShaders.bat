@echo off
setlocal

if "%VULKAN_SDK%"=="" exit /b 1

set DXC=%VULKAN_SDK%\bin\dxc.exe

:: Compute shaders
for %%f in (*.comp.hlsl) do (
    "%DXC%" -spirv -T cs_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

:: Vertex shaders
for %%f in (*.vert.hlsl) do (
    "%DXC%" -spirv -T vs_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

:: Fragment shaders
for %%f in (*.frag.hlsl) do (
    "%DXC%" -spirv -T ps_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

endlocal