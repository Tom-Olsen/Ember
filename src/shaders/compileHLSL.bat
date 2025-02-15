@echo off
setlocal

REM Ensure Vulkan SDK path is set
set VULKAN_SDK_PATH=%VULKAN_SDK%
set DXC_PATH=%VULKAN_SDK_PATH%\Bin\dxc.exe

REM Compile vertex shaders (*.vert.hlsl)
for %%f in (*.vert.hlsl) do (
    echo Compiling vertex shader %%f
    "%DXC_PATH%" -spirv -T vs_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

REM Compile fragment shaders (*.frag.hlsl)
for %%f in (*.frag.hlsl) do (
    echo Compiling fragment shader %%f
    "%DXC_PATH%" -spirv -T ps_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

REM Compile compute shaders (*.comp.hlsl)
for %%f in (*.comp.hlsl) do (
    echo Compiling compute shader %%f
    "%DXC_PATH%" -spirv -T cs_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

endlocal