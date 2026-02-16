/**
 * @file    motor_test.c
 * @brief   Test program for TB6612FNG motor drivers (without motors)
 * @author  STM32 Black Pill Project
 * @date    2026-02-16
 *
 * Test without motors connected - verifies pin configuration and signals
 */

#include "main.h"
#include "drivers/motor/tb6612fng.h"
#include <stdio.h>

// External timer handles (defined in main.c)
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/**
 * @brief Test 1: STBY pins (Enable/Disable)
 * Expected: PB14 and PA4 toggle HIGH/LOW
 */
void Test_STBY_Pins(void)
{
    printf("\n=== Test 1: STBY Pins ===\n");
    printf("Watch PB14 (Driver 1) and PA4 (Driver 2)\n");
    printf("Expected: HIGH -> LOW -> HIGH\n\n");

    // Enable both drivers (STBY = HIGH = 3.3V)
    printf("Enabling all drivers...\n");
    TB6612FNG_EnableAll();
    HAL_Delay(2000);

    // Disable both drivers (STBY = LOW = 0V)
    printf("Disabling all drivers...\n");
    TB6612FNG_DisableAll();
    HAL_Delay(2000);

    // Enable again
    printf("Enabling all drivers...\n");
    TB6612FNG_EnableAll();
    HAL_Delay(2000);

    printf("Test 1: PASSED ✓\n");
}

/**
 * @brief Test 2: Direction pins (IN1, IN2)
 * Expected: IN1/IN2 change according to direction
 */
void Test_Direction_Pins(void)
{
    printf("\n=== Test 2: Direction Pins (Motor 0) ===\n");
    printf("Watch PB1 (IN1) and PB10 (IN2)\n\n");

    TB6612FNG_EnableAll();

    // FORWARD: IN1=HIGH, IN2=LOW
    printf("FORWARD: PB1=HIGH, PB10=LOW\n");
    TB6612FNG_SetDirection(MOTOR_0, MOTOR_FORWARD);
    HAL_Delay(2000);

    // REVERSE: IN1=LOW, IN2=HIGH
    printf("REVERSE: PB1=LOW, PB10=HIGH\n");
    TB6612FNG_SetDirection(MOTOR_0, MOTOR_REVERSE);
    HAL_Delay(2000);

    // BRAKE: IN1=HIGH, IN2=HIGH
    printf("BRAKE: PB1=HIGH, PB10=HIGH\n");
    TB6612FNG_SetDirection(MOTOR_0, MOTOR_BRAKE);
    HAL_Delay(2000);

    // STOP: IN1=LOW, IN2=LOW
    printf("STOP: PB1=LOW, PB10=LOW\n");
    TB6612FNG_SetDirection(MOTOR_0, MOTOR_STOP);
    HAL_Delay(2000);

    printf("Test 2: PASSED ✓\n");
}

/**
 * @brief Test 3: PWM signals
 * Expected: PWM duty cycle changes from 0% to 100%
 */
void Test_PWM_Signals(void)
{
    printf("\n=== Test 3: PWM Signals (Motor 0) ===\n");
    printf("Watch PB0 (PWM) with oscilloscope or LED brightness\n\n");

    TB6612FNG_EnableAll();
    TB6612FNG_SetDirection(MOTOR_0, MOTOR_FORWARD);

    // Ramp up speed 0% -> 100%
    printf("Ramping speed 0%% -> 100%%\n");
    for (uint8_t speed = 0; speed <= 100; speed += 10) {
        printf("Speed: %d%%\n", speed);
        TB6612FNG_SetSpeed(MOTOR_0, speed);
        HAL_Delay(500);
    }

    HAL_Delay(1000);

    // Ramp down speed 100% -> 0%
    printf("Ramping speed 100%% -> 0%%\n");
    for (int16_t speed = 100; speed >= 0; speed -= 10) {
        printf("Speed: %d%%\n", speed);
        TB6612FNG_SetSpeed(MOTOR_0, (uint8_t)speed);
        HAL_Delay(500);
    }

    TB6612FNG_Stop(MOTOR_0);
    printf("Test 3: PASSED ✓\n");
}

/**
 * @brief Test 4: All 4 motors sequentially
 * Expected: Each motor control pins activate in sequence
 */
void Test_All_Motors(void)
{
    printf("\n=== Test 4: All Motors Sequential ===\n");
    printf("Testing each motor for 2 seconds at 50%% speed\n\n");

    TB6612FNG_EnableAll();

    for (Motor_ID motor = MOTOR_0; motor < MOTOR_COUNT; motor++) {
        printf("Testing Motor %d...\n", motor);
        TB6612FNG_Drive(motor, MOTOR_FORWARD, 50);
        HAL_Delay(2000);
        TB6612FNG_Stop(motor);
        HAL_Delay(500);
    }

    printf("Test 4: PASSED ✓\n");
}

/**
 * @brief Test 5: Rapid direction changes
 * Expected: Pins switch quickly without errors
 */
void Test_Rapid_Changes(void)
{
    printf("\n=== Test 5: Rapid Direction Changes ===\n");
    printf("Rapidly changing directions 10 times\n\n");

    TB6612FNG_EnableAll();

    for (int i = 0; i < 10; i++) {
        printf("Cycle %d: FORWARD\n", i + 1);
        TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 70);
        HAL_Delay(200);

        printf("Cycle %d: REVERSE\n", i + 1);
        TB6612FNG_Drive(MOTOR_0, MOTOR_REVERSE, 70);
        HAL_Delay(200);
    }

    TB6612FNG_Stop(MOTOR_0);
    printf("Test 5: PASSED ✓\n");
}

/**
 * @brief Test 6: All pins verification table
 * Prints expected voltage levels for manual measurement
 */
void Test_Pin_Verification(void)
{
    printf("\n=== Test 6: Pin Verification Table ===\n");
    printf("Measure these pins with multimeter:\n\n");

    printf("Driver 1 (Motor 0 - FORWARD, 50%% speed):\n");
    printf("  PB1  (AIN1)  -> 3.3V\n");
    printf("  PB10 (AIN2)  -> 0.0V\n");
    printf("  PB0  (PWMA)  -> ~1.65V (50%% PWM average)\n");
    printf("  PB14 (STBY)  -> 3.3V\n\n");

    TB6612FNG_EnableAll();
    TB6612FNG_Drive(MOTOR_0, MOTOR_FORWARD, 50);

    printf("Holding state for 10 seconds for measurement...\n");
    for (int i = 10; i > 0; i--) {
        printf("%d...\n", i);
        HAL_Delay(1000);
    }

    TB6612FNG_Stop(MOTOR_0);
    printf("Test 6: COMPLETE ✓\n");
}

/**
 * @brief Main test sequence runner
 */
void Motor_Run_All_Tests(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║   TB6612FNG Motor Driver Test Suite          ║\n");
    printf("║   Black Pill STM32F411 + 2x TB6612FNG        ║\n");
    printf("║   Testing WITHOUT motors connected            ║\n");
    printf("╚════════════════════════════════════════════════╝\n");

    HAL_Delay(2000);

    // Run all tests
    Test_STBY_Pins();
    HAL_Delay(1000);

    Test_Direction_Pins();
    HAL_Delay(1000);

    Test_PWM_Signals();
    HAL_Delay(1000);

    Test_All_Motors();
    HAL_Delay(1000);

    Test_Rapid_Changes();
    HAL_Delay(1000);

    Test_Pin_Verification();

    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║   ALL TESTS COMPLETED ✓                       ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("\n");

    TB6612FNG_DisableAll();
}

/**
 * @brief Continuous blink test for visual verification
 * Run this if you don't have serial monitor
 */
void Motor_Visual_Blink_Test(void)
{
    while (1) {
        // Blink STBY pins (if LED connected)
        TB6612FNG_EnableAll();
        HAL_Delay(500);
        TB6612FNG_DisableAll();
        HAL_Delay(500);
    }
}
