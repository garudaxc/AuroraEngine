@echo off

IF not DEFINED VULKAN_SDK (

	echo vulkan sdk enviroment not defined
	pause
	exit 0
) else (
	echo vulkan sdk path %VULKAN_SDK%
)


set glslang=%VULKAN_SDK%\bin\glslc.exe

rem 切换当前路径
rem echo %cd%
rem echo %~dp0
%~d0
cd %~dp0

%glslang% ..\dev\data\shader\VulkanSample.vert -o ..\dev\CompiledShader\VulkanSample_vert.spv
%glslang% ..\dev\data\shader\VulkanSample.frag -o ..\dev\CompiledShader\VulkanSample_frag.spv

echo done!

pause
