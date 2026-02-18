/**
 * @file    button_control.c
 * @brief   Button-controlled motor driver implementation
 * @author  STM32 Black Pill Project
 * @date    2026-02-16
 *
 * Logic: Hold button → motor runs + LED on
 *        Release button → motor stops + LED off
 */

#include "button_control.h"
#include <stdio.h>

/* Previous button states for edge detection */
static uint8_t button_prev_state[BUTTON_COUNT] = {0};

/**
 * @brief Initialize GPIO for buttons (input with pull-up) and LEDs (output)
 */
void ButtonControl_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure Button 0 (PB3) */
    GPIO_InitStruct.Pin = BTN_0_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // Internal pull-up resistor
    HAL_GPIO_Init(BTN_0_PORT, &GPIO_InitStruct);

    /* Configure Button 1 (PB4) */
    GPIO_InitStruct.Pin = BTN_1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BTN_1_PORT, &GPIO_InitStruct);

    /* Configure Button 2 (PC14) */
    GPIO_InitStruct.Pin = BTN_2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BTN_2_PORT, &GPIO_InitStruct);

    /* Configure Button 3 (PC15) */
    GPIO_InitStruct.Pin = BTN_3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BTN_3_PORT, &GPIO_InitStruct);

    /* Configure LEDs (PA5, PA6, PA7, PA11) as outputs */
    GPIO_InitStruct.Pin = LED_0_PIN | LED_1_PIN | LED_2_PIN | LED_3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Turn off all LEDs initially */
    HAL_GPIO_WritePin(LED_0_PORT, LED_0_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_1_PORT, LED_1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_2_PORT, LED_2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_3_PORT, LED_3_PIN, GPIO_PIN_RESET);

    printf("\n=== Button Control Initialized ===\n");
    printf("BTN_0 (PB3)  → Motor 0 + LED_0 (PA5)\n");
    printf("BTN_1 (PB4)  → Motor 1 + LED_1 (PA6)\n");
    printf("BTN_2 (PC14) → Motor 2 + LED_2 (PA7)\n");
    printf("BTN_3 (PC15) → Motor 3 + LED_3 (PA11)\n");
    printf("Hold button to run motor at %d%% speed\n\n", MOTOR_DEFAULT_SPEED);
}

/**
 * @brief Check if button is pressed
 * @note Buttons are active LOW (0 = pressed, 1 = released)
 */
uint8_t ButtonControl_IsPressed(Button_ID button_id)
{
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (button_id) {
        case BUTTON_0:
            port = BTN_0_PORT;
            pin = BTN_0_PIN;
            break;
        case BUTTON_1:
            port = BTN_1_PORT;
            pin = BTN_1_PIN;
            break;
        case BUTTON_2:
            port = BTN_2_PORT;
            pin = BTN_2_PIN;
            break;
        case BUTTON_3:
            port = BTN_3_PORT;
            pin = BTN_3_PIN;
            break;
        default:
            return 0;
    }

    /* Button is pressed when pin is LOW (pull-up resistor) */
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) ? 1 : 0;
}

/**
 * @brief Turn LED on
 */
void ButtonControl_LED_On(uint8_t led_id)
{
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (led_id) {
        case 0:
            port = LED_0_PORT;
            pin = LED_0_PIN;
            break;
        case 1:
            port = LED_1_PORT;
            pin = LED_1_PIN;
            break;
        case 2:
            port = LED_2_PORT;
            pin = LED_2_PIN;
            break;
        case 3:
            port = LED_3_PORT;
            pin = LED_3_PIN;
            break;
        default:
            return;
    }

    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

/**
 * @brief Turn LED off
 */
void ButtonControl_LED_Off(uint8_t led_id)
{
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (led_id) {
        case 0:
            port = LED_0_PORT;
            pin = LED_0_PIN;
            break;
        case 1:
            port = LED_1_PORT;
            pin = LED_1_PIN;
            break;
        case 2:
            port = LED_2_PORT;
            pin = LED_2_PIN;
            break;
        case 3:
            port = LED_3_PORT;
            pin = LED_3_PIN;
            break;
        default:
            return;
    }

    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

/**
 * @brief Toggle LED state
 */
void ButtonControl_LED_Toggle(uint8_t led_id)
{
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (led_id) {
        case 0:
            port = LED_0_PORT;
            pin = LED_0_PIN;
            break;
        case 1:
            port = LED_1_PORT;
            pin = LED_1_PIN;
            break;
        case 2:
            port = LED_2_PORT;
            pin = LED_2_PIN;
            break;
        case 3:
            port = LED_3_PORT;
            pin = LED_3_PIN;
            break;
        default:
            return;
    }

    HAL_GPIO_TogglePin(port, pin);
}

/* D-pad layout:
 *  BTN_0 (PB3)  → Forward  (all motors forward)
 *  BTN_1 (PB4)  → Left     (rotate left in place)
 *  BTN_2 (PC14) → Right    (rotate right in place)
 *  BTN_3 (PC15) → Backward (all motors backward)
 */

#ifdef USE_UART_TELEMETRY
extern void Telemetry_SendButton(uint8_t button_id, uint8_t is_pressed);
extern void Telemetry_SendMotor(uint8_t motor_id, uint8_t direction, uint8_t speed);
#endif

/* Send telemetry for all 4 motors at once */
static void SendAllMotors(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t spd)
{
    #ifdef USE_UART_TELEMETRY
    Telemetry_SendMotor(0, d0, spd);
    Telemetry_SendMotor(1, d1, spd);
    Telemetry_SendMotor(2, d2, spd);
    Telemetry_SendMotor(3, d3, spd);
    #endif
}

/**
 * @brief Main control loop - D-pad style robot control
 * Call this function in main() while(1) loop
 */
void ButtonControl_Update(void)
{
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        uint8_t is_pressed = ButtonControl_IsPressed((Button_ID)i);

        if (is_pressed && !button_prev_state[i]) {
            /* Button pressed - execute movement */
            ButtonControl_LED_On(i);
            #ifdef USE_UART_TELEMETRY
            Telemetry_SendButton(i, 1);
            #endif

            switch (i) {
                case 0: /* Forward */
                    printf("BTN_0 → FORWARD %d%%\n", MOTOR_DEFAULT_SPEED);
                    TB6612FNG_MoveForward(MOTOR_DEFAULT_SPEED);
                    SendAllMotors(MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_FORWARD, MOTOR_DEFAULT_SPEED);
                    break;
                case 1: /* Left (rotate in place) */
                    printf("BTN_1 → ROTATE LEFT %d%%\n", MOTOR_DEFAULT_SPEED);
                    TB6612FNG_RotateLeft(MOTOR_DEFAULT_SPEED);
                    SendAllMotors(MOTOR_REVERSE, MOTOR_FORWARD, MOTOR_REVERSE, MOTOR_FORWARD, MOTOR_DEFAULT_SPEED);
                    break;
                case 2: /* Right (rotate in place) */
                    printf("BTN_2 → ROTATE RIGHT %d%%\n", MOTOR_DEFAULT_SPEED);
                    TB6612FNG_RotateRight(MOTOR_DEFAULT_SPEED);
                    SendAllMotors(MOTOR_FORWARD, MOTOR_REVERSE, MOTOR_FORWARD, MOTOR_REVERSE, MOTOR_DEFAULT_SPEED);
                    break;
                case 3: /* Backward */
                    printf("BTN_3 → BACKWARD %d%%\n", MOTOR_DEFAULT_SPEED);
                    TB6612FNG_MoveBackward(MOTOR_DEFAULT_SPEED);
                    SendAllMotors(MOTOR_REVERSE, MOTOR_REVERSE, MOTOR_REVERSE, MOTOR_REVERSE, MOTOR_DEFAULT_SPEED);
                    break;
            }
        }
        else if (!is_pressed && button_prev_state[i]) {
            /* Button released - stop all motors */
            printf("BTN_%d released → STOP ALL\n", i);
            TB6612FNG_StopAll();
            ButtonControl_LED_Off(i);

            #ifdef USE_UART_TELEMETRY
            Telemetry_SendButton(i, 0);
            SendAllMotors(MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, MOTOR_STOP, 0);
            #endif
        }

        button_prev_state[i] = is_pressed;
    }

    HAL_Delay(10);
}
