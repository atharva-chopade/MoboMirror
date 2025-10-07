# Installation Guide - MoboMirror

This guide will help you install and set up MoboMirror on your system.

## Table of Contents

- [System Requirements](#system-requirements)
- [Linux Installation](#linux-installation)
- [macOS Installation](#macos-installation)
- [Post-Installation Setup](#post-installation-setup)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

## System Requirements

### Hardware
- USB 2.0 or 3.0 port
- 2GB RAM minimum (4GB recommended)
- Multi-core processor recommended for smooth video playback

### Software
- Go 1.16 or later
- Qt 6.x
- GStreamer 1.0
- libusb 1.0
- pkg-config

### iOS Devices
- iPhone or iPad running iOS 12 or later
- Device must support QuickTime screen capture

## Linux Installation

### Ubuntu/Debian

```bash
# Update package list
sudo apt-get update

# Install Go (if not already installed)
sudo apt-get install -y golang-go

# Install build dependencies
sudo apt-get install -y \
    libusb-1.0-0-dev \
    libglib2.0-dev \
    pkg-config \
    build-essential

# Install GStreamer
sudo apt-get install -y \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav

# Install Qt6
sudo apt-get install -y \
    qt6-base-dev \
    qt6-multimedia-dev \
    qmake6

# Clone the repository
git clone https://github.com/atharva-chopade/MoboMirror.git
cd MoboMirror

# Build the Go backend
go build -o qvh

# Build the Qt GUI
cd qt-app
./build.sh
```

### Fedora/RHEL/CentOS

```bash
# Install Go
sudo dnf install -y golang

# Install dependencies
sudo dnf install -y \
    libusb-devel \
    glib2-devel \
    pkg-config \
    gcc-c++

# Install GStreamer
sudo dnf install -y \
    gstreamer1-devel \
    gstreamer1-plugins-base-devel \
    gstreamer1-plugins-good \
    gstreamer1-plugins-bad-free \
    gstreamer1-plugins-ugly-free

# Install Qt6
sudo dnf install -y \
    qt6-qtbase-devel \
    qt6-qtmultimedia-devel

# Build (same as Ubuntu)
git clone https://github.com/atharva-chopade/MoboMirror.git
cd MoboMirror
go build -o qvh
cd qt-app
./build.sh
```

### Arch Linux

```bash
# Install dependencies
sudo pacman -S \
    go \
    libusb \
    glib2 \
    pkg-config \
    qt6-base \
    qt6-multimedia \
    gstreamer \
    gst-plugins-base \
    gst-plugins-good \
    gst-plugins-bad \
    gst-plugins-ugly

# Build
git clone https://github.com/atharva-chopade/MoboMirror.git
cd MoboMirror
go build -o qvh
cd qt-app
./build.sh
```

## macOS Installation

### Using Homebrew

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install \
    go \
    libusb \
    pkg-config \
    qt@6 \
    gstreamer \
    gst-plugins-base \
    gst-plugins-good \
    gst-plugins-bad \
    gst-plugins-ugly

# Add Qt to PATH
export PATH="/usr/local/opt/qt@6/bin:$PATH"
export PKG_CONFIG_PATH="/usr/local/opt/qt@6/lib/pkgconfig:$PKG_CONFIG_PATH"

# Clone and build
git clone https://github.com/atharva-chopade/MoboMirror.git
cd MoboMirror
go build -o qvh
cd qt-app
./build.sh
```

## Post-Installation Setup

### Linux USB Permissions

On Linux, you need to set up udev rules for USB access:

1. Create a udev rules file:
   ```bash
   sudo nano /etc/udev/rules.d/70-iphone.rules
   ```

2. Add the following content:
   ```
   # iPhone in normal mode
   SUBSYSTEM=="usb", ATTR{idVendor}=="05ac", ATTR{idProduct}=="12a8", MODE="0666", GROUP="plugdev"
   
   # iPhone in QuickTime mode
   SUBSYSTEM=="usb", ATTR{idVendor}=="05ac", ATTR{idProduct}=="12ab", MODE="0666", GROUP="plugdev"
   
   # iPad
   SUBSYSTEM=="usb", ATTR{idVendor}=="05ac", MODE="0666", GROUP="plugdev"
   ```

3. Reload udev rules:
   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

4. Add your user to the plugdev group:
   ```bash
   sudo usermod -a -G plugdev $USER
   ```

5. Log out and log back in for group changes to take effect.

### iOS Device Setup

1. **Connect your iPhone/iPad** via USB cable
2. **Unlock the device** 
3. **Trust the computer** when prompted on the device
4. **Keep the device unlocked** during first-time setup

### Pairing (Required for Wireless)

For wireless mirroring, pair your device:

```bash
# Using go-ios (if available)
ios pair

# Or use iTunes/Finder on Mac
# The device will be paired when you trust the computer
```

## Verification

### Test the Go Backend

```bash
# Change to MoboMirror directory
cd /path/to/MoboMirror

# List connected devices
./qvh devices -v

# You should see output like:
# {"device_list":[{"serial_number":"xxxx","product_name":"iPhone","screen_mirroring_activated":false}]}
```

### Test the Qt GUI

```bash
# Run the Qt application
cd qt-app/build
./MoboMirror

# The application window should open
# You should see your device in the dropdown
```

### Test Mirroring

1. Open the MoboMirror application
2. Select your device from the dropdown
3. Choose "USB" connection type
4. Click "Start Mirroring"
5. A GStreamer window should open showing your device screen

## Troubleshooting

### Device Not Found

**Problem:** `./qvh devices` shows no devices

**Solutions:**
- Check USB cable is properly connected
- Verify device is unlocked
- Ensure "Trust This Computer" was accepted
- On Linux, check USB permissions (see udev rules above)
- Try a different USB port
- Run with sudo to test if it's a permissions issue: `sudo ./qvh devices`

### Build Errors

**Problem:** Go build fails with libusb errors

**Solution:**
```bash
# Install libusb development files
sudo apt-get install libusb-1.0-0-dev  # Ubuntu/Debian
sudo dnf install libusb-devel          # Fedora/RHEL
```

**Problem:** Qt build fails

**Solution:**
```bash
# Make sure Qt6 is installed
which qmake6

# If not found, install Qt6:
sudo apt-get install qt6-base-dev qt6-multimedia-dev
```

### GStreamer Issues

**Problem:** Video window doesn't open

**Solution:**
```bash
# Install all GStreamer plugins
sudo apt-get install gstreamer1.0-plugins-*

# Test GStreamer
gst-launch-1.0 videotestsrc ! autovideosink
```

### Permission Denied Errors

**Problem:** "Permission denied" when accessing USB

**Solution:**
1. Check udev rules are installed (see Post-Installation Setup)
2. Verify you're in the plugdev group: `groups $USER`
3. Log out and back in
4. Try running with sudo as a test: `sudo ./qvh devices`

### Qt Application Won't Start

**Problem:** Qt app crashes or won't start

**Solution:**
```bash
# Check Qt dependencies
ldd qt-app/build/MoboMirror

# Install missing Qt modules
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# Run with debug output
QT_DEBUG_PLUGINS=1 ./MoboMirror
```

### Wireless Mirroring Not Working

**Problem:** Can't connect wirelessly

**Solution:**
1. Ensure device is paired (connect via USB first)
2. Verify device and computer are on same network
3. Check firewall isn't blocking usbmuxd
4. Install usbmuxd: `sudo apt-get install usbmuxd`
5. Restart usbmuxd: `sudo systemctl restart usbmuxd`

## Getting Help

If you continue to have issues:

1. Check the [ARCHITECTURE.md](ARCHITECTURE.md) for technical details
2. Review the [qt-app/README.md](qt-app/README.md) for Qt-specific help
3. Run diagnostics: `./qvh diagnostics output.csv --dump=dump.bin`
4. Create an issue on GitHub with:
   - Your operating system and version
   - Output of `./qvh devices -v`
   - Error messages
   - Diagnostic files (if applicable)

## Next Steps

Once installed:
- Read the [README.md](README.md) for usage instructions
- Check [ARCHITECTURE.md](ARCHITECTURE.md) to understand how it works
- Review [qt-app/README.md](qt-app/README.md) for GUI-specific features

Happy mirroring! 📱➡️🖥️
