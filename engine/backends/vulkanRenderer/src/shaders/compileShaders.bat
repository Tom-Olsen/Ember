@echo off
setlocal

if "%VULKAN_SDK%"=="" exit /b 1

set DXC=%VULKAN_SDK%\bin\dxc.exe

for %%f in (*.comp.hlsl) do (
    "%DXC%" -spirv -T cs_6_0 -E main "%%f" -Fo "%%~nf.spv"
)

endlocal