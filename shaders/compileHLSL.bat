@echo off
setlocal

REM Ensure Vulkan SDK path is set
set VULKAN_SDK_PATH=%VULKAN_SDK%
set DXC_PATH=%VULKAN_SDK_PATH%\Bin\dxc.exe

REM Compile shaders
"%DXC_PATH%" -spirv -T vs_6_0 -E main .\vert.hlsl -Fo .\vert.spv
"%DXC_PATH%" -spirv -T ps_6_0 -E main .\frag.hlsl -Fo .\frag.spv

endlocal