# Quick Start Guide - MoboMirror

Get up and running with MoboMirror in 5 minutes!

## Prerequisites

Before you begin, make sure you have:
- An iPhone or iPad
- A USB cable
- A Linux or macOS computer
- Administrative/sudo access

## Step 1: Install Dependencies

### Ubuntu/Debian (One Command):
```bash
sudo apt-get update && sudo apt-get install -y \
    golang-go libusb-1.0-0-dev libglib2.0-dev pkg-config \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
    qt6-base-dev qt6-multimedia-dev qmake6
```

### macOS (Homebrew):
```bash
brew install go libusb pkg-config qt@6 gstreamer \
    gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly
```

## Step 2: Clone and Build

```bash
# Clone the repository
git clone https://github.com/atharva-chopade/MoboMirror.git
cd MoboMirror

# Build the Go backend
go build -o qvh

# Build the Qt GUI
cd qt-app
./build.sh
```

## Step 3: Set Up USB Permissions (Linux Only)

```bash
# Create udev rules
sudo tee /etc/udev/rules.d/70-iphone.rules <<EOF
SUBSYSTEM=="usb", ATTR{idVendor}=="05ac", MODE="0666", GROUP="plugdev"
EOF

# Reload udev
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add yourself to plugdev group
sudo usermod -a -G plugdev $USER

# Log out and back in for group to take effect
```

## Step 4: Connect Your iPhone

1. **Connect** your iPhone via USB
2. **Unlock** the device
3. **Trust** the computer when prompted
4. Keep the device **unlocked**

## Step 5: Run MoboMirror

```bash
cd MoboMirror/qt-app/build
./MoboMirror
```

## Step 6: Start Mirroring

1. Your iPhone should appear in the device dropdown
2. Select **"USB"** as the connection type
3. Click **"Start Mirroring"**
4. A GStreamer window will open showing your iPhone screen! 🎉

## Troubleshooting

### No device found?
```bash
# Test the backend directly
cd /path/to/MoboMirror
./qvh devices -v
```

If you see your device in the output but not in the GUI, try restarting the Qt app.

### Permission errors?
Make sure you:
- Created the udev rules (Linux)
- Added yourself to plugdev group
- Logged out and back in

### Build errors?
Check that all dependencies are installed:
```bash
# Check Qt
which qmake6

# Check Go
go version

# Check GStreamer
gst-launch-1.0 --version
```

## Next Steps

- Read [INSTALL.md](INSTALL.md) for detailed installation
- Check [ARCHITECTURE.md](ARCHITECTURE.md) to understand how it works
- See [qt-app/README.md](qt-app/README.md) for GUI-specific features

## Command Line Alternative

If you prefer the command line:

```bash
# List devices
./qvh devices

# Mirror a specific device
./qvh gstreamer --udid=YOUR_DEVICE_UDID
```

## Common Commands

```bash
# List devices with details
./qvh devices -v

# Activate QuickTime config
./qvh activate --udid=YOUR_DEVICE_UDID

# Record video and audio to files
./qvh record video.h264 audio.wav --udid=YOUR_DEVICE_UDID

# Record audio only (no status bar change)
./qvh audio output.wav --wav --udid=YOUR_DEVICE_UDID
```

## Getting Help

If you encounter issues:

1. Check the [INSTALL.md](INSTALL.md) troubleshooting section
2. Run diagnostics: `./qvh diagnostics output.csv --dump=dump.bin`
3. Check existing issues on GitHub
4. Create a new issue with:
   - Your OS and version
   - Output of `./qvh devices -v`
   - Any error messages

## Features to Try

- **Recording**: Use the command line to record videos
- **Audio Only**: Mirror just audio without changing the status bar
- **Custom Pipelines**: Use GStreamer pipelines for advanced streaming

## Tips

💡 **Tip**: Keep your iPhone unlocked during first connection  
💡 **Tip**: Use a good quality USB cable for best results  
💡 **Tip**: Close other applications using USB (iTunes, Xcode, etc.)  
💡 **Tip**: Try different USB ports if you have connection issues  

## What's Next?

Now that you have MoboMirror running, explore:
- Recording capabilities
- Custom GStreamer pipelines
- Wireless mirroring (when available)
- Screen recording for presentations

Happy mirroring! 📱➡️💻
