# 🚀 STM32 → ESP32 → Node.js: Quick Setup

## 📋 Prerequisites

### Hardware

- [ ] STM32F411CEU6 Black Pill
- [ ] ESP32-WROOM-32D
- [ ] ST-Link V2 programmer
- [ ] 4 wires (GND, TX, RX, 5V)
- [ ] 1 USB cable (for ESP32 only)

### Software

- [ ] PlatformIO CLI installed
- [ ] Arduino IDE 2.x installed
- [ ] Node.js installed

---

## 🔌 Wiring

### Connect STM32 ↔ ESP32

- [x] STM32 PA9 (TX) → ESP32 **RXD0** (GPIO3)
- [x] STM32 PA10 (RX) → ESP32 **TXD0** (GPIO1)
- [x] STM32 GND → ESP32 **GND**
- [x] STM32 5V → ESP32 **5V** (top-left pin)

⚠️ **Important:**

- TX → RX, RX → TX (crossover connection)
- Common GND required
- Connect **5V to 5V** (NOT 3.3V to 3.3V!)
- USB connected to ESP32 only — ESP32 powers STM32 via 5V pin

---

## 1️⃣ Setup Node.js Server

### Install Dependencies

```bash
cd ~/Documents/PlatformIO/Projects/RobotServer
npm install
```

### Start Server

```bash
node server.js
```

**Expected output:**

```text
✅ Server running on: http://localhost:3000
📡 Network address: http://192.168.0.4:3000
```

### Get Your Computer's IP Address

```bash
# macOS/Linux
ifconfig | grep "inet " | grep -v 127.0.0.1

# Windows
ipconfig | findstr IPv4
```

**Checklist:**

- [ ] Server running
- [ ] IP address noted: `___.___.___.___`

---

## 2️⃣ Flash ESP32

### Arduino IDE Setup

- [ ] File → Preferences → Additional Boards Manager URLs:

  ```text
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```

- [ ] Tools → Board → Boards Manager → Install "esp32"
- [ ] Tools → Manage Libraries → Install "ArduinoJson" (v6.x)

### Configure Board

- [ ] Tools → Board → ESP32 Dev Module
- [ ] Upload Speed: `115200`
- [ ] Port: Select ESP32 port

### Edit WiFi Settings

Open: `~/Documents/PlatformIO/Projects/STM32_Core/esp32/esp32_wifi_bridge.ino`

**Configure:**

- [ ] Change `WIFI_SSID` to your WiFi name
- [ ] Change `WIFI_PASSWORD` to your WiFi password
- [ ] Change `SERVER_URL` to your computer's IP: `http://192.168.X.X:3000/data`

### Upload to ESP32

- [ ] Connect ESP32 via USB
- [ ] Click Upload (→) button
- [ ] Open Serial Monitor (115200 baud)
- [ ] Verify WiFi connected

**Expected Serial output:**

```text
[WiFi] ✓ Подключено!
[WiFi] IP адрес: 192.168.X.X
```

---

## 3️⃣ Flash STM32

### Build and Upload

```bash
cd ~/Documents/PlatformIO/Projects/STM32_Core
platformio run
platformio run --target upload
```

**Or use VS Code:**

- [ ] Click "Upload" in PlatformIO toolbar

### Verify

- [ ] LED on PC13 blinks (1 Hz = system working)

---

## 4️⃣ Test System

### Startup Sequence

1. [ ] Node.js server running
2. [ ] ESP32 connected to WiFi (check Serial Monitor)
3. [ ] STM32 flashed and powered
4. [ ] All wires connected (PA9→GPIO3, PA10→GPIO1, GND→GND)

### Test Button Press

- [ ] Press BTN_0 on STM32
- [ ] Motor 0 starts
- [ ] LED_0 turns ON
- [ ] Server displays button data:

  ```json
  {"button":0,"state":"pressed"}
  ```

### Test All Buttons

- [ ] BTN_0 → Motor 0 + LED_0
- [ ] BTN_1 → Motor 1 + LED_1
- [ ] BTN_2 → Motor 2 + LED_2
- [ ] BTN_3 → Motor 3 + LED_3

---

## 🐛 Troubleshooting

### ESP32 не подключается к WiFi

- [ ] Check WiFi SSID and password in code
- [ ] Ensure router is 2.4 GHz (ESP32 doesn't support 5 GHz)
- [ ] Move ESP32 closer to router

### ESP32 не получает данные от STM32

- [ ] Verify TX/RX crossover: PA9→GPIO3, PA10→GPIO1
- [ ] Check GND connection
- [ ] Confirm baud rate 115200 on both sides

### Сервер не получает данные

- [ ] Verify computer IP address in ESP32 code
- [ ] Check firewall allows port 3000
- [ ] Ensure ESP32 and computer on same WiFi network

### Кракозябры в данных

- [ ] Check baud rate = 115200 everywhere
- [ ] Use short wires (< 30 cm)
- [ ] Check wire connections

---

## ✅ Quick Checklist

### Ready to go?

- [ ] Server running on port 3000
- [ ] ESP32 WiFi connected
- [ ] STM32 powered and programmed
- [ ] 3 wires connected (TX, RX, GND)
- [ ] Button press sends data to server

**System operational! 🎉**
