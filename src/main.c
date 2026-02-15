/**
 * @file    main.c
 * @brief   Main application - Robot with motors and encoders
 * @author  STM32 Black Pill Project
 * @date    2026-02-15
 */

// ============================================================================
// ПОДКЛЮЧЕНИЕ МОДУЛЕЙ
// ============================================================================

// Системные библиотеки STM32
#include "main.h"
#include <stdio.h>

// Драйверы моторов
#include "drivers/motor/tb6612fng.h"

// Датчики (закомментировано - нет физических датчиков)
// #include "drivers/sensors/encoder.h"

// ============================================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ (Таймеры для PWM)
// ============================================================================

TIM_HandleTypeDef htim1; // Motor 2 (PWM на PA8)
TIM_HandleTypeDef htim2; // Motor 3 (PWM на PA15)
TIM_HandleTypeDef htim3; // Motor 0 (PWM на PB0)
TIM_HandleTypeDef htim4; // Motor 1 (PWM на PB7)

// ============================================================================
// ПРОТОТИПЫ ФУНКЦИЙ
// ============================================================================

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
void Error_Handler(void);

// ============================================================================
// ГЛАВНАЯ ПРОГРАММА
// ============================================================================

/**
 * @brief Главная функция
 */
int main(void)
{
    // ------------------------------------------------------------------------
    // 1. Инициализация HAL и системной частоты
    // ------------------------------------------------------------------------
    HAL_Init();
    SystemClock_Config();

    // ------------------------------------------------------------------------
    // 2. Инициализация GPIO
    // ------------------------------------------------------------------------
    MX_GPIO_Init();

    // ------------------------------------------------------------------------
    // 3. Инициализация таймеров для PWM моторов
    // ------------------------------------------------------------------------
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();

    // ------------------------------------------------------------------------
    // 4. Инициализация драйверов и датчиков
    // ------------------------------------------------------------------------

    // Инициализация драйвера моторов TB6612FNG
    TB6612FNG_Init(&htim3, &htim4, &htim1, &htim2);
    TB6612FNG_EnableAll();

    // Инициализация энкодеров (закомментировано - нет физических датчиков)
    // Encoder_Init();

    // LED для индикации (PC13)
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef led = {0};
    led.Pin = GPIO_PIN_13;
    led.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOC, &led);

    // ------------------------------------------------------------------------
    // 5. Главный цикл программы
    // ------------------------------------------------------------------------

    // ========== КОД ЭНКОДЕРОВ ЗАКОММЕНТИРОВАН (нет физических датчиков) ==========
    /*
    uint32_t last_encoder_update = 0;

    while (1) {
        // Обновление энкодеров каждые 100 мс
        if (HAL_GetTick() - last_encoder_update >= 100) {
            last_encoder_update = HAL_GetTick();

            // Обновить расчёт RPM
            Encoder_Update();

            // Прочитать скорость моторов
            float rpm0 = Encoder_GetRPM(ENCODER_0);
            float rpm1 = Encoder_GetRPM(ENCODER_1);
            float rpm2 = Encoder_GetRPM(ENCODER_2);
            float rpm3 = Encoder_GetRPM(ENCODER_3);

            // Моргать LED если моторы вращаются
            if (rpm0 > 10 || rpm1 > 10 || rpm2 > 10 || rpm3 > 10) {
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            }

            // Пример: Простой контроль скорости Motor 0
            float target_rpm = 300.0f;
            if (rpm0 < target_rpm - 20) {
                TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 80);
            } else if (rpm0 > target_rpm + 20) {
                TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 60);
            }
        }
    }
    */

    // ========== ТЕСТОВЫЙ РЕЖИМ (пока нет энкодеров) ==========
    while (1)
    {
        // LED мигает для индикации работы
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        // Раскомментируйте для тестирования моторов:

        // Тест 1: Все моторы вперёд на 2 секунды
        // TB6612FNG_MoveForward(50);
        // HAL_Delay(2000);
        // TB6612FNG_StopAll();
        // HAL_Delay(1000);

        // Тест 2: Все моторы назад на 2 секунды
        // TB6612FNG_MoveBackward(50);
        // HAL_Delay(2000);
        // TB6612FNG_StopAll();
        // HAL_Delay(1000);

        // Тест 3: Поворот влево
        // TB6612FNG_TurnLeft(60, 50);
        // HAL_Delay(1500);
        // TB6612FNG_StopAll();
        // HAL_Delay(1000);

        // Тест 4: Один мотор вперёд
        // TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 70);
        // HAL_Delay(2000);
        // TB6612FNG_Stop(MOTOR_0);

        HAL_Delay(500); // Мигание LED каждые 500мс
    }
}

// ============================================================================
// СИСТЕМНЫЕ ФУНКЦИИ
// ============================================================================

/**
 * @brief Настройка системной частоты (96 MHz от HSE 25 MHz)
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;            // 25 MHz / 25 = 1 MHz
    RCC_OscInitStruct.PLL.PLLN = 192;           // 1 MHz * 192 = 192 MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // 192 / 2 = 96 MHz
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // 96 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // 48 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 96 MHz
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Инициализация GPIO
 */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
}

// ============================================================================
// ИНИЦИАЛИЗАЦИЯ ТАЙМЕРОВ ДЛЯ PWM
// ============================================================================

/**
 * @brief TIM1 Init (Motor 2 PWM - PA8)
 */
static void MX_TIM1_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 96 - 1; // 96 MHz / 96 = 1 MHz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000 - 1; // 1 MHz / 1000 = 1 kHz PWM
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM2 Init (Motor 3 PWM - PA15)
 */
static void MX_TIM2_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 96 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM3 Init (Motor 0 PWM - PB0)
 */
static void MX_TIM3_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 96 - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000 - 1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM4 Init (Motor 1 PWM - PB7)
 */
static void MX_TIM4_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 96 - 1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000 - 1;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief HAL MSP Init для таймеров (настройка GPIO для PWM)
 *  PWM это
 *   - P - (Pulse) - ШИМ сигнал, который управляет скоростью мотора. Для каждого таймера мы настраиваем соответствующий GPIO пин в альтернативной функции для генерации PWM сигнала.
 *   - W - (Width) - Ширина импульса, которая определяется значением в регистре CCRx. Чем больше значение, тем длиннее импульс и выше скорость мотора.
 *   - M - (Mode) - Режим работы таймера. Мы используем режим PWM1, который генерирует высокий уровень сигнала, когда счётчик меньше значения в CCRx, и низкий уровень, когда счётчик больше или равен этому значению.
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (tim_pwmHandle->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (tim_pwmHandle->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (tim_pwmHandle->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (tim_pwmHandle->Instance == TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * @brief Обработчик ошибок
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        // Быстро моргать LED при ошибке
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (volatile uint32_t i = 0; i < 100000; i++)
            ;
    }
}
