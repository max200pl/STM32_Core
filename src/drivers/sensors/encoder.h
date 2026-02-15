/**
 * @file    encoder.h
 * @brief   Optical encoder driver for 4 motors
 * @author  STM32 Black Pill Project
 * @date    2026-02-15
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// Количество прорезов на диске энкодера
#define ENCODER_SLOTS_PER_REV   20

// Идентификаторы энкодеров
typedef enum {
    ENCODER_0 = 0,  // Motor 0 - PB6
    ENCODER_1 = 1,  // Motor 1 - PB7
    ENCODER_2 = 2,  // Motor 2 - PA2
    ENCODER_3 = 3,  // Motor 3 - PA3
    ENCODER_COUNT = 4
} Encoder_ID;

// ============================================================================
// ПУБЛИЧНЫЕ ФУНКЦИИ
// ============================================================================

/**
 * @brief Инициализация всех энкодеров
 * Настраивает GPIO с прерываниями EXTI
 */
void Encoder_Init(void);

/**
 * @brief Сбросить счётчик импульсов
 * @param encoder ID энкодера (ENCODER_0 ... ENCODER_3)
 */
void Encoder_ResetCount(Encoder_ID encoder);

/**
 * @brief Получить общее количество импульсов
 * @param encoder ID энкодера
 * @return Количество импульсов с момента сброса
 */
uint32_t Encoder_GetCount(Encoder_ID encoder);

/**
 * @brief Получить текущую скорость (RPM)
 * @param encoder ID энкодера
 * @return Скорость в оборотах/минуту
 */
float Encoder_GetRPM(Encoder_ID encoder);

/**
 * @brief Обновить расчёт RPM
 * @note Вызывать периодически каждые 100 мс
 */
void Encoder_Update(void);

#endif /* ENCODER_H */
