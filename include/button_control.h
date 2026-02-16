/**
 * @file    button_control.h
 * @brief   Button-controlled motor driver interface
 * @author  STM32 Black Pill Project
 * @date    2026-02-16
 *
 * Maps 4 buttons to 4 motors with LED indicators
 * Logic: Hold button to run motor, release to stop
 */

#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include "main.h"
#include "drivers/motor/tb6612fng.h"

/* Button GPIO Pins (with internal pull-up) */
#define BTN_0_PORT      GPIOB
#define BTN_0_PIN       GPIO_PIN_3

#define BTN_1_PORT      GPIOB
#define BTN_1_PIN       GPIO_PIN_4

#define BTN_2_PORT      GPIOC
#define BTN_2_PIN       GPIO_PIN_14

#define BTN_3_PORT      GPIOC
#define BTN_3_PIN       GPIO_PIN_15

/* LED GPIO Pins (optional indicators) */
#define LED_0_PORT      GPIOA
#define LED_0_PIN       GPIO_PIN_5

#define LED_1_PORT      GPIOA
#define LED_1_PIN       GPIO_PIN_6

#define LED_2_PORT      GPIOA
#define LED_2_PIN       GPIO_PIN_7

#define LED_3_PORT      GPIOA
#define LED_3_PIN       GPIO_PIN_11

/* Motor control parameters */
#define MOTOR_DEFAULT_SPEED     70      // Default speed (0-100%)
#define MOTOR_DIRECTION         MOTOR_FORWARD

/* Button IDs */
typedef enum {
    BUTTON_0 = 0,
    BUTTON_1 = 1,
    BUTTON_2 = 2,
    BUTTON_3 = 3,
    BUTTON_COUNT = 4
} Button_ID;

/**
 * @brief Initialize buttons and LEDs GPIO
 */
void ButtonControl_Init(void);

/**
 * @brief Main control loop - call this in main while(1)
 * Reads button states and controls motors accordingly
 */
void ButtonControl_Update(void);

/**
 * @brief Check if button is pressed (active LOW with pull-up)
 * @param button_id Button to check (BUTTON_0 to BUTTON_3)
 * @return 1 if pressed, 0 if released
 */
uint8_t ButtonControl_IsPressed(Button_ID button_id);

/**
 * @brief Turn LED on
 * @param led_id LED number (0-3)
 */
void ButtonControl_LED_On(uint8_t led_id);

/**
 * @brief Turn LED off
 * @param led_id LED number (0-3)
 */
void ButtonControl_LED_Off(uint8_t led_id);

/**
 * @brief Toggle LED state
 * @param led_id LED number (0-3)
 */
void ButtonControl_LED_Toggle(uint8_t led_id);

#endif // BUTTON_CONTROL_H
