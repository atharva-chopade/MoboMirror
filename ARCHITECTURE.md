# MoboMirror Architecture Documentation

## Overview

MoboMirror consists of two main components:

1. **Go Backend (`qvh`)** - Handles USB/wireless communication with iOS devices
2. **Qt GUI Application** - Provides user-friendly interface for device management and mirroring

## Architecture Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                      User Interface Layer                     │
│                                                                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │         Qt GUI Application (C++/Qt6)                   │  │
│  │  - MainWindow: UI management & event handling          │  │
│  │  - DeviceManager: Device discovery & control           │  │
│  │  - VideoWidget: Display area & user messages           │  │
│  └────────┬───────────────────────────────────────────────┘  │
└───────────┼──────────────────────────────────────────────────┘
            │ QProcess (spawn & control)
            ▼
┌──────────────────────────────────────────────────────────────┐
│                     Backend Processing Layer                  │
│                                                                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │         Go Backend (qvh binary)                        │  │
│  │  - screencapture: Device detection & USB control       │  │
│  │  - activator: QuickTime config management              │  │
│  │  - usbadapter: USB bulk endpoint reading               │  │
│  │  - messageprocessor: Protocol handling                 │  │
│  └────────┬───────────────────────────────────────────────┘  │
└───────────┼──────────────────────────────────────────────────┘
            │ libusb / usbmuxd
            ▼
┌──────────────────────────────────────────────────────────────┐
│                      Hardware Layer                           │
│                                                                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │         iOS Device (iPhone/iPad)                       │  │
│  │  - QuickTime streaming interface                       │  │
│  │  - H.264 video encoding                                │  │
│  │  - Audio streaming                                     │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
            │
            ▼
┌──────────────────────────────────────────────────────────────┐
│                      Display Layer                            │
│                                                                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │         GStreamer Pipeline                             │  │
│  │  - h264parse: Parse H.264 stream                       │  │
│  │  - avdec_h264: Decode video                            │  │
│  │  - Audio decoding & playback                           │  │
│  │  - Video rendering (xvimagesink/osxvideosink)          │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
```

## Component Details

### Qt GUI Application

#### MainWindow
- **Responsibilities:**
  - UI layout and widget management
  - Event handling for buttons and combo boxes
  - Process lifecycle management for `qvh` backend
  - Status updates and error handling
  
- **Key Methods:**
  - `onRefreshDevices()`: Triggers device discovery
  - `onStartMirroring()`: Launches mirroring process
  - `onStopMirroring()`: Terminates mirroring gracefully
  - `updateDeviceList()`: Refreshes device list from backend
  - `updateUI()`: Updates button states based on current state

#### DeviceManager
- **Responsibilities:**
  - Execute `qvh` commands for device operations
  - Parse JSON output from backend
  - Maintain list of available devices
  
- **Key Methods:**
  - `getDevices()`: Returns list of connected iOS devices
  - `activateDevice()`: Enable QuickTime config on device
  - `deactivateDevice()`: Disable QuickTime config
  - `parseDevicesOutput()`: Parse JSON from `qvh devices`

#### VideoWidget
- **Responsibilities:**
  - Display placeholder content before/after mirroring
  - Show status messages to user
  - Provide dark background for better contrast
  
- **Key Methods:**
  - `clearScreen()`: Reset display after mirroring stops
  - `displayMessage()`: Show custom message to user

### Go Backend (qvh)

#### Device Discovery
- Uses libusb to enumerate USB devices
- Identifies iOS devices by VID/PID (Apple Inc.)
- Checks for QuickTime interface availability
- Returns device info as JSON

#### QuickTime Configuration
- Sends control transfer to enable hidden QuickTime config
- Device disconnects and reconnects with new configuration
- Exposes bulk endpoints for AV streaming
- Configuration includes:
  - Video: H.264 encoded stream
  - Audio: PCM/compressed audio stream

#### USB Adapter
- Claims bulk endpoints for reading/writing
- Handles USB transfer errors and retries
- Implements frame extraction from raw USB data
- Manages endpoint synchronization

#### Message Processor
- Parses CoreMedia protocol messages
- Extracts video frames (CMSampleBuffer)
- Handles audio samples
- Forwards data to consumers (GStreamer, file writers, etc.)

## Data Flow

### USB Mirroring Flow

1. **User Action:**
   ```
   User clicks "Start Mirroring" → Qt GUI
   ```

2. **Device Activation:**
   ```
   Qt → QProcess → qvh activate --udid=<device_id>
   qvh → USB Control Transfer → iOS Device
   iOS Device → Reconnects with QuickTime config
   ```

3. **Streaming Start:**
   ```
   Qt → QProcess → qvh gstreamer --udid=<device_id>
   qvh → Claims USB bulk endpoints
   qvh → Sends sync/async messages to device
   iOS Device → Starts sending H.264 + Audio data
   ```

4. **Data Processing:**
   ```
   iOS Device → USB Bulk Endpoint → qvh UsbAdapter
   UsbAdapter → MessageProcessor → Frame Extraction
   Frames → GStreamer Pipeline → Video Display Window
   ```

5. **Stop:**
   ```
   User clicks "Stop Mirroring" → Qt GUI
   Qt → Terminates qvh process
   qvh cleanup → Releases USB endpoints
   iOS Device → Stops streaming
   ```

### Wireless Mirroring Flow

Wireless mirroring uses the same protocol but over network:

1. **Device Pairing:**
   - Device must be paired using `go-ios` or similar tool
   - Usbmuxd establishes network connection
   - Device advertises itself over mDNS/Bonjour

2. **Network Tunnel:**
   - Usbmuxd creates TCP tunnel to device
   - Same USB protocol encapsulated over TCP
   - Higher latency but wireless convenience

3. **Streaming:**
   - Same as USB flow but over network socket
   - Slightly higher latency
   - Requires stable WiFi connection

## Protocol Details

### QuickTime Streaming Protocol

The iOS QuickTime streaming uses CoreMedia framework protocol:

1. **Initialization:**
   - Sync messages establish timing
   - CLOK (clock) messages synchronize timestamps
   - AFMT (audio format) defines audio parameters

2. **Streaming:**
   - FEED messages contain video frames
   - Each frame has presentation timestamp
   - Audio samples in separate messages
   - Frames are H.264 NAL units

3. **Control:**
   - STOP message ends streaming
   - PING/PONG for keepalive
   - Error messages for problems

### Message Structure

```
┌─────────────────────────────────────┐
│  Message Header (32 bytes)          │
│  - Magic: "sync" or "asyn"          │
│  - Type: 4-char code (FEED, etc)    │
│  - Length: payload size             │
│  - Timestamp: device time           │
├─────────────────────────────────────┤
│  Message Payload (variable)         │
│  - Depends on message type          │
│  - For FEED: H.264 NAL units       │
│  - For audio: PCM samples           │
└─────────────────────────────────────┘
```

## Building and Integration

### Prerequisites

**Go Backend:**
- Go 1.16+
- libusb-1.0
- GStreamer 1.0
- pkg-config

**Qt GUI:**
- Qt 6.x
- C++ compiler (g++/clang)
- Qt Widgets, Multimedia modules

### Build Process

1. **Build Go Backend:**
   ```bash
   cd /path/to/MoboMirror
   go build -o qvh
   ```

2. **Build Qt GUI:**
   ```bash
   cd qt-app
   mkdir build && cd build
   qmake6 ..
   make
   ```

3. **Install:**
   - Copy `qvh` to system PATH or keep with Qt app
   - Qt app expects `qvh` in `../qvh` relative to binary

### Integration Points

The Qt GUI integrates with Go backend through:

1. **Command Execution:**
   - Uses QProcess to spawn `qvh` commands
   - Captures stdout/stderr for status updates
   - JSON parsing for structured data

2. **Process Management:**
   - Monitors process lifecycle
   - Handles graceful termination
   - Deals with process crashes

3. **Error Handling:**
   - Parses JSON error messages
   - Displays user-friendly errors
   - Provides troubleshooting hints

## Future Enhancements

### Planned Features

1. **Embedded GStreamer:**
   - Display video directly in Qt widget
   - No separate GStreamer window
   - Better integration and control

2. **Recording:**
   - Add recording buttons in GUI
   - Save to various formats (MP4, MOV)
   - Screenshot capture

3. **Device Settings:**
   - Resolution/quality settings
   - Framerate control
   - Audio settings

4. **Multi-Device:**
   - Mirror multiple devices simultaneously
   - Grid layout for multiple streams
   - Independent controls per device

5. **Wireless Improvements:**
   - Better network detection
   - Automatic pairing flow
   - Connection quality indicators

### Technical Debt

1. **Error Handling:**
   - More robust USB error recovery
   - Better network timeout handling
   - Graceful degradation on errors

2. **Performance:**
   - Optimize frame processing
   - Reduce CPU usage
   - Better memory management

3. **Testing:**
   - Unit tests for Qt components
   - Integration tests for backend
   - Mock device for testing

## Troubleshooting Guide

### Common Issues

1. **Device Not Detected:**
   - Check USB cable
   - Verify device is unlocked
   - Trust computer on device
   - Check USB permissions (Linux)

2. **Mirroring Fails to Start:**
   - Verify GStreamer installed
   - Check `qvh` binary location
   - Review debug output

3. **Poor Video Quality:**
   - Check USB 2.0/3.0 connection
   - Verify CPU isn't bottleneck
   - Check GStreamer pipeline

4. **Audio Issues:**
   - Verify audio plugins installed
   - Check system audio settings
   - Review GStreamer audio elements

### Debug Commands

```bash
# Test backend directly
./qvh devices -v

# Test activation
./qvh activate --udid=<device> -v

# Test streaming with debug
./qvh gstreamer --udid=<device> -v

# Run diagnostics
./qvh diagnostics output.csv --dump=dump.bin
```

## License

This project follows the same license as the original quicktime_video_hack project.

## Credits

- Original QuickTime Video Hack by Daniel Paulus
- Qt GUI implementation for MoboMirror project
- Community contributions and testing
