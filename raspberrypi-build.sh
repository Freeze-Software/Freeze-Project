#!/bin/bash
set -e

PI_ENV="raspberry-pi"
BUILD_DIR=$(pwd)
ISO_OUTPUT="$BUILD_DIR/freeze.iso"
ARM_TOOLCHAIN="/usr/local/gcc-arm/"

if [ "$(uname -m)" == "armv7l" ] || [ "$(uname -m)" == "aarch64" ]; then
    echo "Running on Raspberry Pi. Starting the build process locally..."
else
    echo "Cross-compiling for Raspberry Pi. Ensure you have the correct toolchain installed."
    export PATH="$ARM_TOOLCHAIN/bin:$PATH"
fi

echo "Building FreezeOS for Raspberry Pi..."

cd "$(dirname "$0")"

echo "Cleaning previous build..."
make clean

echo "Compiling for Raspberry Pi..."
make

echo "Creating .iso image..."

echo "$ISO_OUTPUT"
echo "Build complete."
