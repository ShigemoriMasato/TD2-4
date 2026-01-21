@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem ============================================================
rem ========== 作業ディレクトリ ================================
rem ============================================================
cd /d DirectXGame\externals || exit /b 1

rem ============================================================
rem ========== 共通ルート =====================================
rem ============================================================
set SRC_ROOT=src
set BUILD_ROOT=build
set INSTALL_ROOT=install
set INCLUDE_ROOT=include
set LIB_ROOT=lib
set GENERATED=generated

rem ============================================================
rem ========== ルートディレクトリ作成 =========================
rem ============================================================
if not exist "%GENERATED%\%BUILD_ROOT%"   mkdir "%GENERATED%\%BUILD_ROOT%"
if not exist "%GENERATED%\%INSTALL_ROOT%" mkdir "%GENERATED%\%INSTALL_ROOT%"
if not exist "%GENERATED%\%INCLUDE_ROOT%" mkdir "%GENERATED%\%INCLUDE_ROOT%"
if not exist "%GENERATED%\%LIB_ROOT%"     mkdir "%GENERATED%\%LIB_ROOT%"
if not exist "%GENERATED%\%LIB_ROOT%\Release" mkdir "%GENERATED%\%LIB_ROOT%\Release"
if not exist "%GENERATED%\%LIB_ROOT%\Debug"   mkdir "%GENERATED%\%LIB_ROOT%\Debug"

echo.
echo ===== Build started =====

rem ============================================================
rem ========== assimp =========================================
rem ============================================================
set CMAKE_EXTRA_OPTS=-DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_SAMPLES=OFF
call :build_lib assimp
set CMAKE_EXTRA_OPTS=

rem ============================================================
rem ========== DirectXTex ====================================
rem ============================================================
set CMAKE_EXTRA_OPTS=-DBUILD_TEXCONV=OFF -DBUILD_DDSVIEW=OFF
call :build_lib DirectXTex
set CMAKE_EXTRA_OPTS=

rem ============================================================
rem ========== spdlog ========================================
rem ============================================================
call :build_lib spdlog

rem ============================================================
rem ========== imgui =========================================
rem ============================================================
call :build_lib imgui

rem ============================================================
rem ========== freetype ======================================
rem ============================================================
call :build_lib freetype

xcopy /E /I /Y "%GENERATED%\%LIB_ROOT%\Release" "%GENERATED%\%LIB_ROOT%\Development"
cd ..
powershell.exe -NoProfile -ExecutionPolicy Bypass ^
    -File "Scripts\FilterAdjust.ps1"

echo.
echo ===== All libraries build & install completed =====
exit /b 0


rem ============================================================
rem ========== ライブラリ単位処理 =============================
rem ============================================================
:build_lib
set LIBNAME=%1

echo.
echo ============================================================
echo  Building %LIBNAME%
echo ============================================================

call :build_one %LIBNAME% Release MultiThreaded
if errorlevel 1 exit /b 1

call :build_one %LIBNAME% Debug MultiThreadedDebug
if errorlevel 1 exit /b 1

call :file_copy %LIBNAME%
if errorlevel 1 exit /b 1

exit /b 0

rem ============================================================
rem ========== Debug / Release 個別ビルド =====================
rem ============================================================
:build_one
set LIBNAME=%1
set CONFIG=%2
set CRT=%3

set SRC_DIR=%SRC_ROOT%\%LIBNAME%
set BUILD_DIR=%GENERATED%\%BUILD_ROOT%\%LIBNAME%\%CONFIG%
set INSTALL_DIR=%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\%CONFIG%

echo.
echo ---- %LIBNAME% [%CONFIG] ----

if not exist "%BUILD_DIR%"   mkdir "%BUILD_DIR%"
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

rem ============================================================
rem ========== CMake Configure ================================
rem ============================================================
cmake -S "%SRC_DIR%" -B "%BUILD_DIR%" ^
    -G "Visual Studio 17 2022" ^
    -A x64 ^
    -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
    -DCMAKE_MSVC_RUNTIME_LIBRARY=%CRT% ^
    -DBUILD_SHARED_LIBS=OFF ^
    %CMAKE_EXTRA_OPTS%

if errorlevel 1 (
    echo [ERROR] Configure failed: %LIBNAME% %CONFIG%
    exit /b 1
)

rem ============================================================
rem ========== Build & Install ================================
rem ============================================================
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 exit /b 1

cmake --install "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 exit /b 1

:file_copy
set LIBNAME=%1
rem ------------------------------------------------------------
rem include コピー（Release 側から 1 回）
rem ------------------------------------------------------------
if not exist "%GENERATED%\%INCLUDE_ROOT%\%LIBNAME%" mkdir "%GENERATED%\%INCLUDE_ROOT%\%LIBNAME%"
echo testText
if exist "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Release\include" (
    xcopy /E /I /Y "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Release\include" "%GENERATED%\%INCLUDE_ROOT%" >nul
)

rem ------------------------------------------------------------
rem lib コピー（Debug / Release 分離）
rem ------------------------------------------------------------
if exist "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Release\lib" (
    echo [LIB] %LIBNAME% Release
    dir "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Release\lib"
    xcopy /Y /F ^
        "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Release\lib\*.lib" ^
        "%GENERATED%\%LIB_ROOT%\Release\"
)

if exist "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Debug\lib" (
    echo [LIB] %LIBNAME% Debug
    dir "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Debug\lib"
    xcopy /Y /F ^
        "%GENERATED%\%INSTALL_ROOT%\%LIBNAME%\Debug\lib\*.lib" ^
        "%GENERATED%\%LIB_ROOT%\Debug\"
)

exit /b 0
