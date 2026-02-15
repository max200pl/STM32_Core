# TB6612FNG Motor Driver Datasheet

## Overview

Модуль драйвера керування двома колекторними двигунами на мікросхемі TB6612FNG.

### Key Features
- **Chip**: TB6612FNG
- **Motors**: 2 DC motors (independent control)
- **Motor Voltage**: 2.5V - 15V (VM)
- **Logic Voltage**: 2.7V - 5.5V (VCC)
- **Current**: 1.2A continuous / 3.2A peak per channel
- **PWM Frequency**: up to 100 kHz
- **Control**: H-bridge with forward, reverse, brake, stop

## Pin Description

### Input Pins (Connect to Black Pill)
| Pin   | Function | Description |
|-------|----------|-------------|
| PWMA  | PWM A    | Speed control for Motor A (0-100% duty cycle) |
| AIN1  | Input A1 | Direction control for Motor A |
| AIN2  | Input A2 | Direction control for Motor A |
| STBY  | Standby  | Enable/disable driver (LOW=standby, HIGH=active) |
| BIN1  | Input B1 | Direction control for Motor B |
| BIN2  | Input B2 | Direction control for Motor B |
| PWMB  | PWM B    | Speed control for Motor B (0-100% duty cycle) |
| GND   | Ground   | Logic ground |

### Output Pins (Connect to Motors and Power)
| Pin   | Function | Description |
|-------|----------|-------------|
| VM    | Motor Power | Motor supply voltage (2.5V - 15V) |
| VCC   | Logic Power | Logic supply voltage (2.7V - 5.5V, connect to 3V3) |
| GND   | Ground   | Power ground |
| A01   | Motor A+ | Motor A terminal 1 |
| A02   | Motor A- | Motor A terminal 2 |
| B01   | Motor B+ | Motor B terminal 1 |
| B02   | Motor B- | Motor B terminal 2 |

## Logic Operation Table

### Single Motor Control (A or B)

| IN1 | IN2 | PWM | STBY | Motor Action |
|-----|-----|-----|------|--------------|
| H   | H   | H/L | H    | Short brake  |
| L   | H   | H   | H    | CCW (reverse)|
| L   | H   | L   | H    | Short brake  |
| H   | L   | H   | H    | CW (forward) |
| H   | L   | L   | H    | Short brake  |
| L   | L   | H   | H    | Stop         |
| H/L | H/L | H/L | L    | Standby      |

**Legend:**
- H = HIGH (логічна 1)
- L = LOW (логічна 0)
- CW = Clockwise (за годинниковою стрілкою)
- CCW = Counter-Clockwise (проти годинникової стрілки)

## Connection to Black Pill

### Recommended Pinout

| TB6612FNG | Black Pill | Function |
|-----------|------------|----------|
| PWMA      | PB0 (TIM3_CH3) | PWM for Motor A speed |
| AIN1      | PB1        | Motor A direction 1 |
| AIN2      | PB10       | Motor A direction 2 |
| STBY      | PB11       | Standby control |
| BIN1      | PB12       | Motor B direction 1 |
| BIN2      | PB13       | Motor B direction 2 |
| PWMB      | PB7 (TIM4_CH2) | PWM for Motor B speed |
| VCC       | 3V3        | Logic power |
| GND       | GND        | Ground |
| VM        | 6-12V      | External motor power supply |

### Connection Diagram

```
┌─────────────────────┐
│   TB6612FNG Module  │
│                     │
│ PWMA  AIN1 AIN2     │     ┌──────────┐
│  │     │    │       │     │ Motor A  │
│  │     │    │  A01──┼─────┤  +    -  │
│  │     │    │  A02──┼─────┤  -    +  │
│  │     │    │       │     └──────────┘
│  │     │    │       │
│  │     │    │  STBY │
│  │     │    │   │   │
│  │     │    │   │   │     ┌──────────┐
│  │     │    │   │   │     │ Motor B  │
│  │     │    │   │  B01────┤  +    -  │
│  │     │    │   │  B02────┤  -    +  │
│  │     │    │   │   │     └──────────┘
│  │     │    │   │   │       │
│ PWMB BIN1 BIN2     │       │
└──┬────┬───┬───┬────┼───┬───┤
   │    │   │   │ VCC│GND│VM │
   │    │   │   │    │   │   │
   │    │   │   │    │   │  [6-12V]
   │    │   │   │    │   │  External
   PB7 PB12 PB13 PB11 GND│  Power
   │    │   │   │    │   │
   └────┴───┴───┴────┴───┘
        Black Pill
```

## PWM Configuration for STM32

### TIM3_CH3 (PB0) for PWMA
- Timer: TIM3
- Channel: CH3
- Pin: PB0
- Frequency: 1-20 kHz recommended

### TIM4_CH2 (PB7) for PWMB
- Timer: TIM4
- Channel: CH2
- Pin: PB7
- Frequency: 1-20 kHz recommended

## Code Implementation

### Header File: `tb6612fng.h`

```c
#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "main.h"

// Motor A pins
#define MOTOR_A_IN1_PORT    GPIOB
#define MOTOR_A_IN1_PIN     GPIO_PIN_1
#define MOTOR_A_IN2_PORT    GPIOB
#define MOTOR_A_IN2_PIN     GPIO_PIN_10

// Motor B pins
#define MOTOR_B_IN1_PORT    GPIOB
#define MOTOR_B_IN1_PIN     GPIO_PIN_12
#define MOTOR_B_IN2_PORT    GPIOB
#define MOTOR_B_IN2_PIN     GPIO_PIN_13

// Standby pin
#define MOTOR_STBY_PORT     GPIOB
#define MOTOR_STBY_PIN      GPIO_PIN_11

// Motor identifiers
typedef enum {
    MOTOR_A = 0,
    MOTOR_B = 1
} Motor_TypeDef;

// Motor directions
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1,
    MOTOR_REVERSE = 2,
    MOTOR_BRAKE = 3
} Motor_Direction;

// Function prototypes
void TB6612FNG_Init(TIM_HandleTypeDef *htim_a, TIM_HandleTypeDef *htim_b);
void TB6612FNG_Enable(void);
void TB6612FNG_Disable(void);
void TB6612FNG_SetSpeed(Motor_TypeDef motor, uint8_t speed);
void TB6612FNG_SetDirection(Motor_TypeDef motor, Motor_Direction direction);
void TB6612FNG_Drive(Motor_TypeDef motor, Motor_Direction direction, uint8_t speed);
void TB6612FNG_Stop(Motor_TypeDef motor);
void TB6612FNG_StopAll(void);

#endif
```

### Source File: `tb6612fng.c`

```c
#include "tb6612fng.h"

static TIM_HandleTypeDef *htim_motor_a = NULL;
static TIM_HandleTypeDef *htim_motor_b = NULL;

void TB6612FNG_Init(TIM_HandleTypeDef *htim_a, TIM_HandleTypeDef *htim_b) {
    htim_motor_a = htim_a;
    htim_motor_b = htim_b;

    // Enable GPIO clocks
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure direction pins
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Motor A
    GPIO_InitStruct.Pin = MOTOR_A_IN1_PIN;
    HAL_GPIO_Init(MOTOR_A_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_A_IN2_PIN;
    HAL_GPIO_Init(MOTOR_A_IN2_PORT, &GPIO_InitStruct);

    // Motor B
    GPIO_InitStruct.Pin = MOTOR_B_IN1_PIN;
    HAL_GPIO_Init(MOTOR_B_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_B_IN2_PIN;
    HAL_GPIO_Init(MOTOR_B_IN2_PORT, &GPIO_InitStruct);

    // Standby
    GPIO_InitStruct.Pin = MOTOR_STBY_PIN;
    HAL_GPIO_Init(MOTOR_STBY_PORT, &GPIO_InitStruct);

    // Start PWM
    HAL_TIM_PWM_Start(htim_a, TIM_CHANNEL_3);  // PB0
    HAL_TIM_PWM_Start(htim_b, TIM_CHANNEL_2);  // PB7

    // Initial state: disabled, stopped
    TB6612FNG_Disable();
    TB6612FNG_StopAll();
}

void TB6612FNG_Enable(void) {
    HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN, GPIO_PIN_SET);
}

void TB6612FNG_Disable(void) {
    HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN, GPIO_PIN_RESET);
}

void TB6612FNG_SetSpeed(Motor_TypeDef motor, uint8_t speed) {
    // Speed: 0-100%
    if (speed > 100) speed = 100;

    uint32_t pulse = (speed * htim_motor_a->Init.Period) / 100;

    if (motor == MOTOR_A) {
        __HAL_TIM_SET_COMPARE(htim_motor_a, TIM_CHANNEL_3, pulse);
    } else {
        __HAL_TIM_SET_COMPARE(htim_motor_b, TIM_CHANNEL_2, pulse);
    }
}

void TB6612FNG_SetDirection(Motor_TypeDef motor, Motor_Direction direction) {
    GPIO_TypeDef *in1_port, *in2_port;
    uint16_t in1_pin, in2_pin;

    if (motor == MOTOR_A) {
        in1_port = MOTOR_A_IN1_PORT;
        in1_pin = MOTOR_A_IN1_PIN;
        in2_port = MOTOR_A_IN2_PORT;
        in2_pin = MOTOR_A_IN2_PIN;
    } else {
        in1_port = MOTOR_B_IN1_PORT;
        in1_pin = MOTOR_B_IN1_PIN;
        in2_port = MOTOR_B_IN2_PORT;
        in2_pin = MOTOR_B_IN2_PIN;
    }

    switch (direction) {
        case MOTOR_FORWARD:
            HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
            break;
        case MOTOR_REVERSE:
            HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
            break;
        case MOTOR_BRAKE:
            HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
            break;
        case MOTOR_STOP:
        default:
            HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
            break;
    }
}

void TB6612FNG_Drive(Motor_TypeDef motor, Motor_Direction direction, uint8_t speed) {
    TB6612FNG_SetDirection(motor, direction);
    TB6612FNG_SetSpeed(motor, speed);
}

void TB6612FNG_Stop(Motor_TypeDef motor) {
    TB6612FNG_SetDirection(motor, MOTOR_STOP);
    TB6612FNG_SetSpeed(motor, 0);
}

void TB6612FNG_StopAll(void) {
    TB6612FNG_Stop(MOTOR_A);
    TB6612FNG_Stop(MOTOR_B);
}
```

## Usage Example: `main.c`

```c
#include "main.h"
#include "tb6612fng.h"

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    // Initialize motor driver
    TB6612FNG_Init(&htim3, &htim4);
    TB6612FNG_Enable();

    while (1) {
        // Motor A forward at 70% speed
        TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 70);
        HAL_Delay(2000);

        // Motor A reverse at 50% speed
        TB6612FNG_Drive(MOTOR_A, MOTOR_REVERSE, 50);
        HAL_Delay(2000);

        // Brake Motor A
        TB6612FNG_SetDirection(MOTOR_A, MOTOR_BRAKE);
        HAL_Delay(500);

        // Both motors forward at different speeds
        TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 80);
        TB6612FNG_Drive(MOTOR_B, MOTOR_FORWARD, 80);
        HAL_Delay(2000);

        // Stop all
        TB6612FNG_StopAll();
        HAL_Delay(1000);

        // Rotate in place (tank turn)
        TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 60);
        TB6612FNG_Drive(MOTOR_B, MOTOR_REVERSE, 60);
        HAL_Delay(1000);

        // Stop all
        TB6612FNG_StopAll();
        HAL_Delay(2000);
    }
}

static void MX_TIM3_Init(void) {
    // Configure TIM3 for PWM on CH3 (PB0)
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 96 - 1;         // 96 MHz / 96 = 1 MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000 - 1;          // 1 MHz / 1000 = 1 kHz PWM
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);
}

static void MX_TIM4_Init(void) {
    // Configure TIM4 for PWM on CH2 (PB7)
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 96 - 1;         // 96 MHz / 96 = 1 MHz
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000 - 1;          // 1 MHz / 1000 = 1 kHz PWM
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim4);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2);
}
```

## Motion Patterns

### Forward
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 80);
TB6612FNG_Drive(MOTOR_B, MOTOR_FORWARD, 80);
```

### Reverse
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_REVERSE, 80);
TB6612FNG_Drive(MOTOR_B, MOTOR_REVERSE, 80);
```

### Turn Left (pivot)
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 50);
TB6612FNG_Drive(MOTOR_B, MOTOR_FORWARD, 100);
```

### Turn Right (pivot)
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 100);
TB6612FNG_Drive(MOTOR_B, MOTOR_FORWARD, 50);
```

### Rotate Left (tank turn)
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_REVERSE, 70);
TB6612FNG_Drive(MOTOR_B, MOTOR_FORWARD, 70);
```

### Rotate Right (tank turn)
```c
TB6612FNG_Drive(MOTOR_A, MOTOR_FORWARD, 70);
TB6612FNG_Drive(MOTOR_B, MOTOR_REVERSE, 70);
```

### Emergency Stop
```c
TB6612FNG_SetDirection(MOTOR_A, MOTOR_BRAKE);
TB6612FNG_SetDirection(MOTOR_B, MOTOR_BRAKE);
```

## PWM Frequency Calculation

**Formula:**
```
PWM Frequency = Timer Clock / (Prescaler × Period)
```

**Example (1 kHz):**
```c
Timer Clock: 96 MHz
Prescaler: 96    → 96 MHz / 96 = 1 MHz
Period: 1000     → 1 MHz / 1000 = 1 kHz
```

**Example (10 kHz):**
```c
Timer Clock: 96 MHz
Prescaler: 96    → 1 MHz
Period: 100      → 1 MHz / 100 = 10 kHz
```

## Important Notes

⚠️ **Power Supply:**
- Logic voltage (VCC): 3.3V from Black Pill
- Motor voltage (VM): 6-12V from external power supply
- **ВАЖЛИВО**: Use separate power supply for motors, NOT from USB or Black Pill
- Connect GND of Black Pill and external power supply together

⚡ **Current Protection:**
- Maximum continuous: 1.2A per channel
- Peak current: 3.2A (short duration)
- Use appropriate motors for this current limit

🔄 **Standby (STBY):**
- LOW = driver disabled (saves power)
- HIGH = driver active
- Must be HIGH for motors to work

⏱️ **PWM Frequency:**
- Recommended: 1-20 kHz
- Lower frequency: audible noise, but more torque at low speeds
- Higher frequency: quieter, less torque at low speeds

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Motors don't work | Check STBY pin is HIGH, verify VM power supply |
| Motors run weakly | Check VM voltage (6-12V), verify motor current rating |
| Motors run intermittently | Check current limit (1.2A), add bigger capacitor to VM |
| PWM doesn't work | Verify timer configuration, check PB0/PB7 AF mapping |
| One motor works, other doesn't | Check wiring, verify both timers initialized |
| Motor runs one direction only | Check IN1/IN2 connections, verify GPIO configuration |
| Module overheats | Reduce PWM duty cycle, check motor stall condition |

## Next Steps

- Add speed ramping (smooth acceleration/deceleration)
- Implement PID control for precise speed
- Add encoder feedback for position control
- Create robot movement library (forward, turn, arc)
- Add current sensing for motor protection

## References

- TB6612FNG Datasheet: [Toshiba](https://toshiba.semicon-storage.com/info/docget.jsp?did=10660)
- STM32F4 Timer Reference: See STM32F411 Reference Manual
- PWM Configuration: See [01_project_structure.md](01_project_structure.md)
