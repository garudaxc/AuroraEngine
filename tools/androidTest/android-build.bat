rem 注意需要交叉编译环境（ninja环境）

@ set PATH=D:\Program Files\cmake-3.16.3-win64-x64\bin;%PATH%
@ set NDK=E:\develop\android\NVPACK\android-ndk-r15c
@ set ABI=armeabi-v7a

cmake . ^
-DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake ^
-DANDROID_ABI=%ABI% ^
-DANDROID_NATIVE_API_LEVEL=23 ^
-DANDROID_TOOLCHAIN=clang ^
-GNinja