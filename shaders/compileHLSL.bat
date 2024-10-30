@echo off
setlocal

REM Ensure Vulkan SDK path is set
set VULKAN_SDK_PATH=%VULKAN_SDK%
set DXC_PATH=%VULKAN_SDK_PATH%\Bin\dxc.exe

REM Compile shaders
"%DXC_PATH%" -spirv -T vs_6_0 -E main .\vertDefault.hlsl -Fo .\vertDefault.spv
"%DXC_PATH%" -spirv -T ps_6_0 -E main .\fragDefault.hlsl -Fo .\fragDefault.spv

"%DXC_PATH%" -spirv -T vs_6_0 -E main .\vertColor.hlsl -Fo .\vertColor.spv
"%DXC_PATH%" -spirv -T ps_6_0 -E main .\fragColor.hlsl -Fo .\fragColor.spv

"%DXC_PATH%" -spirv -T vs_6_0 -E main .\vertShadow.hlsl -Fo .\vertShadow.spv

"%DXC_PATH%" -spirv -T vs_6_0 -E main .\vertShadowMap.hlsl -Fo .\vertShadowMap.spv
"%DXC_PATH%" -spirv -T ps_6_0 -E main .\fragShadowMap.hlsl -Fo .\fragShadowMap.spv

endlocal