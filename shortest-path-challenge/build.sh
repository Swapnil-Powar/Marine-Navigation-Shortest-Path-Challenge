#!/bin/bash

# Build script for Marine Navigation Shortest Path Challenge
# This script handles dependency installation and compilation

set -e

echo "Marine Navigation Shortest Path Challenge - Build Script"
echo "========================================================"

# Check if we're on a supported system
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    SYSTEM="windows"
else
    echo "Unsupported operating system: $OSTYPE"
    exit 1
fi

echo "Detected system: $SYSTEM"

# Function to install dependencies on Windows
install_deps_windows() {
    echo "Installing dependencies on Windows..."
    echo "Please ensure you have the following installed:"
    echo "  - Visual Studio 2019 or later with C++ support"
    echo "  - CMake"
    echo "  - vcpkg package manager"
    echo ""
    echo "Then run:"
    echo "  vcpkg install geos nlohmann-json"
    echo ""
    echo "Press Enter to continue if dependencies are installed..."
    read
}

# Install dependencies based on system
case $SYSTEM in
    windows)
        install_deps_windows
        ;;
esac

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
if [[ "$SYSTEM" == "windows" ]]; then
    # On Windows, might need to specify vcpkg toolchain
    if [[ -n "$VCPKG_ROOT" ]]; then
        cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
    else
        cmake ..
    fi
else
    cmake ..
fi

# Build
echo "Building..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build completed successfully!"
echo "Executable location: build/bin/shortest_path"
echo ""
echo "To test with the example data:"
echo "  cd build/bin"
echo "  ./shortest_path example_input.json"
echo ""