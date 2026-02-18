/**
 * @file    uart_telemetry.c
 * @brief   UART telemetry implementation
 * @author  STM32 Black Pill Project
 * @date    2026-02-17
 */

#include "uart_telemetry.h"
#include <stdio.h>
#include <string.h>

/* UART Handle */
UART_HandleTypeDef huart1;

/* Internal buffer for JSON formatting */
static char telemetry_buffer[TELEMETRY_BUFFER_SIZE];

/**
 * @brief Initialize UART1 for telemetry (PA9=TX, PA10=RX)
 */
void Telemetry_Init(void)
{
    /* UART1 Configuration */
    huart1.Instance = TELEMETRY_UART;
    huart1.Init.BaudRate = TELEMETRY_BAUD_RATE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }

    printf("\n=== UART Telemetry Initialized ===\n");
    printf("UART: USART1\n");
    printf("Baud: %d\n", TELEMETRY_BAUD_RATE);
    printf("Pins: PA9 (TX), PA10 (RX)\n");
    printf("Target: ESP32-WROOM-32D\n\n");
}

/**
 * @brief HAL UART MSP Init (GPIO configuration)
 */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (uartHandle->Instance == USART1) {
        /* Enable clocks */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Configure GPIO pins: PA9 (TX), PA10 (RX) */
        GPIO_InitStruct.Pin = TELEMETRY_TX_PIN | TELEMETRY_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(TELEMETRY_GPIO_PORT, &GPIO_InitStruct);
    }
}

/**
 * @brief HAL UART MSP DeInit
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    if (uartHandle->Instance == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(TELEMETRY_GPIO_PORT, TELEMETRY_TX_PIN | TELEMETRY_RX_PIN);
    }
}

/**
 * @brief Send button event as JSON
 * Format: {"button":0,"state":"pressed"}
 */
void Telemetry_SendButton(uint8_t button_id, uint8_t is_pressed)
{
    int len = snprintf(telemetry_buffer, TELEMETRY_BUFFER_SIZE,
                       "{\"button\":%d,\"state\":\"%s\"}\n",
                       button_id,
                       is_pressed ? "pressed" : "released");

    if (len > 0 && len < TELEMETRY_BUFFER_SIZE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)telemetry_buffer, len, 100);
    }
}

/**
 * @brief Send motor state as JSON
 * Format: {"motor":0,"direction":"forward","speed":75}
 */
void Telemetry_SendMotor(uint8_t motor_id, uint8_t direction, uint8_t speed)
{
    const char* dir_str;
    switch (direction) {
        case 1:  dir_str = "forward";  break;  /* MOTOR_FORWARD */
        case 2:  dir_str = "backward"; break;  /* MOTOR_REVERSE */
        default: dir_str = "stop";     break;  /* MOTOR_STOP */
    }

    int len = snprintf(telemetry_buffer, TELEMETRY_BUFFER_SIZE,
                       "{\"motor\":%d,\"direction\":\"%s\",\"speed\":%d}\n",
                       motor_id, dir_str, speed);

    if (len > 0 && len < TELEMETRY_BUFFER_SIZE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)telemetry_buffer, len, 100);
    }
}

/**
 * @brief Send all motors state as JSON
 * Format: {"motors":[{"state":"running","speed":80},{"state":"stopped","speed":0},...]}
 */
void Telemetry_SendAllMotors(uint8_t* motor_states, uint8_t* motor_speeds)
{
    int len = snprintf(telemetry_buffer, TELEMETRY_BUFFER_SIZE,
                       "{\"motors\":[{\"state\":\"%s\",\"speed\":%d},{\"state\":\"%s\",\"speed\":%d},{\"state\":\"%s\",\"speed\":%d},{\"state\":\"%s\",\"speed\":%d}]}\n",
                       motor_states[0] ? "running" : "stopped", motor_speeds[0],
                       motor_states[1] ? "running" : "stopped", motor_speeds[1],
                       motor_states[2] ? "running" : "stopped", motor_speeds[2],
                       motor_states[3] ? "running" : "stopped", motor_speeds[3]);

    if (len > 0 && len < TELEMETRY_BUFFER_SIZE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)telemetry_buffer, len, 100);
    }
}

/**
 * @brief Send RPM data as JSON
 * Format: {"motor":0,"rpm":325.5}
 */
void Telemetry_SendRPM(uint8_t motor_id, float rpm)
{
    int len = snprintf(telemetry_buffer, TELEMETRY_BUFFER_SIZE,
                       "{\"motor\":%d,\"rpm\":%.1f}\n",
                       motor_id, rpm);

    if (len > 0 && len < TELEMETRY_BUFFER_SIZE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)telemetry_buffer, len, 100);
    }
}

/**
 * @brief Send custom JSON string
 */
void Telemetry_SendJSON(const char* json_string)
{
    int len = strlen(json_string);
    if (len > 0 && len < TELEMETRY_BUFFER_SIZE) {
        HAL_UART_Transmit(&huart1, (uint8_t*)json_string, len, HAL_MAX_DELAY);
        // Add newline if not present
        if (json_string[len - 1] != '\n') {
            HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 100);
        }
    }
}

/**
 * @brief Send raw string (for debugging)
 */
void Telemetry_SendString(const char* message)
{
    int len = strlen(message);
    if (len > 0) {
        HAL_UART_Transmit(&huart1, (uint8_t*)message, len, 100);
    }
}
