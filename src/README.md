# 📁 Структура проекта STM32_Core

## 🗂️ Организация кода

```
src/
├── main.c                      # Главная программа
├── main.h                      # Заголовок main
├── stm32f4xx_it.c              # Обработчики прерываний
│
└── drivers/                    # Папка драйверов
    ├── motor/                  # Драйверы моторов
    │   ├── tb6612fng.h        # Интерфейс TB6612FNG
    │   └── tb6612fng.c        # Реализация TB6612FNG
    │
    └── sensors/                # Датчики
        ├── encoder.h           # Интерфейс энкодеров
        └── encoder.c           # Реализация энкодеров
```

---

## 📦 Подключение модулей в main.c

```c
// Системные библиотеки
#include "main.h"
#include <stdio.h>

// Драйверы моторов
#include "drivers/motor/tb6612fng.h"

// Датчики
#include "drivers/sensors/encoder.h"
```

---

## 🔧 Доступные модули

### 1. **TB6612FNG Motor Driver**

**Путь:** `drivers/motor/tb6612fng.h`

**Описание:** Драйвер для управления 4 DC моторами через 2 модуля TB6612FNG

**Основные функции:**
```c
TB6612FNG_Init(&htim3, &htim4, &htim1, &htim2);  // Инициализация
TB6612FNG_EnableAll();                           // Включить все драйверы
TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 70);    // Управление мотором
TB6612FNG_MoveForward(80);                       // Все моторы вперёд
TB6612FNG_TurnLeft(70, 50);                      // Поворот ліворуч
TB6612FNG_StopAll();                             // Остановить все
```

**Константы:**
- `MOTOR_0`, `MOTOR_1`, `MOTOR_2`, `MOTOR_3` - ID моторов
- `MOTOR_FORWARD`, `MOTOR_REVERSE` - Направление

**Подключение:**
```
Driver 1 (Моторы 0, 1):
- PWMA → PB0 (TIM3_CH3), AIN1 → PB1, AIN2 → PB10, STBY → PB11
- PWMB → PB7 (TIM4_CH2), BIN1 → PB12, BIN2 → PB13

Driver 2 (Моторы 2, 3):
- PWMA → PA8 (TIM1_CH1), AIN1 → PA0, AIN2 → PA1, STBY → PA4
- PWMB → PA15 (TIM2_CH1), BIN1 → PA2, BIN2 → PA3
```

---

### 2. **Optical Encoder**

**Путь:** `drivers/sensors/encoder.h`

**Описание:** Драйвер для чтения оптических энкодеров (измерение RPM)

**Основные функции:**
```c
Encoder_Init();                      // Инициализация энкодеров
Encoder_Update();                    // Обновить расчёт RPM (каждые 100мс)
float rpm = Encoder_GetRPM(ENCODER_0);  // Получить RPM
uint32_t count = Encoder_GetCount(ENCODER_0);  // Общий счётчик импульсов
Encoder_ResetCount(ENCODER_0);       // Сбросить счётчик
```

**Константы:**
- `ENCODER_0`, `ENCODER_1`, `ENCODER_2`, `ENCODER_3` - ID энкодеров
- `ENCODER_SLOTS_PER_REV = 20` - Прорезов на диске

**Подключение:**
```
ENC0 → PB6 (EXTI6, Motor 0)
ENC1 → PB7 (EXTI7, Motor 1)
ENC2 → PA2 (EXTI2, Motor 2)
ENC3 → PA3 (EXTI3, Motor 3)
```

---

## 🚀 Пример использования

### Минимальный пример (LED мигание)

```c
#include "main.h"

int main(void) {
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef led = {0};
    led.Pin = GPIO_PIN_13;
    led.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOC, &led);

    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    }
}
```

### Полный пример (Моторы + Энкодеры)

```c
#include "main.h"
#include "drivers/motor/tb6612fng.h"
#include "drivers/sensors/encoder.h"

TIM_HandleTypeDef htim1, htim2, htim3, htim4;

int main(void) {
    HAL_Init();
    SystemClock_Config();

    // Инициализация таймеров
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    // Инициализация драйверов
    TB6612FNG_Init(&htim3, &htim4, &htim1, &htim2);
    TB6612FNG_EnableAll();
    Encoder_Init();

    uint32_t last_update = 0;

    while (1) {
        // Обновление каждые 100мс
        if (HAL_GetTick() - last_update >= 100) {
            last_update = HAL_GetTick();

            Encoder_Update();
            float rpm = Encoder_GetRPM(ENCODER_0);

            // Простой контроль скорости
            if (rpm < 200) {
                TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 80);
            } else {
                TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 50);
            }
        }
    }
}
```

---

## 📚 Документация

Подробная документация в папке `learning/`:

- [01_project_structure.md](../learning/01_project_structure.md) - Структура проекта
- [03_device_overview_and_connection.md](../learning/03_device_overview_and_connection.md) - Black Pill обзор
- [04_optical_encoders_rpm_measurement.md](../learning/04_optical_encoders_rpm_measurement.md) - Оптические энкодеры
- [05_module_imports_c_language.md](../learning/05_module_imports_c_language.md) - Импорты модулей в C
- [TB6612FNG_Dual_Connection_Guide.md](../learning/TB6612FNG_Dual_Connection_Guide.md) - Подключение TB6612FNG

---

## 🔧 Компиляция и загрузка

### Сборка проекта

```bash
cd /Users/maksym_poskannyi/Documents/PlatformIO/Projects/STM32_Core
platformio run
```

### Загрузка на плату

```bash
platformio run --target upload
```

### Отладка

```bash
platformio debug
```

Или в VS Code: **F5**

---

## ⚠️ Важные замечания

1. **Не используйте PA13, PA14** - это пины SWD (отладка)
2. **Моторы требуют внешнего питания** - не от USB! (6-12V на VM)
3. **Обязательно соедините GND** Black Pill и внешнего питания
4. **LED на PC13 инвертирован** - LOW = включён, HIGH = выключен
5. **Энкодеры используют прерывания** - не блокируйте их долгими операциями

---

**Автор:** STM32 Black Pill Project
**Дата:** 15.02.2026
