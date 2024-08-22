@echo off
setlocal

REM Ensure Vulkan SDK path is set
set VULKAN_SDK_PATH=%VULKAN_SDK%
set DXC_PATH=%VULKAN_SDK_PATH%\Bin\dxc.exe

REM Compile shaders
"%DXC_PATH%" -spirv -T vs_6_0 -E main .\triangleVert.hlsl -Fo .\triangleVert.spv
"%DXC_PATH%" -spirv -T ps_6_0 -E main .\triangleFrag.hlsl -Fo .\triangleFrag.spv

endlocal