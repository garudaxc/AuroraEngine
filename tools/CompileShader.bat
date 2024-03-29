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

set outputDir=..\dev\CompiledShader

IF not exist %outputDir% (
rem	echo output dir %outputDir% not exist
	md %outputDir%
)

%glslang% ..\dev\data\shader\VulkanSample.vert -o %outputDir%\VulkanSample_vert.spv
%glslang% ..\dev\data\shader\VulkanSample.frag -o %outputDir%\VulkanSample_frag.spv

echo done!

pause
