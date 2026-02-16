/**
 * @file    motor_test.h
 * @brief   Test functions for TB6612FNG motor drivers
 * @author  STM32 Black Pill Project
 * @date    2026-02-16
 */

#ifndef MOTOR_TEST_H
#define MOTOR_TEST_H

#include <stdint.h>

/**
 * @brief Run all motor driver tests (comprehensive test suite)
 * Tests STBY, direction pins, PWM, all motors, rapid changes, and pin verification
 * Total duration: ~1-2 minutes
 */
void Motor_Run_All_Tests(void);

/**
 * @brief Simple visual blink test (no serial needed)
 * Continuously blinks STBY pins for LED verification
 * Use this if you don't have serial monitor connected
 */
void Motor_Visual_Blink_Test(void);

/**
 * @brief Individual test functions (can be called separately)
 */
void Test_STBY_Pins(void);
void Test_Direction_Pins(void);
void Test_PWM_Signals(void);
void Test_All_Motors(void);
void Test_Rapid_Changes(void);
void Test_Pin_Verification(void);

#endif /* MOTOR_TEST_H */
