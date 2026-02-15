/**
 * @file    encoder.c
 * @brief   Optical encoder implementation
 * @author  STM32 Black Pill Project
 * @date    2026-02-15
 */

#include "encoder.h"

// ============================================================================
// ПРИВАТНЫЕ ПЕРЕМЕННЫЕ
// ============================================================================

// Счётчики импульсов (инкрементируются в прерываниях)
static volatile uint32_t pulse_count[ENCODER_COUNT] = {0, 0, 0, 0};
static volatile uint32_t last_count[ENCODER_COUNT] = {0, 0, 0, 0};
static volatile uint32_t last_time[ENCODER_COUNT] = {0, 0, 0, 0};
static float rpm[ENCODER_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f};

// ============================================================================
// ПУБЛИЧНЫЕ ФУНКЦИИ
// ============================================================================

/**
 * @brief Инициализация всех энкодеров
 */
void Encoder_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Включить тактирование GPIO
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Настройка GPIO как входа с прерыванием по нарастающему фронту
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    // Encoder 0 - PB6
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Encoder 1 - PB7
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Encoder 2 - PA2
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Encoder 3 - PA3
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Включить прерывания EXTI
    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief Сбросить счётчик импульсов
 */
void Encoder_ResetCount(Encoder_ID encoder) {
    if (encoder < ENCODER_COUNT) {
        pulse_count[encoder] = 0;
        last_count[encoder] = 0;
        rpm[encoder] = 0.0f;
    }
}

/**
 * @brief Получить общее количество импульсов
 */
uint32_t Encoder_GetCount(Encoder_ID encoder) {
    if (encoder < ENCODER_COUNT) {
        return pulse_count[encoder];
    }
    return 0;
}

/**
 * @brief Получить текущий RPM
 */
float Encoder_GetRPM(Encoder_ID encoder) {
    if (encoder < ENCODER_COUNT) {
        return rpm[encoder];
    }
    return 0.0f;
}

/**
 * @brief Обновить расчёт RPM (вызывать каждые 100мс)
 */
void Encoder_Update(void) {
    uint32_t current_time = HAL_GetTick();

    for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
        // Количество импульсов за промежуток времени
        uint32_t pulses = pulse_count[i] - last_count[i];
        uint32_t time_diff = current_time - last_time[i];

        if (time_diff > 0) {
            // RPM = (импульсы * 60000 мс) / (время_мс * прорезы_на_оберт)
            rpm[i] = (pulses * 60000.0f) / (time_diff * ENCODER_SLOTS_PER_REV);
        }

        // Если долго нет импульсов - мотор остановился
        if (time_diff > 500) {  // 500мс без импульсов
            rpm[i] = 0.0f;
        }

        last_count[i] = pulse_count[i];
        last_time[i] = current_time;
    }
}

// ============================================================================
// ОБРАБОТЧИКИ ПРЕРЫВАНИЙ
// ============================================================================

/**
 * @brief Обработчик прерывания для PB6 и PB7 (EXTI9_5)
 */
void EXTI9_5_IRQHandler(void) {
    // Encoder 0 - PB6
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
        pulse_count[ENCODER_0]++;
    }

    // Encoder 1 - PB7
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
        pulse_count[ENCODER_1]++;
    }
}

/**
 * @brief Обработчик прерывания для PA2 (EXTI2)
 */
void EXTI2_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
        pulse_count[ENCODER_2]++;
    }
}

/**
 * @brief Обработчик прерывания для PA3 (EXTI3)
 */
void EXTI3_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
        pulse_count[ENCODER_3]++;
    }
}
