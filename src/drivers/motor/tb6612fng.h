/**
 * @file    tb6612fng.h
 * @brief   Driver for dual TB6612FNG motor controllers (4 motors total)
 * @author  STM32 Black Pill Project
 * @date    2026-02-15
 *
 * Supports 2 x TB6612FNG modules = 4 DC motors
 */

#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Motor Configuration
// ============================================================================

/**
 * @brief Motor identifiers (0-3 for 4 motors)
 */
typedef enum {
    MOTOR_0 = 0,  // Driver 1, Channel A
    MOTOR_1 = 1,  // Driver 1, Channel B
    MOTOR_2 = 2,  // Driver 2, Channel A
    MOTOR_3 = 3,  // Driver 2, Channel B
    MOTOR_COUNT = 4
} Motor_ID;

/**
 * @brief Motor direction and control states
 */
typedef enum {
    MOTOR_STOP    = 0,  // IN1=L, IN2=L (Coast stop)
    MOTOR_FORWARD = 1,  // IN1=H, IN2=L
    MOTOR_REVERSE = 2,  // IN1=L, IN2=H
    MOTOR_BRAKE   = 3   // IN1=H, IN2=H (Short brake)
} Motor_Direction;

/**
 * @brief Pin configuration for a single motor channel
 */
typedef struct {
    GPIO_TypeDef *in1_port;
    uint16_t in1_pin;
    GPIO_TypeDef *in2_port;
    uint16_t in2_pin;
    TIM_HandleTypeDef *htim;
    uint32_t tim_channel;
} Motor_Config;

/**
 * @brief Configuration for a TB6612FNG driver (2 motors)
 */
typedef struct {
    Motor_Config motor_a;
    Motor_Config motor_b;
    GPIO_TypeDef *stby_port;
    uint16_t stby_pin;
} TB6612FNG_Config;

// ============================================================================
// Pin Definitions - Driver 1 (Motors 0, 1)
// ============================================================================

// Motor 0 (Driver 1, Channel A)
#define MOTOR_0_IN1_PORT        GPIOB
#define MOTOR_0_IN1_PIN         GPIO_PIN_1
#define MOTOR_0_IN2_PORT        GPIOB
#define MOTOR_0_IN2_PIN         GPIO_PIN_10
#define MOTOR_0_PWM_TIMER       TIM3
#define MOTOR_0_PWM_CHANNEL     TIM_CHANNEL_3  // PB0

// Motor 1 (Driver 1, Channel B)
#define MOTOR_1_IN1_PORT        GPIOB
#define MOTOR_1_IN1_PIN         GPIO_PIN_12
#define MOTOR_1_IN2_PORT        GPIOB
#define MOTOR_1_IN2_PIN         GPIO_PIN_13
#define MOTOR_1_PWM_TIMER       TIM4
#define MOTOR_1_PWM_CHANNEL     TIM_CHANNEL_2  // PB7

// Driver 1 Standby
#define DRIVER_1_STBY_PORT      GPIOB
#define DRIVER_1_STBY_PIN       GPIO_PIN_14

// ============================================================================
// Pin Definitions - Driver 2 (Motors 2, 3)
// ============================================================================

// Motor 2 (Driver 2, Channel A)
#define MOTOR_2_IN1_PORT        GPIOA
#define MOTOR_2_IN1_PIN         GPIO_PIN_0
#define MOTOR_2_IN2_PORT        GPIOA
#define MOTOR_2_IN2_PIN         GPIO_PIN_1
#define MOTOR_2_PWM_TIMER       TIM1
#define MOTOR_2_PWM_CHANNEL     TIM_CHANNEL_1  // PA8

// Motor 3 (Driver 2, Channel B)
#define MOTOR_3_IN1_PORT        GPIOA
#define MOTOR_3_IN1_PIN         GPIO_PIN_2
#define MOTOR_3_IN2_PORT        GPIOA
#define MOTOR_3_IN2_PIN         GPIO_PIN_3
#define MOTOR_3_PWM_TIMER       TIM2
#define MOTOR_3_PWM_CHANNEL     TIM_CHANNEL_1  // PA15

// Driver 2 Standby
#define DRIVER_2_STBY_PORT      GPIOA
#define DRIVER_2_STBY_PIN       GPIO_PIN_4

// ============================================================================
// Public API Functions
// ============================================================================

/**
 * @brief Initialize TB6612FNG drivers and all 4 motors
 * @param htim1 Timer for Motor 0 PWM (TIM3)
 * @param htim2 Timer for Motor 1 PWM (TIM4)
 * @param htim3 Timer for Motor 2 PWM (TIM1)
 * @param htim4 Timer for Motor 3 PWM (TIM2)
 */
void TB6612FNG_Init(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2,
                    TIM_HandleTypeDef *htim3, TIM_HandleTypeDef *htim4);

/**
 * @brief Enable driver 1 (motors 0, 1)
 */
void TB6612FNG_EnableDriver1(void);

/**
 * @brief Disable driver 1 (motors 0, 1)
 */
void TB6612FNG_DisableDriver1(void);

/**
 * @brief Enable driver 2 (motors 2, 3)
 */
void TB6612FNG_EnableDriver2(void);

/**
 * @brief Disable driver 2 (motors 2, 3)
 */
void TB6612FNG_DisableDriver2(void);

/**
 * @brief Enable all drivers
 */
void TB6612FNG_EnableAll(void);

/**
 * @brief Disable all drivers (standby mode)
 */
void TB6612FNG_DisableAll(void);

/**
 * @brief Set motor speed (0-100%)
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 * @param speed Speed percentage (0-100)
 */
void TB6612FNG_SetSpeed(Motor_ID motor, uint8_t speed);

/**
 * @brief Set motor direction
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 * @param direction Direction (STOP, FORWARD, REVERSE, BRAKE)
 */
void TB6612FNG_SetDirection(Motor_ID motor, Motor_Direction direction);

/**
 * @brief Drive motor with speed and direction
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 * @param direction Direction (STOP, FORWARD, REVERSE, BRAKE)
 * @param speed Speed percentage (0-100)
 */
void TB6612FNG_Drive(Motor_ID motor, Motor_Direction direction, uint8_t speed);

/**
 * @brief Stop single motor
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 */
void TB6612FNG_Stop(Motor_ID motor);

/**
 * @brief Stop all motors
 */
void TB6612FNG_StopAll(void);

/**
 * @brief Emergency brake for single motor
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 */
void TB6612FNG_Brake(Motor_ID motor);

/**
 * @brief Emergency brake for all motors
 */
void TB6612FNG_BrakeAll(void);

// ============================================================================
// Advanced Functions (Robot Movement)
// ============================================================================

/**
 * @brief Move robot forward (all wheels same speed)
 * @param speed Speed percentage (0-100)
 */
void TB6612FNG_MoveForward(uint8_t speed);

/**
 * @brief Move robot backward (all wheels same speed)
 * @param speed Speed percentage (0-100)
 */
void TB6612FNG_MoveBackward(uint8_t speed);

/**
 * @brief Turn left (differential speed)
 * @param speed Base speed (0-100)
 * @param turn_ratio Turn sharpness (0-100, 0=slight, 100=pivot)
 */
void TB6612FNG_TurnLeft(uint8_t speed, uint8_t turn_ratio);

/**
 * @brief Turn right (differential speed)
 * @param speed Base speed (0-100)
 * @param turn_ratio Turn sharpness (0-100, 0=slight, 100=pivot)
 */
void TB6612FNG_TurnRight(uint8_t speed, uint8_t turn_ratio);

/**
 * @brief Rotate left in place (tank turn)
 * @param speed Rotation speed (0-100)
 */
void TB6612FNG_RotateLeft(uint8_t speed);

/**
 * @brief Rotate right in place (tank turn)
 * @param speed Rotation speed (0-100)
 */
void TB6612FNG_RotateRight(uint8_t speed);

/**
 * @brief Get current motor speed
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 * @return Current speed (0-100)
 */
uint8_t TB6612FNG_GetSpeed(Motor_ID motor);

/**
 * @brief Get current motor direction
 * @param motor Motor ID (MOTOR_0 to MOTOR_3)
 * @return Current direction
 */
Motor_Direction TB6612FNG_GetDirection(Motor_ID motor);

#endif /* TB6612FNG_H */
