#include "main.h"

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();                   // Initialize the HAL Library
    SystemClock_Config();         // Configure the system clock
    __HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock

    GPIO_InitTypeDef GPIO_InitStruct = {0};      // Initialize the GPIO structure
    GPIO_InitStruct.Pin = GPIO_PIN_13;           // Configure pin 13 (connected to the onboard LED)
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Set the pin as push-pull output
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull-up or pull-down resistors
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Set the speed to low
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);      // Initialize GPIOC with the specified settings

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // Toggle the LED state
        HAL_Delay(500);                         // Delay for 500 milliseconds
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // Initialize the RCC oscillator structure (RCC stands for Reset and Clock Control)
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // Initialize the RCC clock structure

    __HAL_RCC_PWR_CLK_ENABLE();                                    // Enable the power control clock
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1); // Configure the voltage scaling for the power regulator

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // Use the High-Speed External (HSE) oscillator
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;                   // Turn on the HSE oscillator
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;               // Enable the Phase-Locked Loop (PLL)
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;       // Set the PLL source to HSE
    RCC_OscInitStruct.PLL.PLLM = 25;                           //
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
