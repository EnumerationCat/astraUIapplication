/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdio.h>
#define SCB_CFSR_IACCVIOL_Msk    (1UL << 0)    // 指令访问违例（MMFSR）
#define SCB_CFSR_DACCVIOL_Msk    (1UL << 1)    // 数据访问违例（MMFSR）
#define SCB_CFSR_STKOF_Msk       (1UL << 25)   // 栈溢出（UFSR，扩展故障）
#define SCB_CFSR_BUSERR_Msk      (1UL << 8)    // 精确数据总线错误（BFSR）
#define SCB_CFSR_UNDEFINSTR_Msk  (1UL << 16)   // 未定义指令（UFSR，你之前遇到的故障）
#define SCB_CFSR_DIVBYZERO_Msk   (1UL << 24)   // 整数除零（UFSR）
#define SCB_CFSR_UNALIGNED_Msk   (1UL << 25)   // 未对齐访问（UFSR）
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_tim3_ch1_trig;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  // 读取Cortex-M内核故障状态寄存器CFSR（核心：获取故障根源）
  uint32_t cfsr = SCB->CFSR;
  // 拆分CFSR为4字节HEX（便于串口查看原始值，小端顺序）
  uint8_t cfsrBuf[4] = {
    (uint8_t)(cfsr >> 0) & 0xFF,  // 最低字节
    (uint8_t)(cfsr >> 8) & 0xFF,
    (uint8_t)(cfsr >> 16) & 0xFF,
    (uint8_t)(cfsr >> 24) & 0xFF   // 最高字节
  };
  // 1. 定义字符串缓冲区：存储 "CFSR=0x00010000\n" 格式的文本
  // 缓冲区大小计算："CFSR=0x"（7字符） + 8位十六进制（如00010000） + "\n"（1字符） + 结束符\0（1字符） → 共17字节
  char cfsrStr[17] = {0};
  // 2. 将CFSR数值格式化为 "CFSR=0xXXXXXXXX\n" 字符串
  // %08X：8位大写十六进制，不足补0（32位数值正好对应8位十六进制）
  sprintf(cfsrStr, "CFSR=0x%08X\n", cfsr);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */

    // 通用错误标识（确认HardFault触发）
    HAL_UART_Transmit(&huart1, (uint8_t*)"Error:\n", 7, 100);  // 修正长度："Error\n"共6字节
    // 核心：解析所有常见故障类型（else if确保仅匹配一种故障）
    if (cfsr & SCB_CFSR_UNDEFINSTR_Msk) {
      // 未定义指令：执行无效指令（如hal.cpp中函数未实现、函数指针错误）
      HAL_UART_Transmit(&huart1, (uint8_t*)"UNDEFINSTR\n", 11, 100);  // 长度12（含\n）
    } else if (cfsr & SCB_CFSR_DIVBYZERO_Msk) {
      // 整数除零：代码中存在/0或%0操作（需检查hal.cpp中数值计算）
      HAL_UART_Transmit(&huart1, (uint8_t*)"DIVBYZERO\n", 11, 100);   // 长度11
    } else if (cfsr & SCB_CFSR_UNALIGNED_Msk) {
      // 未对齐访问：读写非4字节对齐地址（如hal.cpp中指针强制转换错误）
      HAL_UART_Transmit(&huart1, (uint8_t*)"UNALIGNED\n", 11, 100);   // 长度11
    } else if (cfsr & SCB_CFSR_STKOF_Msk) {
      // 栈溢出：菜单过多/函数嵌套深（你之前的高频场景）
      HAL_UART_Transmit(&huart1, (uint8_t*)"Stack Overflow\n", 15, 100);  // 长度15
    } else if (cfsr & SCB_CFSR_DACCVIOL_Msk) {
      // 数据访问违例：hal.cpp中越界读写（如数组、未初始化指针）
      HAL_UART_Transmit(&huart1, (uint8_t*)"DACCVIOL\n", 9, 100);     // 长度9
    } else if (cfsr & SCB_CFSR_IACCVIOL_Msk) {
      // 指令访问违例：执行NULL指针函数（如hal.cpp中虚函数未实现）
      HAL_UART_Transmit(&huart1, (uint8_t*)"IACCVIOL\n", 9, 100);     // 长度9
    } else if (cfsr & SCB_CFSR_BUSERR_Msk) {
      // 总线错误：访问不存在的外设（如hal.cpp中UART/SPI地址错误）
      HAL_UART_Transmit(&huart1, (uint8_t*)"Bus Fault\n", 10, 100);   // 长度10
    } else {
      // 未知故障：输出原始值便于调试
      HAL_UART_Transmit(&huart1, (uint8_t*)"Unknown Fault\n", 14, 100);  // 长度14
    }
    //发送格式化后的字符串（以字符形式发送，接收端直接显示 "CFSR=0x00010000"）
    // 发送长度："CFSR=0xXXXXXXXX\n" 共 7+8+1=16 字节（不含结束符\0）
    HAL_UART_Transmit(&huart1, (uint8_t*)cfsrStr, 16, 100);



    for (int i=0;i<=5000;i++) {
      for (int j=0;j<=1000;j++);

    }

    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel5 global interrupt.
  */
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */

  /* USER CODE END DMA1_Channel6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim3_ch1_trig);
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles ADC1 and ADC2 global interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi1);
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
