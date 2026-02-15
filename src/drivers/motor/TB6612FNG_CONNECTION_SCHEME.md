# 🔌 Схема підключення TB6612FNG до Black Pill (Breadboard)

**Версія:** 3.1 - Спрощена (Схеми + Чеклист)
**Дата:** 15.02.2026
**Проект:** STM32_Core - Управління 4 DC моторами з ESP32 + Black Pill

---

## 📌 Загальна інформація

Цей документ описує **реальну схему підключення** на макетній платі **MB-102** для системи керування 4 DC моторами з:

- **ESP32-WROOM-32D** (Master Controller - Wi-Fi, навігація)
- **STM32F411 Black Pill** (Motor Controller - PWM генерація)
- **2× TB6612FNG** (H-bridge драйвери для 4 моторів)

### Архітектура системи

```
┌───────────────────────────────────────────────────────────┐
│                   SYSTEM ARCHITECTURE                      │
│                                                            │
│  ESP32-WROOM-32D  ←──[UART]──→  Black Pill STM32F411     │
│  (Master)                        (Motor Controller)       │
│  • Wi-Fi / BT                    • PWM generation         │
│  • Navigation                    • Real-time control      │
│  • High-level logic              • 4× motor drivers       │
│                                  │                         │
│                                  ↓                         │
│                           TB6612FNG #1 & #2               │
│                           (H-bridge drivers)              │
│                                  ↓                         │
│                           4× DC Motors                    │
└───────────────────────────────────────────────────────────┘
```

---

## ✅ Assembly Checklist

### 1️⃣ Component Placement

- [x] ESP32-WROOM-32D installed at Rows 1-19 (right side pins, facing left) ✅
- [x] Black Pill STM32F411 installed at Rows 23-42 (facing left, USB top) ✅
- [x] TB6612FNG Driver 1 installed at Rows 45-52 (facing left) ✅
- [x] TB6612FNG Driver 2 installed at Rows 54-61 (facing left) ✅
- [x] All modules firmly seated in breadboard ✅

### 2️⃣ Power Rails Setup

- [x] TOP [ + ] rail = 3.3V (from ESP32 3V3 output) ✅
- [x] TOP [ - ] rail = GND (common ground) ✅
- [ ] BOTTOM [ + ] rail = 6-12V (external motor power)
- [ ] BOTTOM [ - ] rail = GND (motor power ground)
- [x] **CRITICAL:** TOP [ - ] connected to BOTTOM [ - ] (vertical jumper!) ✅

### 3️⃣ UART Communication (ESP32 ↔ Black Pill)

- [ ] ESP32 IO17 → Black Pill PA10 (RX)
- [ ] ESP32 IO16 ← Black Pill PA9 (TX)
- [ ] ESP32 GND → TOP [ - ] rail
- [ ] Black Pill GND → TOP [ - ] rail

### 4️⃣ Driver 1 Control Signals (Black Pill → TB6612FNG #1)

- [x] 3V3 → VCC (Driver 1) ✅
- [x] GND → GND (Driver 1) ✅
- [x] PB0 → PWMA ✅
- [x] PB1 → AIN1 ✅
- [x] PB10 → AIN2 ✅
- [x] PB14 → STBY ✅
- [x] PB12 → BIN1 ✅
- [x] PB13 → BIN2 ✅
- [x] PB7 → PWMB ✅

### 5️⃣ Driver 2 Control Signals (Black Pill → TB6612FNG #2)

- [x] 3V3 → VCC (Driver 2) ✅
- [x] GND → GND (Driver 2) ✅
- [x] PA8 → PWMA ✅
- [x] PA0 → AIN1 ✅
- [x] PA1 → AIN2 ✅
- [x] PA4 → STBY ✅
- [x] PA2 → BIN1 ✅
- [x] PA3 → BIN2 ✅
- [x] PA15 → PWMB ✅

### 6️⃣ External Motor Power (6-12V)

- [x] External power supply 6-12V prepared ✅
- [x] (+) wire → BOTTOM [ + ] rail ✅
- [x] (-) wire → BOTTOM [ - ] rail ✅
- [x] TB6612FNG #1 VM pin connected to BOTTOM [ + ] rail ✅
- [x] TB6612FNG #2 VM pin connected to BOTTOM [ + ] rail ✅
- [ ] Polarity checked (+ and - correct)
- [ ] **Capacitor 100-1000µF** on VM line (recommended for stability)

### 7️⃣ Motor Connections

- [ ] Motor 0 → Driver 1 (A01, A02)
- [ ] Motor 1 → Driver 1 (B01, B02)
- [ ] Motor 2 → Driver 2 (A01, A02)
- [ ] Motor 3 → Driver 2 (B01, B02)

### 8️⃣ Final Verification

- [ ] All connections secure (no loose wires)
- [ ] No short circuits visible
- [ ] Power polarity correct (+/-)
- [ ] Common GND between all components
- [ ] Wires not stressed or twisted
- [ ] ESP32 USB cable ready for power
- [ ] Black Pill USB cable ready for programming

---

**Version 3.1 - Simplified**
**Key Change:** PB11 → PB14 (Driver 1 STBY)
