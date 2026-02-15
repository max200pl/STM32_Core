# 📦 Урок 05: Как работают импорты модулей в C

## 🎯 Цель урока

Понять, как организовать код в модули и подключать их в `main.c` для создания структурированного проекта.

---

## 📚 Основная концепция

В C нет "импортов" как в Python (`import module`). Вместо этого используется **препроцессор** с директивой `#include`.

### Два типа файлов

```
┌────────────────────────────────────────────────────────┐
│  МОДУЛЬ В C СОСТОИТ ИЗ ДВУХ ФАЙЛОВ                    │
└────────────────────────────────────────────────────────┘

┌─────────────────────┐         ┌─────────────────────┐
│  module.h           │         │  module.c           │
│  (Заголовок)        │         │  (Реализация)       │
├─────────────────────┤         ├─────────────────────┤
│ • Прототипы функций │         │ • Реальный код      │
│ • Структуры         │         │ • Реализация функций│
│ • typedef           │         │ • Приватные данные  │
│ • #define           │         │                     │
└─────────────────────┘         └─────────────────────┘
         ↓                               ↓
    "Интерфейс"                   "Имплементация"
```

---

## 🏗️ Структура нашего проекта

```
STM32_Core/
├── src/
│   ├── main.c                    ← Главный файл программы
│   ├── main.h                    ← Заголовок main
│   ├── stm32f4xx_it.c            ← Обработчики прерываний
│   │
│   └── drivers/                  ← Папка драйверов
│       ├── motor/                ← Драйверы моторов
│       │   ├── tb6612fng.h      ← Интерфейс драйвера
│       │   └── tb6612fng.c      ← Реализация драйвера
│       │
│       └── sensors/              ← Датчики
│           ├── encoder.h         ← Интерфейс энкодеров
│           └── encoder.c         ← Реализация энкодеров
│
└── include/                      ← Системные заголовки (HAL, CMSIS)
```

---

## 🔍 Как работает #include

### Препроцессор - это "копировщик"

```c
// main.c
#include "drivers/motor/tb6612fng.h"

int main(void) {
    TB6612FNG_Init(...);  // Функция из модуля
}
```

**Что делает препроцессор:**

```
Шаг 1: Читает main.c
Шаг 2: Видит #include "drivers/motor/tb6612fng.h"
Шаг 3: Открывает файл tb6612fng.h
Шаг 4: КОПИРУЕТ его содержимое в main.c
Шаг 5: Результат передаётся компилятору
```

**После препроцессора main.c выглядит так:**

```c
// ===== Вставлено из tb6612fng.h =====
#ifndef TB6612FNG_H
#define TB6612FNG_H

typedef enum {
    MOTOR_0 = 0,
    MOTOR_1 = 1
} Motor_ID;

void TB6612FNG_Init(...);
void TB6612FNG_Drive(...);
// ====================================

int main(void) {
    TB6612FNG_Init(...);  // Теперь компилятор знает про эту функцию!
}
```

---

## 📝 Защита от двойного включения

### Проблема: Двойное определение

```c
// main.c
#include "encoder.h"
#include "motor.h"    // motor.h тоже включает encoder.h!

// ❌ ОШИБКА: Encoder_ID определён дважды!
```

### Решение: Include Guards

```c
// encoder.h
#ifndef ENCODER_H         // Если ENCODER_H не определён
#define ENCODER_H         // Определить ENCODER_H

// ... код модуля ...

typedef enum {
    ENCODER_0 = 0
} Encoder_ID;

#endif /* ENCODER_H */   // Конец блока
```

**Как это работает:**

```
Первое включение:
1. ENCODER_H не определён → входим в блок
2. Определяем ENCODER_H
3. Читаем код модуля

Второе включение:
1. ENCODER_H уже определён → пропускаем весь блок
2. Ничего не делаем
```

---

## 📂 Типы путей в #include

### 1. Угловые скобки `< >`

```c
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
```

**Для чего:** Системные библиотеки, HAL

**Где ищет компилятор:**
```
/path/to/HAL/Inc/
/path/to/CMSIS/Include/
/usr/include/
```

### 2. Кавычки `" "`

```c
#include "main.h"
#include "drivers/motor/tb6612fng.h"
#include "drivers/sensors/encoder.h"
```

**Для чего:** Ваши собственные модули

**Где ищет компилятор:**
```
1. Сначала в текущей директории (src/)
2. Потом в директориях проекта
3. Потом в системных директориях
```

---

## 🔗 Процесс компиляции

```
┌─────────────────────────────────────────────────────────┐
│  1. ПРЕПРОЦЕССОР (Копирует файлы)                      │
└─────────────────────────────────────────────────────────┘
                        ↓
         main.c + включённые .h → main.i (обработанный файл)
         tb6612fng.c + включённые .h → tb6612fng.i
         encoder.c + включённые .h → encoder.i

┌─────────────────────────────────────────────────────────┐
│  2. КОМПИЛЯТОР (Создаёт объектные файлы)               │
└─────────────────────────────────────────────────────────┘
                        ↓
         main.i → main.o (машинный код)
         tb6612fng.i → tb6612fng.o
         encoder.i → encoder.o

┌─────────────────────────────────────────────────────────┐
│  3. ЛИНКОВЩИК (Объединяет всё в программу)             │
└─────────────────────────────────────────────────────────┘
                        ↓
         main.o + tb6612fng.o + encoder.o → firmware.elf
```

---

## 💡 Пример: Подключение модулей в main.c

### drivers/motor/tb6612fng.h

```c
#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "main.h"

// ============ ПУБЛИЧНЫЙ ИНТЕРФЕЙС ============

typedef enum {
    MOTOR_0 = 0,
    MOTOR_1 = 1,
    MOTOR_2 = 2,
    MOTOR_3 = 3
} Motor_ID;

typedef enum {
    MOTOR_FORWARD,
    MOTOR_REVERSE
} Motor_Direction;

// Прототипы функций (объявления)
void TB6612FNG_Init(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2,
                    TIM_HandleTypeDef *htim3, TIM_HandleTypeDef *htim4);
void TB6612FNG_Drive(Motor_ID motor, Motor_Direction dir, uint8_t speed);
void TB6612FNG_Stop(Motor_ID motor);
void TB6612FNG_MoveForward(uint8_t speed);

#endif /* TB6612FNG_H */
```

### drivers/motor/tb6612fng.c

```c
#include "drivers/motor/tb6612fng.h"

// ============ ПРИВАТНЫЕ ДАННЫЕ ============
// (Видны только внутри этого файла)

static TIM_HandleTypeDef *motor_timers[4];
static uint8_t motor_speeds[4] = {0, 0, 0, 0};

// ============ РЕАЛИЗАЦИЯ ФУНКЦИЙ ============

void TB6612FNG_Init(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2,
                    TIM_HandleTypeDef *htim3, TIM_HandleTypeDef *htim4) {
    motor_timers[0] = htim3;
    motor_timers[1] = htim4;
    motor_timers[2] = htim1;
    motor_timers[3] = htim2;

    // ... настройка GPIO и таймеров
}

void TB6612FNG_Drive(Motor_ID motor, Motor_Direction dir, uint8_t speed) {
    // ... код управления мотором
}

// ... остальные функции
```

### main.c - Подключение модулей

```c
#include "main.h"
#include <stdio.h>

// ============ ПОДКЛЮЧЕНИЕ НАШИХ МОДУЛЕЙ ============

#include "drivers/motor/tb6612fng.h"    // Драйвер моторов
#include "drivers/sensors/encoder.h"    // Датчики энкодеров

// ============ ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ============

TIM_HandleTypeDef htim1, htim2, htim3, htim4;

// ============ ГЛАВНАЯ ПРОГРАММА ============

int main(void) {
    HAL_Init();
    SystemClock_Config();

    // Инициализация таймеров
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    // ===== Используем функции из модулей =====

    // Из модуля drivers/motor/tb6612fng.h
    TB6612FNG_Init(&htim3, &htim4, &htim1, &htim2);
    TB6612FNG_EnableAll();

    // Из модуля drivers/sensors/encoder.h
    Encoder_Init();

    while (1) {
        Encoder_Update();

        float rpm = Encoder_GetRPM(ENCODER_0);

        if (rpm < 200) {
            TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 80);
        } else {
            TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 50);
        }

        HAL_Delay(100);
    }
}
```

---

## 🎨 Визуализация связей модулей

```
┌──────────────────────────────────────────────────────────┐
│                        main.c                             │
│                                                           │
│  #include "drivers/motor/tb6612fng.h"                    │
│  #include "drivers/sensors/encoder.h"                    │
│                                                           │
│  int main(void) {                                        │
│      TB6612FNG_Init();  ───────┐                         │
│      Encoder_Init();     ──────┼────┐                    │
│      TB6612FNG_Drive();  ───┐  │    │                    │
│      Encoder_GetRPM();   ───┼──┘    │                    │
│  }                          │       │                    │
└─────────────────────────────┼───────┼────────────────────┘
                              │       │
                 ┌────────────┘       └─────────────┐
                 ↓                                   ↓
┌────────────────────────────┐   ┌────────────────────────┐
│ drivers/motor/tb6612fng.c  │   │ drivers/sensors/       │
│                            │   │ encoder.c              │
│ void TB6612FNG_Init() {    │   │                        │
│     // Реализация          │   │ void Encoder_Init() {  │
│ }                          │   │     // Реализация      │
│                            │   │ }                      │
│ void TB6612FNG_Drive() {   │   │                        │
│     // Реализация          │   │ float Encoder_GetRPM() │
│ }                          │   │     // Реализация      │
└────────────────────────────┘   └────────────────────────┘
```

---

## ⚠️ Частые ошибки

### Ошибка 1: Забыли #include

```c
// main.c
// ЗАБЫЛИ: #include "drivers/motor/tb6612fng.h"

int main(void) {
    TB6612FNG_Init(...);  // ❌ ОШИБКА!
}
```

**Ошибка компилятора:**
```
error: implicit declaration of function 'TB6612FNG_Init'
```

**Решение:**
```c
#include "drivers/motor/tb6612fng.h"  // ✅ Добавить!
```

### Ошибка 2: Неправильный путь

```c
#include "tb6612fng.h"  // ❌ Файл в drivers/motor/!
```

**Ошибка компилятора:**
```
fatal error: 'tb6612fng.h' file not found
```

**Решение:**
```c
#include "drivers/motor/tb6612fng.h"  // ✅ Правильный путь
```

### Ошибка 3: Циклические зависимости

```c
// motor.h
#include "sensor.h"

// sensor.h
#include "motor.h"  // ❌ Цикл!
```

**Решение:** Используйте forward declarations

```c
// motor.h
typedef struct Sensor_Data Sensor_Data;  // Forward declaration

void Motor_UseSensor(Sensor_Data *data);
```

---

## 📊 Сравнение: Python vs C

| Аспект | Python | C |
|--------|--------|---|
| **Импорт** | `import module` | `#include "module.h"` |
| **Когда** | Во время выполнения | До компиляции (препроцессор) |
| **Что делает** | Загружает модуль | Копирует текст файла |
| **Файлов** | 1 файл (.py) | 2 файла (.h + .c) |
| **Защита** | Автоматическая | Ручная (#ifndef) |

---

## 🎓 Правила хорошей организации

### 1. Один модуль = Одна задача

```
✅ ХОРОШО:
drivers/motor/tb6612fng.h    - только моторы
drivers/sensors/encoder.h    - только энкодеры

❌ ПЛОХО:
drivers/everything.h         - всё в одном файле
```

### 2. Заголовки содержат только интерфейс

```c
// ✅ ХОРОШО - encoder.h
void Encoder_Init(void);
float Encoder_GetRPM(Encoder_ID id);

// ❌ ПЛОХО - encoder.h
void Encoder_Init(void) {
    // Реализация в .h файле!
}
```

### 3. Приватные данные в .c файле

```c
// encoder.c
static uint32_t pulse_count[4];  // ✅ static = приватная переменная

// encoder.h
uint32_t pulse_count[4];  // ❌ Глобальная переменная!
```

### 4. Используйте понятные имена папок

```
drivers/
├── motor/           ✅ Понятно
├── sensors/         ✅ Понятно
└── stuff/           ❌ Непонятно
```

---

## 🛠️ Практическое задание

### Задание 1: Создать новый модуль LED

Создайте модуль для управления LED:

```
src/drivers/led/
├── led.h
└── led.c
```

**led.h:**
```c
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
```

**led.c:**
```c
#include "drivers/led/led.h"

void LED_Init(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // ... GPIO init
}

void LED_On(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

// ... остальные функции
```

**main.c:**
```c
#include "drivers/led/led.h"

int main(void) {
    LED_Init();

    while (1) {
        LED_Toggle();
        HAL_Delay(500);
    }
}
```

### Задание 2: Добавить модуль кнопки

Создайте модуль для кнопки:
- `drivers/button/button.h`
- `drivers/button/button.c`

Функции:
- `Button_Init()`
- `Button_IsPressed()`

---

## 📚 Итого

✅ **Что вы узнали:**

1. `#include` копирует содержимое файла (препроцессор)
2. Модуль = `.h` (интерфейс) + `.c` (реализация)
3. `#ifndef` защищает от двойного включения
4. `" "` для своих файлов, `< >` для системных
5. Структура папок помогает организовать код

✅ **Структура проекта:**

```
src/
├── main.c
└── drivers/
    ├── motor/
    │   ├── tb6612fng.h
    │   └── tb6612fng.c
    └── sensors/
        ├── encoder.h
        └── encoder.c
```

✅ **В main.c подключаем модули:**

```c
#include "drivers/motor/tb6612fng.h"
#include "drivers/sensors/encoder.h"
```

---

**Следующий урок:** PID контроллер для стабилизации скорости моторов

*Урок создан 15.02.2026 для проекта STM32 Black Pill*
