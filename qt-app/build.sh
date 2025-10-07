#!/bin/bash
# Build script for MoboMirror Qt Application

set -e

echo "Building MoboMirror Qt Application..."
echo

# Check if Qt6 is installed
if ! command -v qmake6 &> /dev/null; then
    echo "Error: qmake6 not found. Please install Qt6 development tools."
    exit 1
fi

# Navigate to qt-app directory
cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# Run qmake
echo "Running qmake..."
qmake6 ..

# Build
echo "Building..."
make

echo
echo "Build complete!"
echo "Binary location: $(pwd)/MoboMirror"
echo
echo "To run the application:"
echo "  cd $(pwd)"
echo "  ./MoboMirror"
echo
echo "Note: Make sure the qvh binary is built in the parent directory."
