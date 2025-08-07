@echo off
REM Build script for Marine Navigation Shortest Path Challenge on Windows

echo Marine Navigation Shortest Path Challenge - Windows Build Script
echo ================================================================

REM Check if Visual Studio is available
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Visual Studio C++ compiler not found in PATH.
    echo Please run this script from a Visual Studio Developer Command Prompt.
    echo Or install Visual Studio 2019/2022 with C++ support.
    pause
    exit /b 1
)

REM Check if CMake is available
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo CMake not found in PATH.
    echo Please install CMake and add it to your PATH.
    pause
    exit /b 1
)

echo Dependencies check:
echo - Visual Studio C++ compiler: Found
echo - CMake: Found

REM Check for vcpkg
if defined VCPKG_ROOT (
    echo - vcpkg root: %VCPKG_ROOT%
    set CMAKE_TOOLCHAIN=-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
) else (
    echo - vcpkg: Not configured (optional)
    echo   To use vcpkg, set VCPKG_ROOT environment variable
    set CMAKE_TOOLCHAIN=
)

echo.
echo Installing dependencies with vcpkg (if available)...
if defined VCPKG_ROOT (
    echo Installing geos and nlohmann-json...
    %VCPKG_ROOT%\vcpkg.exe install geos:x64-windows nlohmann-json:x64-windows
    if %ERRORLEVEL% NEQ 0 (
        echo Warning: vcpkg installation failed. You may need to install dependencies manually.
    )
) else (
    echo Skipping vcpkg installation. Please ensure dependencies are available:
    echo   - GEOS library
    echo   - nlohmann/json library
)

echo.
echo Creating build directory...
if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake .. %CMAKE_TOOLCHAIN% -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable location: build\Release\shortest_path.exe
echo.
echo To test with the example data:
echo   cd build\Release
echo   shortest_path.exe ..\..\data\example_input.json
echo.
pause