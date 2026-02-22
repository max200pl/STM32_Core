# Motor Power Connection Checklist

**Project:** STM32_Core - 4 Motor Control System
**Step:** Connect 6-12V power bus and DC motors
**Prerequisite:** TESTING_CHECKLIST.md completed (logic tested without VM)

---

## WARNING - Read Before Starting

- **NEVER** connect 6-12V to 3.3V rail - destroys all ICs instantly
- **NEVER** reverse polarity on VM - destroys TB6612FNG instantly
- **ALWAYS** verify GND is shared between logic and motor power
- **ALWAYS** check capacitor polarity (electrolytic: long leg = +)
- **Disconnect USB** from STM32 before connecting motor power

---

## 1. Power-Off State

- [x] USB disconnected from Black Pill
- [x] USB disconnected from ESP32
- [x] External power supply OFF (or battery disconnected)
- [x] Multimeter ready

---

## 2. Capacitor Installation (VM Stability) - Optional

Place 100-1000 µF capacitor on BOTTOM [+] rail (VM line).

> **Can skip if no capacitor available.** Safe to test 1-2 motors at low speed (30-50%) without it.
> Required when running all 4 motors simultaneously or at high speed.

- [ ] Capacitor long leg (+) → BOTTOM [ + ] rail (6-12V)
- [ ] Capacitor short leg (-) → BOTTOM [ - ] rail (GND)
- [ ] Polarity correct (do not reverse!)

> Use any capacitor from 100 µF to 1000 µF, 16V+ rated.

---

## 3. GND Cross-Connection (Critical!)

Logic GND and motor GND must be common.

- [x] TOP [ - ] rail → BOTTOM [ - ] rail (vertical jumper wire)
- [x] Verify: both GND rails connected with one wire

> Without this, motors may not work or behave erratically.

---

## 4. VM Power Connections

Connect 6-12V to both TB6612FNG drivers.

### Driver 1

- [x] **VM** (Driver 1) → BOTTOM [ + ] rail (6-12V)

### Driver 2

- [x] **VM** (Driver 2) → BOTTOM [ + ] rail (6-12V)

---

## 5. External Power Supply Wiring

- [ ] Power supply (+) wire → BOTTOM [ + ] rail
- [ ] Power supply (-) wire → BOTTOM [ - ] rail
- [ ] Polarity verified with multimeter before connecting supply

> Acceptable voltage range: 6V - 12V DC.
> Recommended for first test: 6V (safer for motors).

---

## 6. Motor Connections

### Robot Layout (from code: button_control.c)

```text
           [FRONT]
  L                   R
Motor0 (A)         Motor2 (A)   <- front axle
  |                    |
Driver 1            Driver 2
  |                    |
Motor1 (B)         Motor3 (B)   <- rear axle
  L                   R
           [BACK]

Driver 1 = LEFT side:  M0=L-front, M1=L-rear
Driver 2 = RIGHT side: M2=R-front, M3=R-rear

BTN_0 = FORWARD  (all forward)
BTN_1 = ROTATE LEFT  (M0,M1=reverse; M2,M3=forward)
BTN_2 = ROTATE RIGHT (M0,M1=forward; M2,M3=reverse)
BTN_3 = BACKWARD (all reverse)
```

### Driver 1 Pinout (verified from tb6612fng.h)

```text
TB6612FNG Driver 1 pin  <- STM32 Black Pill pin
PWMA                    <- PB0  (TIM3_CH3)
AIN1                    <- PB1
AIN2                    <- PB10
STBY                    <- PB14
BIN1                    <- PB12
BIN2                    <- PB13
PWMB                    <- PB7  (TIM4_CH2)
```

### Driver 2 Pinout (verified from tb6612fng.h)

```text
TB6612FNG Driver 2 pin  <- STM32 Black Pill pin
PWMA                    <- PA8  (TIM1_CH1)
AIN1                    <- PA0
AIN2                    <- PA1
STBY                    <- PA4
BIN1                    <- PA2
BIN2                    <- PA3
PWMB                    <- PA15 (TIM2_CH1)
```

### Motor 0 - LEFT FRONT (Driver 1, Channel A)

- [ ] Motor 0 wire 1 → **A01** (Driver 1)
- [ ] Motor 0 wire 2 → **A02** (Driver 1)

### Motor 1 - LEFT REAR (Driver 1, Channel B)

- [ ] Motor 1 wire 1 → **B01** (Driver 1)
- [ ] Motor 1 wire 2 → **B02** (Driver 1)

### Motor 2 - RIGHT FRONT (Driver 2, Channel A)

- [ ] Motor 2 wire 1 → **A01** (Driver 2)
- [ ] Motor 2 wire 2 → **A02** (Driver 2)

### Motor 3 - RIGHT REAR (Driver 2, Channel B)

- [ ] Motor 3 wire 1 → **B01** (Driver 2)
- [ ] Motor 3 wire 2 → **B02** (Driver 2)

> Motor wire polarity determines rotation direction.
> If motor spins backwards - swap its two wires (A01 and A02, or B01 and B02).

---

## 7. Pre-Power Visual Inspection

- [ ] No bare wires touching each other
- [ ] BOTTOM [ + ] (6-12V) does NOT touch TOP [ + ] (3.3V)
- [ ] All VM pins go to BOTTOM [ + ] only (never to TOP [ + ])
- [ ] Capacitor polarity correct
- [ ] Motor wires seated firmly in breadboard or terminals
- [ ] No loose wires that could short circuit

---

## 8. Power-Up Sequence

Follow this exact order:

1. [ ] Verify all checks above are complete
2. [ ] Connect external power supply (6-12V) - motors NOT energized yet (STM32 off)
3. [ ] Measure BOTTOM [ + ] rail with multimeter: should read 6-12V
4. [ ] Measure TOP [ + ] rail with multimeter: should still read 0V (or 3.3V if isolated)
5. [ ] Connect USB to Black Pill (STM32 powers on)
6. [ ] Check PC13 LED blinks (system running)

---

## 9. First Motor Test - D-pad Button Control

Buttons control the robot as a whole (D-pad layout), speed = **70%**.
Hold button to move, release to stop all motors.

```text
D-pad layout:
         [BTN_0 = FORWARD]
[BTN_1 = LEFT]    [BTN_2 = RIGHT]
         [BTN_3 = BACKWARD]
```

### BTN_0 (PB3) - Forward

Hold BTN_0:

- [ ] All 4 motors spin (robot moves forward)
- [ ] LED_0 (PA5) turns ON
- [ ] Release BTN_0 → all motors stop, LED_0 off

Motor directions when moving forward:

```text
Motor 0 (Driver 1 A) → FORWARD
Motor 1 (Driver 1 B) → FORWARD
Motor 2 (Driver 2 A) → FORWARD
Motor 3 (Driver 2 B) → FORWARD
```

### BTN_3 (PC15) - Backward

Hold BTN_3:

- [ ] All 4 motors spin in reverse (robot moves backward)
- [ ] LED_3 (PA11) turns ON
- [ ] Release → stop

### BTN_1 (PB4) - Rotate Left (tank turn)

Hold BTN_1:

- [ ] Robot rotates left in place (no translation)
- [ ] LED_1 (PA6) turns ON
- [ ] Release → stop

Motor directions when rotating left:

```text
Motor 0 (L-front) → REVERSE
Motor 1 (L-rear)  → REVERSE
Motor 2 (R-front) → FORWARD
Motor 3 (R-rear)  → FORWARD
```

### BTN_2 (PC14) - Rotate Right (tank turn)

Hold BTN_2:

- [ ] Robot rotates right in place
- [ ] LED_2 (PA7) turns ON
- [ ] Release → stop

Motor directions when rotating right:

```text
Motor 0 (L-front) → FORWARD
Motor 1 (L-rear)  → FORWARD
Motor 2 (R-front) → REVERSE
Motor 3 (R-rear)  → REVERSE
```

### Fix Wrong Direction

If robot moves opposite to expected - swap motor output wires on the driver:

| Problem               | Fix                                              |
| --------------------- | ------------------------------------------------ |
| Forward goes backward | Swap A01/A02 on Driver 1 AND A01/A02 on Driver 2 |
| Left side wrong       | Swap A01/A02 on Driver 1 (Motor 0, left front)   |
| Right side wrong      | Swap A01/A02 on Driver 2 (Motor 2, right front)  |
| One motor backwards   | Swap its two output wires only                   |

---

## 10. Troubleshooting

### Motor does not spin

- [ ] Check VM connected to driver (measure VM pin → should be 6-12V)
- [ ] Check STBY pin HIGH (Driver 1: PB14, Driver 2: PA4)
- [ ] Check GND cross-connection (TOP [-] to BOTTOM [-])
- [ ] Check motor wires not loose

### Motor spins wrong direction

- [ ] Swap the two motor output wires (A01 ↔ A02 or B01 ↔ B02)

### Motor vibrates but does not rotate

- [ ] Increase PWM duty cycle (speed too low)
- [ ] Check motor power supply voltage (may be too low)

### Supply voltage drops when motor runs

- [ ] Add or increase capacitor on VM (use 1000 µF)
- [ ] Use power supply with higher current rating (≥ 2A per motor)

### STM32 resets when motor starts

- [ ] GND cross-connection missing or loose
- [ ] Add capacitor on VM line

---

## Result

**Date:** ___________

**VM Voltage measured:** _______ V

**All 4 motors tested:** ⬜ YES  ⬜ NO

**Issues found:**

```text
_____________________________________________
_____________________________________________
```

**Status:** ⬜ PASSED  ⬜ FAILED

---

**Version:** 1.0
**Last updated:** 2026-02-22
**Project:** STM32_Core - Black Pill + 2×TB6612FNG + 4 DC Motors
