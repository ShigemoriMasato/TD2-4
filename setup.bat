@echo off
setlocal

cd DirectXGame\externals
robocopy ^ src\spdlog\ ^ install\include\ ^ /E /XC /XN /XO

REM ==============================
REM 各種パス指定（ここだけ触ればOK）
REM ==============================

REM インクルードパス（.h が配置される場所）
set INCLUDE_PATH=%INSTALL_ROOT%\include

REM リブパス（.lib が配置される場所）
set LIB_PATH=%INSTALL_ROOT%\lib

REM ==============================
REM Assimp専用
REM ==============================

REM assimp のソースコード場所
set ASSIMP_SOURCE=source\assimp

REM cmake の出力先（build ディレクトリ）
set ASSIMP_BUILD=build\assimp

REM インストール先（生成物）
set INSTALL_ROOT=install

REM ==============================
REM CMake 生成
REM ==============================

echo === Generate assimp project ===
cmake -S %ASSIMP_SOURCE% ^
      -B %ASSIMP_BUILD% ^
      -DCMAKE_INSTALL_PREFIX=%INSTALL_ROOT% ^
      -DASSIMP_BUILD_TESTS=OFF ^
      -DASSIMP_BUILD_ASSIMP_TOOLS=OFF ^
      -DASSIMP_BUILD_SAMPLES=OFF ^
      -DASSIMP_NO_EXPORT=ON

if errorlevel 1 goto ERROR

REM ==============================
REM Build (Release)
REM ==============================

echo === Build assimp (Release) ===
cmake --build %ASSIMP_BUILD% --config Release
if errorlevel 1 goto ERROR

REM ==============================
REM Install (Release)
REM ==============================

echo === Install assimp (Release) ===
cmake --install %ASSIMP_BUILD% --config Release
if errorlevel 1 goto ERROR

REM ==============================
REM 結果表示
REM ==============================

echo.
echo === assimp build completed ===
echo INCLUDE_PATH : %インクルードパス%
echo LIB_PATH         : %LIB_PATH%
echo.

exit /b 0

:ERROR
echo.
echo !!! assimp build failed !!!
exit /b 1


