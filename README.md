# **R2-D2 ESP32 Test Project**

A modular **ESP32-based control system** for an R2-D2 droid using:

- **Xbox One Controller** (Bluetooth)
- **Bluepad32 library**
- **ESP-NOW wireless peer-to-peer**
- **Heartbeat + Failsafe** for safety
- **Drive Node & Dome Node separation**

This project is designed for **testing with onboard LEDs** before attaching motors, servos, lights, and audio hardware.

---

## **1. Project Structure**

```
R2D2-ESP32/
│
├─ platformio.ini           # PlatformIO config
├─ include/
│   └─ R2D2Shared.h         # Shared structs for ESP-NOW
├─ src/
│   ├─ config_template.h    # MAC address template (shared)
│   ├─ config.h             # Local MAC address config (gitignored)
│   ├─ controller/main.cpp  # Controller ESP32 (TX)
│   ├─ drive_node/main.cpp  # Drive ESP32 (RX)
│   └─ dome_node/main.cpp   # Dome ESP32 (RX)
└─ README.md
```

---

## **2. Requirements**

### **Hardware**

- 3 × **ESP32-WROOM-32 dev boards**
- 1 × **Xbox One S / Series X|S Controller** (Bluetooth model)
- USB cables for programming
- (Optional) WS2812 RGB LED or LED strip for visual testing

### **Software**

- **VS Code** with **PlatformIO**
- **ESP32 Platform** installed via PlatformIO
- Libraries (declared in `platformio.ini`):
  - [Bluepad32](https://github.com/ricardoquesada/bluepad32)

---

## **3. Setup Instructions**

1. **Clone or Copy the Project**

   ```bash
   git clone https://github.com/yourusername/R2D2-ESP32.git
   cd R2D2-ESP32
   ```

2. **Open in VS Code → PlatformIO**

3. **Check `platformio.ini`**

   - Board: `esp32dev`
   - Framework: `arduino`
   - Serial speed: `115200`

4. **Install Libraries**
   - PlatformIO will auto-install dependencies from `lib_deps`.

---

## **4. Configure MAC Addresses (via `config.h`)**

1. Flash this sketch to each **Drive** and **Dome ESP32** to get its MAC address:

   ```cpp
   #include <WiFi.h>
   void setup() {
     Serial.begin(115200);
     WiFi.mode(WIFI_STA);
     Serial.println(WiFi.macAddress());
   }
   void loop() {}
   ```

2. Copy the printed MAC addresses into a new file:

   - Duplicate `src/config_template.h` to `src/config.h`:

     ```bash
     cp src/config_template.h src/config.h
     ```

3. Replace the dummy MACs in `config.h` with your real ones:

   ```cpp
   const uint8_t DRIVE_NODE_MAC[] = { 0x94, 0x54, 0xC5, 0x63, 0x58, 0xA0 };
   const uint8_t DOME_NODE_MAC[]  = { 0x8C, 0x4B, 0x14, 0x04, 0x66, 0xC0 };
   ```

4. **Do not commit `config.h`** — it's already in `.gitignore`.

5. In your code (e.g. `controller.cpp`), include the config:

   ```cpp
   #include "config.h"
   ```

This keeps your setup private while making the project fully shareable.

---

## **5. Flashing the Nodes**

- **Controller Node (TX)** → `src/controller.cpp`
- **Drive Node (RX)** → `src/drive_node.cpp`
- **Dome Node (RX)** → `src/dome_node.cpp`

Use PlatformIO’s **Build → Upload → Monitor** for each board.

---

## **6. Test Behavior**

**Drive Node LED**

- Forward Stick → LED solid, brightness = speed
- Reverse Stick → LED blinks, brightness = speed
- Stick Center → LED off
- Controller Disconnect → LED off (failsafe)

**Dome Node LED**

- Right Stick → LED brightness = dome rotation strength
- X Button → Fast blink
- Y Button → Slow blink
- Disconnect → LED off (failsafe)

**Serial Output**

- Shows uptime, packets sent/received, missed packets, free heap

---

## **7. Safety Features**

- **Heartbeat (sequence number)**: Detects stale packets
- **Failsafe Timeout (500ms)**: Stops motion if controller signal lost
- **Deadzone Filtering**: Optional in `controller.cpp` to prevent drift

---

## **8. Terminal Commands (PlatformIO CLI)**

With the following environments in `platformio.ini`:

- `controller` → `/dev/cu.usbserial-10`
- `drive_node` → `/dev/cu.usbserial-110`
- `dome_node` → `/dev/cu.usbserial-3130`

### **Build**

```bash
pio run -e controller
pio run -e drive_node
pio run -e dome_node
```

### **Upload**

```bash
pio run -t upload -e controller
pio run -t upload -e drive_node
pio run -t upload -e dome_node
```

### **Monitor Serial Output**

```bash
pio device monitor -e controller
pio device monitor -e drive_node
pio device monitor -e dome_node
```

### **Build, Upload, and Monitor**

```bash
pio run -t upload -e controller && pio device monitor -e controller
```

### **Clean Build**

```bash
pio run -t clean -e controller
pio run -t clean -e drive_node
pio run -t clean -e dome_node
```

---

## **9. Next Steps**

Once LED testing is reliable:

1. Connect **motor driver** to **Drive Node**
2. Connect **servo/stepper + lights/audio** to **Dome Node**
3. Use the same ESP-NOW and heartbeat system for safe control
4. Add **battery, slip ring, and buck converters** for full R2-D2 deployment

---

## **10. Troubleshooting**

- **LED stays off** → Check MAC addresses & pairing
- **LED flickers at idle** → Adjust deadzone in `controller.cpp`
- **Packet loss in Serial** → Reduce obstacles, keep ESP32s within ~10m
- **Controller doesn’t pair** → Use **Xbox One S / Series X|S Bluetooth models only**
