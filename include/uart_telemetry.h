/**
 * @file    uart_telemetry.h
 * @brief   UART telemetry module for sending data to ESP32
 * @author  STM32 Black Pill Project
 * @date    2026-02-17
 *
 * Sends JSON formatted telemetry data via UART to ESP32
 * ESP32 forwards data to Node.js server via WiFi
 */

#ifndef UART_TELEMETRY_H
#define UART_TELEMETRY_H

#include "main.h"
#include <stdint.h>

/* UART Configuration */
#define TELEMETRY_UART          USART1
#define TELEMETRY_BAUD_RATE     115200

/* UART GPIO Pins */
#define TELEMETRY_TX_PIN        GPIO_PIN_9   // PA9
#define TELEMETRY_RX_PIN        GPIO_PIN_10  // PA10
#define TELEMETRY_GPIO_PORT     GPIOA

/* JSON Buffer Size */
#define TELEMETRY_BUFFER_SIZE   256

/* UART Handle (external declaration) */
extern UART_HandleTypeDef huart1;

/**
 * @brief Initialize UART for telemetry
 */
void Telemetry_Init(void);

/**
 * @brief Send button event (press/release)
 * @param button_id Button number (0-3)
 * @param is_pressed 1 if pressed, 0 if released
 */
void Telemetry_SendButton(uint8_t button_id, uint8_t is_pressed);

/**
 * @brief Send motor state
 * @param motor_id Motor number (0-3)
 * @param direction Direction (0=FORWARD, 1=BACKWARD)
 * @param speed Speed percentage (0-100)
 */
void Telemetry_SendMotor(uint8_t motor_id, uint8_t direction, uint8_t speed);

/**
 * @brief Send all motors state
 * @param motor_states Array of 4 motor states (0=stopped, 1=running)
 * @param motor_speeds Array of 4 motor speeds (0-100%)
 */
void Telemetry_SendAllMotors(uint8_t* motor_states, uint8_t* motor_speeds);

/**
 * @brief Send RPM data from encoder
 * @param motor_id Motor number (0-3)
 * @param rpm RPM value
 */
void Telemetry_SendRPM(uint8_t motor_id, float rpm);

/**
 * @brief Send custom JSON string
 * @param json_string Pre-formatted JSON string
 */
void Telemetry_SendJSON(const char* json_string);

/**
 * @brief Send raw string (for debugging)
 * @param message String to send
 */
void Telemetry_SendString(const char* message);

#endif // UART_TELEMETRY_H
