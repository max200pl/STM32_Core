/**
 * @file    tb6612fng.c
 * @brief   Implementation of dual TB6612FNG motor driver
 * @author  STM32 Black Pill Project
 * @date    2026-02-15
 */

#include "tb6612fng.h"

// ============================================================================
// Private Variables
// ============================================================================

// Motor configurations
static Motor_Config motor_configs[MOTOR_COUNT];

// Motor states (current speed and direction)
static uint8_t motor_speeds[MOTOR_COUNT] = {0, 0, 0, 0};
static Motor_Direction motor_directions[MOTOR_COUNT] = {
    MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP
};

// Driver configurations
static TB6612FNG_Config driver1_config;
static TB6612FNG_Config driver2_config;

// Initialization flag
static bool is_initialized = false;

// ============================================================================
// Private Functions
// ============================================================================

/**
 * @brief Configure GPIO for motor control pins
 */
static void GPIO_ConfigureMotorPins(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure all direction pins as output
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Motor 0 (Driver 1, Channel A)
    GPIO_InitStruct.Pin = MOTOR_0_IN1_PIN;
    HAL_GPIO_Init(MOTOR_0_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_0_IN2_PIN;
    HAL_GPIO_Init(MOTOR_0_IN2_PORT, &GPIO_InitStruct);

    // Motor 1 (Driver 1, Channel B)
    GPIO_InitStruct.Pin = MOTOR_1_IN1_PIN;
    HAL_GPIO_Init(MOTOR_1_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_1_IN2_PIN;
    HAL_GPIO_Init(MOTOR_1_IN2_PORT, &GPIO_InitStruct);

    // Driver 1 Standby
    GPIO_InitStruct.Pin = DRIVER_1_STBY_PIN;
    HAL_GPIO_Init(DRIVER_1_STBY_PORT, &GPIO_InitStruct);

    // Motor 2 (Driver 2, Channel A)
    GPIO_InitStruct.Pin = MOTOR_2_IN1_PIN;
    HAL_GPIO_Init(MOTOR_2_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_2_IN2_PIN;
    HAL_GPIO_Init(MOTOR_2_IN2_PORT, &GPIO_InitStruct);

    // Motor 3 (Driver 2, Channel B)
    GPIO_InitStruct.Pin = MOTOR_3_IN1_PIN;
    HAL_GPIO_Init(MOTOR_3_IN1_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MOTOR_3_IN2_PIN;
    HAL_GPIO_Init(MOTOR_3_IN2_PORT, &GPIO_InitStruct);

    // Driver 2 Standby
    GPIO_InitStruct.Pin = DRIVER_2_STBY_PIN;
    HAL_GPIO_Init(DRIVER_2_STBY_PORT, &GPIO_InitStruct);
}

/**
 * @brief Set motor direction pins
 */
static void SetMotorDirection(Motor_ID motor, Motor_Direction direction) {
    if (motor >= MOTOR_COUNT) return;

    Motor_Config *config = &motor_configs[motor];

    switch (direction) {
        case MOTOR_FORWARD:
            HAL_GPIO_WritePin(config->in1_port, config->in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(config->in2_port, config->in2_pin, GPIO_PIN_RESET);
            break;

        case MOTOR_REVERSE:
            HAL_GPIO_WritePin(config->in1_port, config->in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(config->in2_port, config->in2_pin, GPIO_PIN_SET);
            break;

        case MOTOR_BRAKE:
            HAL_GPIO_WritePin(config->in1_port, config->in1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(config->in2_port, config->in2_pin, GPIO_PIN_SET);
            break;

        case MOTOR_STOP:
        default:
            HAL_GPIO_WritePin(config->in1_port, config->in1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(config->in2_port, config->in2_pin, GPIO_PIN_RESET);
            break;
    }

    motor_directions[motor] = direction;
}

/**
 * @brief Set PWM duty cycle for motor
 */
static void SetMotorPWM(Motor_ID motor, uint8_t speed) {
    if (motor >= MOTOR_COUNT) return;
    if (speed > 100) speed = 100;

    Motor_Config *config = &motor_configs[motor];

    // Calculate pulse width based on timer period
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(config->htim);
    uint32_t pulse = (speed * period) / 100;

    // Set compare value for PWM
    __HAL_TIM_SET_COMPARE(config->htim, config->tim_channel, pulse);

    motor_speeds[motor] = speed;
}

// ============================================================================
// Public API Implementation
// ============================================================================

void TB6612FNG_Init(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2,
                    TIM_HandleTypeDef *htim3, TIM_HandleTypeDef *htim4) {

    // Configure motor 0 (Driver 1, Channel A)
    motor_configs[MOTOR_0].in1_port = MOTOR_0_IN1_PORT;
    motor_configs[MOTOR_0].in1_pin = MOTOR_0_IN1_PIN;
    motor_configs[MOTOR_0].in2_port = MOTOR_0_IN2_PORT;
    motor_configs[MOTOR_0].in2_pin = MOTOR_0_IN2_PIN;
    motor_configs[MOTOR_0].htim = htim1;
    motor_configs[MOTOR_0].tim_channel = MOTOR_0_PWM_CHANNEL;

    // Configure motor 1 (Driver 1, Channel B)
    motor_configs[MOTOR_1].in1_port = MOTOR_1_IN1_PORT;
    motor_configs[MOTOR_1].in1_pin = MOTOR_1_IN1_PIN;
    motor_configs[MOTOR_1].in2_port = MOTOR_1_IN2_PORT;
    motor_configs[MOTOR_1].in2_pin = MOTOR_1_IN2_PIN;
    motor_configs[MOTOR_1].htim = htim2;
    motor_configs[MOTOR_1].tim_channel = MOTOR_1_PWM_CHANNEL;

    // Configure motor 2 (Driver 2, Channel A)
    motor_configs[MOTOR_2].in1_port = MOTOR_2_IN1_PORT;
    motor_configs[MOTOR_2].in1_pin = MOTOR_2_IN1_PIN;
    motor_configs[MOTOR_2].in2_port = MOTOR_2_IN2_PORT;
    motor_configs[MOTOR_2].in2_pin = MOTOR_2_IN2_PIN;
    motor_configs[MOTOR_2].htim = htim3;
    motor_configs[MOTOR_2].tim_channel = MOTOR_2_PWM_CHANNEL;

    // Configure motor 3 (Driver 2, Channel B)
    motor_configs[MOTOR_3].in1_port = MOTOR_3_IN1_PORT;
    motor_configs[MOTOR_3].in1_pin = MOTOR_3_IN1_PIN;
    motor_configs[MOTOR_3].in2_port = MOTOR_3_IN2_PORT;
    motor_configs[MOTOR_3].in2_pin = MOTOR_3_IN2_PIN;
    motor_configs[MOTOR_3].htim = htim4;
    motor_configs[MOTOR_3].tim_channel = MOTOR_3_PWM_CHANNEL;

    // Configure driver 1
    driver1_config.motor_a = motor_configs[MOTOR_0];
    driver1_config.motor_b = motor_configs[MOTOR_1];
    driver1_config.stby_port = DRIVER_1_STBY_PORT;
    driver1_config.stby_pin = DRIVER_1_STBY_PIN;

    // Configure driver 2
    driver2_config.motor_a = motor_configs[MOTOR_2];
    driver2_config.motor_b = motor_configs[MOTOR_3];
    driver2_config.stby_port = DRIVER_2_STBY_PORT;
    driver2_config.stby_pin = DRIVER_2_STBY_PIN;

    // Initialize GPIO pins
    GPIO_ConfigureMotorPins();

    // Start PWM on all channels
    HAL_TIM_PWM_Start(htim1, MOTOR_0_PWM_CHANNEL);  // Motor 0
    HAL_TIM_PWM_Start(htim2, MOTOR_1_PWM_CHANNEL);  // Motor 1
    HAL_TIM_PWM_Start(htim3, MOTOR_2_PWM_CHANNEL);  // Motor 2
    HAL_TIM_PWM_Start(htim4, MOTOR_3_PWM_CHANNEL);  // Motor 3

    // Initial state: disabled, stopped
    TB6612FNG_DisableAll();
    TB6612FNG_StopAll();

    is_initialized = true;
}

void TB6612FNG_EnableDriver1(void) {
    HAL_GPIO_WritePin(DRIVER_1_STBY_PORT, DRIVER_1_STBY_PIN, GPIO_PIN_SET);
}

void TB6612FNG_DisableDriver1(void) {
    HAL_GPIO_WritePin(DRIVER_1_STBY_PORT, DRIVER_1_STBY_PIN, GPIO_PIN_RESET);
}

void TB6612FNG_EnableDriver2(void) {
    HAL_GPIO_WritePin(DRIVER_2_STBY_PORT, DRIVER_2_STBY_PIN, GPIO_PIN_SET);
}

void TB6612FNG_DisableDriver2(void) {
    HAL_GPIO_WritePin(DRIVER_2_STBY_PORT, DRIVER_2_STBY_PIN, GPIO_PIN_RESET);
}

void TB6612FNG_EnableAll(void) {
    TB6612FNG_EnableDriver1();
    TB6612FNG_EnableDriver2();
}

void TB6612FNG_DisableAll(void) {
    TB6612FNG_DisableDriver1();
    TB6612FNG_DisableDriver2();
}

void TB6612FNG_SetSpeed(Motor_ID motor, uint8_t speed) {
    if (!is_initialized || motor >= MOTOR_COUNT) return;
    SetMotorPWM(motor, speed);
}

void TB6612FNG_SetDirection(Motor_ID motor, Motor_Direction direction) {
    if (!is_initialized || motor >= MOTOR_COUNT) return;
    SetMotorDirection(motor, direction);
}

void TB6612FNG_Drive(Motor_ID motor, Motor_Direction direction, uint8_t speed) {
    if (!is_initialized || motor >= MOTOR_COUNT) return;

    SetMotorDirection(motor, direction);
    SetMotorPWM(motor, speed);
}

void TB6612FNG_Stop(Motor_ID motor) {
    if (!is_initialized || motor >= MOTOR_COUNT) return;

    SetMotorDirection(motor, MOTOR_STOP);
    SetMotorPWM(motor, 0);
}

void TB6612FNG_StopAll(void) {
    for (Motor_ID i = MOTOR_0; i < MOTOR_COUNT; i++) {
        TB6612FNG_Stop(i);
    }
}

void TB6612FNG_Brake(Motor_ID motor) {
    if (!is_initialized || motor >= MOTOR_COUNT) return;

    SetMotorDirection(motor, MOTOR_BRAKE);
    SetMotorPWM(motor, 100);  // Full brake
}

void TB6612FNG_BrakeAll(void) {
    for (Motor_ID i = MOTOR_0; i < MOTOR_COUNT; i++) {
        TB6612FNG_Brake(i);
    }
}

// ============================================================================
// Advanced Movement Functions
// ============================================================================

void TB6612FNG_MoveForward(uint8_t speed) {
    for (Motor_ID i = MOTOR_0; i < MOTOR_COUNT; i++) {
        TB6612FNG_Drive(i, MOTOR_FORWARD, speed);
    }
}

void TB6612FNG_MoveBackward(uint8_t speed) {
    for (Motor_ID i = MOTOR_0; i < MOTOR_COUNT; i++) {
        TB6612FNG_Drive(i, MOTOR_REVERSE, speed);
    }
}

void TB6612FNG_TurnLeft(uint8_t speed, uint8_t turn_ratio) {
    if (turn_ratio > 100) turn_ratio = 100;

    // Calculate left side speed (reduced)
    uint8_t left_speed = speed * (100 - turn_ratio) / 100;

    // Left side (motors 0, 1) slower
    TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, left_speed);
    TB6612FNG_Drive(MOTOR_1, MOTOR_FORWARD, left_speed);

    // Right side (motors 2, 3) normal speed
    TB6612FNG_Drive(MOTOR_2, MOTOR_FORWARD, speed);
    TB6612FNG_Drive(MOTOR_3, MOTOR_FORWARD, speed);
}

void TB6612FNG_TurnRight(uint8_t speed, uint8_t turn_ratio) {
    if (turn_ratio > 100) turn_ratio = 100;

    // Calculate right side speed (reduced)
    uint8_t right_speed = speed * (100 - turn_ratio) / 100;

    // Left side (motors 0, 1) normal speed
    TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, speed);
    TB6612FNG_Drive(MOTOR_1, MOTOR_FORWARD, speed);

    // Right side (motors 2, 3) slower
    TB6612FNG_Drive(MOTOR_2, MOTOR_FORWARD, right_speed);
    TB6612FNG_Drive(MOTOR_3, MOTOR_FORWARD, right_speed);
}

void TB6612FNG_RotateLeft(uint8_t speed) {
    // Left side (0, 1) reverse, right side (2, 3) forward
    TB6612FNG_Drive(MOTOR_0, MOTOR_REVERSE, speed);
    TB6612FNG_Drive(MOTOR_1, MOTOR_REVERSE, speed);
    TB6612FNG_Drive(MOTOR_2, MOTOR_FORWARD, speed);
    TB6612FNG_Drive(MOTOR_3, MOTOR_FORWARD, speed);
}

void TB6612FNG_RotateRight(uint8_t speed) {
    // Left side (0, 1) forward, right side (2, 3) reverse
    TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, speed);
    TB6612FNG_Drive(MOTOR_1, MOTOR_FORWARD, speed);
    TB6612FNG_Drive(MOTOR_2, MOTOR_REVERSE, speed);
    TB6612FNG_Drive(MOTOR_3, MOTOR_REVERSE, speed);
}

uint8_t TB6612FNG_GetSpeed(Motor_ID motor) {
    if (motor >= MOTOR_COUNT) return 0;
    return motor_speeds[motor];
}

Motor_Direction TB6612FNG_GetDirection(Motor_ID motor) {
    if (motor >= MOTOR_COUNT) return MOTOR_STOP;
    return motor_directions[motor];
}
