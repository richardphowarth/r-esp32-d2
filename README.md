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
│   ├─ controller.cpp       # Controller ESP32 (TX)
│   ├─ drive_node.cpp       # Drive ESP32 (RX)
│   └─ dome_node.cpp        # Dome ESP32 (RX)
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

## **4. Configure Receiver MAC Addresses**

1. Flash this to each **Drive** and **Dome ESP32** to get its MAC:

   ```cpp
   #include <WiFi.h>
   void setup() {
     Serial.begin(115200);
     WiFi.mode(WIFI_STA);
     Serial.println(WiFi.macAddress());
   }
   void loop() {}
   ```

2. Copy the printed MAC addresses into **`controller.cpp`**:

   ```cpp
   uint8_t driveNode[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};
   uint8_t domeNode[]  = {0x24, 0x6F, 0x28, 0xDD, 0xEE, 0xFF};
   ```

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

## **8. Next Steps**

Once LED testing is reliable:

1. Connect **motor driver** to **Drive Node**  
2. Connect **servo/stepper + lights/audio** to **Dome Node**  
3. Use the same ESP-NOW and heartbeat system for safe control  
4. Add **battery, slip ring, and buck converters** for full R2-D2 deployment

---

## **9. Troubleshooting**

- **LED stays off** → Check MAC addresses & pairing  
- **LED flickers at idle** → Adjust deadzone in `controller.cpp`  
- **Packet loss in Serial** → Reduce obstacles, keep ESP32s within ~10m  
- **Controller doesn’t pair** → Use **Xbox One S / Series X|S Bluetooth models only**  
