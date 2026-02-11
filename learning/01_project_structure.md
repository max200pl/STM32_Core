# 📚 STM32 Проект - Пояснення для новачків

## 🎯 Що таке STM32?

**STM32** - це мікроконтролер (маленький комп'ютер на одній мікросхемі) від компанії STMicroelectronics.

### Ваша плата: BlackPill STM32F411CE
- **Процесор:** ARM Cortex-M4 @ 100 МГц
- **RAM:** 128 КБ (оперативна пам'ять)
- **Flash:** 512 КБ (пам'ять для програми)
- **Кварц:** 25 МГц (зовнішній генератор тактової частоти)

Це як Arduino, але набагато потужніше і складніше в налаштуванні.

---

## 📁 Структура проекту PlatformIO

```
STM32_Core/
├── src/                           # 📝 Ваш вихідний код
│   ├── main.c                     # Головна програма
│   ├── main.h                     # Заголовковий файл для main.c
│   ├── stm32f4xx_it.c             # Обробники переривань
│   └── (інші .c файли)
│
├── include/                       # 📋 Заголовкові файли проекту
│   ├── stm32f4xx_hal_conf.h       # Конфігурація HAL бібліотеки
│   ├── stm32f4xx_it.h             # Заголовки переривань
│   └── (інші .h файли)
│
├── lib/                           # 📚 Додаткові бібліотеки
│   └── (тут можна додавати свої бібліотеки)
│
├── test/                          # 🧪 Файли для тестування
│
├── .pio/                          # ⚙️ Службова папка PlatformIO
│   └── build/                     # Скомпільовані файли (.o, .elf, .bin)
│
├── platformio.ini                 # ⚙️ Конфігурація проекту
└── learning/                      # 📖 Навчальні матеріали
    └── 01_project_structure.md    # Цей файл
```

### Що де зберігається:
- **src/** - весь ваш C/C++ код
- **include/** - заголовкові файли (.h)
- **lib/** - сторонні бібліотеки
- **.pio/build/** - результат компіляції (не редагуємо вручну!)

---

## 🔧 Що таке HAL?

**HAL** = Hardware Abstraction Layer (шар абстракції від апаратного забезпечення)

Це бібліотека від STMicroelectronics, яка спрощує програмування STM32.

### 🚫 Без HAL (складно):
```c
// Треба знати точні адреси регістрів пам'яті
#define RCC_BASE   0x40023800
#define GPIOC_BASE 0x40020800

*((uint32_t*)(RCC_BASE + 0x30)) |= 0x04;  // Увімкнути GPIOC
*((uint32_t*)(GPIOC_BASE + 0x00)) |= (1 << 26); // Пін 13 на вихід
*((uint32_t*)(GPIOC_BASE + 0x14)) ^= (1 << 13); // Перемкнути пін 13
```

### ✅ З HAL (просто і зрозуміло):
```c
__HAL_RCC_GPIOC_CLK_ENABLE();           // Увімкнути тактування GPIOC
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_13;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // Налаштувати GPIO
HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // Перемкнути пін
```

**Переваги HAL:**
- ✅ Простіше читати і розуміти
- ✅ Менше помилок
- ✅ Легко переносити код між STM32
- ✅ Офіційна підтримка від ST

---

## 🔍 Проблема та її вирішення

### ❌ Що було спочатку:
```c
#include "main.h"  // ❌ Файл не існує!

int main(void) {
    HAL_Init();    // ❌ Компілятор не знає звідки ця функція
    ...
}
```

**Помилка компіляції:**
```
fatal error: main.h: No such file or directory
```

### 🔎 Причини помилки:
1. Файл `main.h` не створено
2. Немає підключення до HAL бібліотеки
3. Немає конфігурації HAL (`stm32f4xx_hal_conf.h`)
4. Немає обробників переривань

---

## 🛠️ Рішення: Створені файли

### 1️⃣ main.h - "Довідник" для main.c

**Розташування:** `src/main.h`

```c
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"  // Підключити HAL бібліотеку

void Error_Handler(void);   // Оголосити функцію обробки помилок

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
```

**Що робить:**
- `#ifndef/__define` - захист від подвійного включення
- `extern "C"` - сумісність з C++
- Підключає всю HAL бібліотеку STM32F4
- Оголошує функцію `Error_Handler()`

---

### 2️⃣ stm32f4xx_hal_conf.h - Конфігурація HAL

**Розташування:** `include/stm32f4xx_hal_conf.h`

```c
#define HAL_MODULE_ENABLED          // Увімкнути HAL
#define HAL_GPIO_MODULE_ENABLED     // Модуль GPIO (піни)
#define HAL_RCC_MODULE_ENABLED      // Модуль RCC (тактування)
#define HAL_PWR_MODULE_ENABLED      // Модуль PWR (живлення)
#define HAL_CORTEX_MODULE_ENABLED   // Модуль процесора
#define HAL_DMA_MODULE_ENABLED      // Модуль DMA
#define HAL_FLASH_MODULE_ENABLED    // Модуль Flash пам'яті

// Частота зовнішнього кварца
#define HSE_VALUE    25000000U      // 25 МГц на BlackPill

// Інші частоти
#define HSI_VALUE    16000000U      // Внутрішній RC генератор
#define LSI_VALUE    32000U         // Низькочастотний внутрішній
#define LSE_VALUE    32768U         // Низькочастотний зовнішній
```

**Навіщо:**
- HAL - дуже велика бібліотека (UART, SPI, I2C, ADC, USB...)
- Тут ми вибираємо тільки потрібні модулі
- Це прискорює компіляцію та зменшує розмір програми

---

### 3️⃣ stm32f4xx_it.c/.h - Обробники переривань

**Розташування:** `src/stm32f4xx_it.c` та `include/stm32f4xx_it.h`

#### Що таке переривання (Interrupt)?

**Переривання** - це коли щось терміново відбувається і процесор зупиняє основну програму:

```
Основна програма → ПЕРЕРИВАННЯ! → Обробник → Повернення до програми
     │                  ↑              │
     │                  │              │
     └─────────────────────────────────┘
```

**Приклади:**
- ⏰ Таймер спрацював → `SysTick_Handler()`
- 🔴 Кнопка натиснута → `EXTI0_IRQHandler()`
- 📡 Прийшли дані UART → `USART1_IRQHandler()`
- 💥 Критична помилка → `HardFault_Handler()`

#### Обов'язкові обробники ARM Cortex-M4:

```c
void SysTick_Handler(void) {
    HAL_IncTick();  // Кожну 1 мс збільшує лічильник
                    // Потрібно для HAL_Delay()
}

void HardFault_Handler(void) {
    while (1) {}    // Критична помилка - зависти
}

void NMI_Handler(void) {}           // Non-Maskable Interrupt
void MemManage_Handler(void) {}     // Помилка доступу до пам'яті
void BusFault_Handler(void) {}      // Помилка шини даних
void UsageFault_Handler(void) {}    // Помилка виконання інструкції
void SVC_Handler(void) {}           // Supervisor Call (RTOS)
void DebugMon_Handler(void) {}      // Debug Monitor
void PendSV_Handler(void) {}        // Pendable Service (RTOS)
```

**Без цих обробників проект не скомпілюється!**

---

### 4️⃣ SystemClock_Config() - Налаштування частоти

**Розташування:** `src/main.c`

#### Навіщо потрібно?

За замовчуванням STM32F411 працює на **16 МГц** (внутрішній RC генератор HSI).
Але він здатний на **100 МГц**! Нам потрібно його "розігнати".

#### Як це працює:

```
[Кварц 25 МГц] → [PLL множник] → [Ділильники] → [96 МГц процесор]
                                              ↓
                                         [48 МГц APB1]
                                         [96 МГц APB2]
```

#### Формула PLL:

```
f_CPU = (HSE_VALUE / PLLM) × PLLN / PLLP

f_CPU = (25 МГц / 25) × 192 / 2 = 96 МГц
```

**Параметри:**
- `PLLM = 25` - ділимо кварц: 25 МГц / 25 = 1 МГц
- `PLLN = 192` - множимо: 1 МГц × 192 = 192 МГц
- `PLLP = 2` - ділимо: 192 МГц / 2 = **96 МГц** 🎯
- `PLLQ = 4` - для USB: 192 МГц / 4 = 48 МГц

#### Шини процесора:

```
SYSCLK (96 МГц) → Процесор, Flash
    ↓
AHB (96 МГц)    → DMA, GPIO, тощо
    ↓
APB1 (48 МГц)   → UART, I2C, таймери (повільні периферійні пристрої)
APB2 (96 МГц)   → SPI, ADC (швидкі периферійні пристрої)
```

**Чому APB1 повільніше?**
Деякі модулі не працюють на 96 МГц, тому їх обмежують до 48 МГц.

---

### 5️⃣ Error_Handler() - Обробка помилок

**Розташування:** `src/main.c`

```c
void Error_Handler(void)
{
    __disable_irq();  // Вимкнути всі переривання
    while (1)         // Зависнути в нескінченному циклі
    {
        // Тут можна додати мигання LED для індикації помилки
    }
}
```

**Коли викликається:**
- Помилка налаштування тактування
- Помилка ініціалізації периферії
- Будь-яка критична помилка

**Що робити при помилці:**
1. Відкрити Debug режим
2. Подивитися де програма зависла
3. Перевірити налаштування (частоти, піни, тощо)

---

## 📝 Розбір main.c

### Повний код з коментарями:

```c
#include "main.h"  // Підключити HAL та оголошення

void SystemClock_Config(void);  // Прототип функції

int main(void)
{
    // 1️⃣ Ініціалізація HAL (налаштування SysTick, Flash, тощо)
    HAL_Init();

    // 2️⃣ Налаштування частоти процесора (16 МГц → 96 МГц)
    SystemClock_Config();

    // 3️⃣ Увімкнути тактування GPIO Port C
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // 4️⃣ Налаштувати пін PC13 (вбудований LED на BlackPill)
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // Структура налаштувань
    GPIO_InitStruct.Pin = GPIO_PIN_13;       // Пін 13
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Режим: вихід Push-Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;      // Без підтяжки
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Низька швидкість
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  // Застосувати налаштування

    // 5️⃣ Головний цикл (виконується безкінечно)
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // Перемкнути LED
        HAL_Delay(500);                         // Затримка 500 мс
    }
}
```

### Пояснення рядок за рядком:

#### `HAL_Init()`
- Ініціалізує HAL бібліотеку
- Налаштовує SysTick таймер (1 мс)
- Налаштовує Flash пам'ять
- **Обов'язково викликати першою!**

#### `SystemClock_Config()`
- Налаштовує PLL (Phase-Locked Loop)
- Переключає процесор з HSI (16 МГц) на PLL (96 МГц)
- Налаштовує ділильники шин (AHB, APB1, APB2)

#### `__HAL_RCC_GPIOC_CLK_ENABLE()`
- **RCC** = Reset and Clock Control (контролер тактування)
- Увімкнути тактування для GPIO Port C
- **Без цього GPIO не працює!** (економія енергії)

#### `GPIO_InitTypeDef`
Структура з налаштуваннями GPIO:
- `Pin` - який пін (0-15)
- `Mode` - режим роботи:
  - `GPIO_MODE_INPUT` - вхід
  - `GPIO_MODE_OUTPUT_PP` - вихід Push-Pull
  - `GPIO_MODE_OUTPUT_OD` - вихід Open-Drain
  - `GPIO_MODE_AF_PP` - альтернативна функція
- `Pull` - підтяжка:
  - `GPIO_NOPULL` - без підтяжки
  - `GPIO_PULLUP` - до +3.3V
  - `GPIO_PULLDOWN` - до GND
- `Speed` - швидкість перемикання:
  - `GPIO_SPEED_FREQ_LOW` - до 8 МГц
  - `GPIO_SPEED_FREQ_MEDIUM` - до 50 МГц
  - `GPIO_SPEED_FREQ_HIGH` - до 100 МГц

#### `HAL_GPIO_TogglePin()`
- Перемикає стан піна (0 → 1 або 1 → 0)
- Для LED: увімкнути ↔ вимкнути

#### `HAL_Delay(500)`
- Затримка 500 мілісекунд
- Використовує SysTick таймер
- **Не точна!** (для точних затримок використовуйте таймери)

---

## 🚀 Процес компіляції

### Етапи збірки проекту:

```
1️⃣ Препроцесор (Preprocessor)
   main.c → Обробка #include, #define → main.i

2️⃣ Компілятор (Compiler)
   main.i → Перевірка синтаксису → main.s (асемблер)

3️⃣ Асемблер (Assembler)
   main.s → Машинний код → main.o (об'єктний файл)

4️⃣ Лінкер (Linker)
   main.o + stm32f4xx_it.o + HAL_lib → firmware.elf

5️⃣ Конвертер (Objcopy)
   firmware.elf → firmware.bin (файл для прошивки)
```

### Результат збірки:

```
Building .pio/build/blackpill_f411ce/firmware.bin
RAM:   [          ]   0.0% (used 44 bytes from 131072 bytes)
Flash: [          ]   0.7% (used 3424 bytes from 524288 bytes)
========================= [SUCCESS] =========================
```

**Що означають цифри:**
- **RAM 44 bytes** - змінні в пам'яті (дуже мало!)
- **Flash 3424 bytes** - розмір програми (~3.3 КБ)
- Ще маємо **508 КБ** вільного місця! 🎉

---

## 🎓 Подальше навчання

### 1. Основи C
- [ ] Змінні та типи даних
- [ ] Функції
- [ ] Покажчики (pointer)
- [ ] Структури (struct)
- [ ] Препроцесор (#include, #define, #ifdef)

### 2. Основи STM32
- [ ] Архітектура ARM Cortex-M4
- [ ] Система тактування (RCC)
- [ ] GPIO (піни вводу/виводу)
- [ ] Переривання (NVIC)
- [ ] Таймери
- [ ] UART (послідовний порт)

### 3. HAL бібліотека
- [ ] Як працює HAL
- [ ] Ініціалізація периферії
- [ ] Callback функції
- [ ] DMA (прямий доступ до пам'яті)

### 4. Практичні проекти
- [ ] ✅ Блимання LED (готово!)
- [ ] Кнопка + переривання
- [ ] PWM (регулювання яскравості LED)
- [ ] UART (зв'язок з комп'ютером)
- [ ] ADC (читання датчиків)
- [ ] I2C/SPI (зв'язок з модулями)

---

## 📚 Корисні ресурси

### Офіційна документація ST:
1. **Reference Manual** - повний опис всіх регістрів
   - [STM32F411 RM0383](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xc-e-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)

2. **HAL User Manual** - документація по HAL функціях
   - [UM1725](https://www.st.com/resource/en/user_manual/um1725-description-of-stm32f4-hal-and-lowlayer-drivers-stmicroelectronics.pdf)

3. **Datasheet** - характеристики мікроконтролера
   - [STM32F411xC/xE](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)

### YouTube канали (українською/російською):
- **ITVDN** - курси по embedded
- **Omega Lab** - STM32 туторіали
- **Mikhail Baranov** - програмування STM32

### YouTube канали (англійською):
- **Controller Tech**
- **DigiKey**
- **Phil's Lab**

### Спільноти:
- [STM32 Forum](https://community.st.com/)
- Reddit: r/stm32
- Telegram: STM32 спільноти

---

## 💡 Поради новачкам

### ✅ DO (Робіть):
- Читайте документацію
- Експериментуйте з прикладами
- Використовуйте debugger
- Запитуйте в спільнотах

### ❌ DON'T (Не робіть):
- Не копіюйте код без розуміння
- Не ігноруйте попередження компілятора
- Не підключайте периферію без схеми
- Не використовуйте затримки замість таймерів (для серйозних проектів)

### 🔧 Інструменти:
- **PlatformIO** - середовище розробки
- **STM32CubeMX** - генератор коду (графічне налаштування)
- **ST-Link V2** - програматор/дебагер
- **Logic Analyzer** - аналіз сигналів

---

## ❓ Часті питання

### Q: Чому не використовувати Arduino?
**A:** STM32 потужніший (100 МГц vs 16 МГц), має більше периферії (UART, SPI, I2C, USB, ADC, DAC, таймери), але складніший у налаштуванні.

### Q: Що краще: HAL чи Register?
**A:** Для новачків - HAL. Для оптимізації та повного контролю - регістри.

### Q: Чому проект не завантажується на плату?
**A:** Перевірте:
- ST-Link підключений
- Драйвери встановлені
- Плата отримує живлення
- Правильно вибраний програматор в platformio.ini

### Q: Як дебажити програму?
**A:** PlatformIO має вбудований debugger. Натисніть F5 або Debug → Start Debugging.

---

**Успіхів у вивченні STM32! 🚀**

*Цей файл створено як частина навчального проекту для розуміння структури STM32 проектів на PlatformIO з HAL.*
