/**
  ******************************************************************************
  * File Name          : SDADC.c
  * Description        : This file provides code for the configuration
  *                      of the SDADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sdadc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SDADC_HandleTypeDef hsdadc1;
SDADC_HandleTypeDef hsdadc3;
DMA_HandleTypeDef hdma_sdadc1;
DMA_HandleTypeDef hdma_sdadc3;

/* SDADC1 init function */
void MX_SDADC1_Init(void)
{
  SDADC_ConfParamTypeDef ConfParamStruct = {0};

  /** Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage 
  */
  hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_VREFINT2;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure The Regular Mode 
  */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    Error_Handler();
  }
  /** Set parameters for SDADC configuration 0 Register 
  */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Regular Channel 
  */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    Error_Handler();
  }

}
/* SDADC3 init function */
void MX_SDADC3_Init(void)
{
  SDADC_ConfParamTypeDef ConfParamStruct = {0};

  /** Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage 
  */
  hsdadc3.Instance = SDADC3;
  hsdadc3.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc3.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
  hsdadc3.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc3.Init.ReferenceVoltage = SDADC_VREF_VREFINT2;
  if (HAL_SDADC_Init(&hsdadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure The Regular Mode 
  */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc3, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    Error_Handler();
  }
  /** Set parameters for SDADC configuration 0 Register 
  */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc3, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Regular Channel 
  */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc3, SDADC_CHANNEL_8, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SDADC_ConfigChannel(&hsdadc3, SDADC_CHANNEL_8, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_SDADC_MspInit(SDADC_HandleTypeDef* sdadcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdadcHandle->Instance==SDADC1)
  {
  /* USER CODE BEGIN SDADC1_MspInit 0 */

  /* USER CODE END SDADC1_MspInit 0 */
    /* SDADC1 clock enable */
    __HAL_RCC_SDADC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SDADC1 GPIO Configuration    
    PB0     ------> SDADC1_AIN6P
    PB1     ------> SDADC1_AIN6M 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SDADC1 DMA Init */
    /* SDADC1 Init */
    hdma_sdadc1.Instance = DMA2_Channel3;
    hdma_sdadc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdadc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdadc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdadc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_sdadc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_sdadc1.Init.Mode = DMA_NORMAL;
    hdma_sdadc1.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_sdadc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdadcHandle,hdma,hdma_sdadc1);

  /* USER CODE BEGIN SDADC1_MspInit 1 */

  /* USER CODE END SDADC1_MspInit 1 */
  }
  else if(sdadcHandle->Instance==SDADC3)
  {
  /* USER CODE BEGIN SDADC3_MspInit 0 */

  /* USER CODE END SDADC3_MspInit 0 */
    /* SDADC3 clock enable */
    __HAL_RCC_SDADC3_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SDADC3 GPIO Configuration    
    PB14     ------> SDADC3_AIN8P
    PB15     ------> SDADC3_AIN8M 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SDADC3 DMA Init */
    /* SDADC3 Init */
    hdma_sdadc3.Instance = DMA2_Channel5;
    hdma_sdadc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdadc3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdadc3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdadc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdadc3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdadc3.Init.Mode = DMA_CIRCULAR;
    hdma_sdadc3.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_sdadc3) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdadcHandle,hdma,hdma_sdadc3);

  /* USER CODE BEGIN SDADC3_MspInit 1 */

  /* USER CODE END SDADC3_MspInit 1 */
  }
}

void HAL_SDADC_MspDeInit(SDADC_HandleTypeDef* sdadcHandle)
{

  if(sdadcHandle->Instance==SDADC1)
  {
  /* USER CODE BEGIN SDADC1_MspDeInit 0 */

  /* USER CODE END SDADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDADC1_CLK_DISABLE();
  
    /**SDADC1 GPIO Configuration    
    PB0     ------> SDADC1_AIN6P
    PB1     ------> SDADC1_AIN6M 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);

    /* SDADC1 DMA DeInit */
    HAL_DMA_DeInit(sdadcHandle->hdma);
  /* USER CODE BEGIN SDADC1_MspDeInit 1 */

  /* USER CODE END SDADC1_MspDeInit 1 */
  }
  else if(sdadcHandle->Instance==SDADC3)
  {
  /* USER CODE BEGIN SDADC3_MspDeInit 0 */

  /* USER CODE END SDADC3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDADC3_CLK_DISABLE();
  
    /**SDADC3 GPIO Configuration    
    PB14     ------> SDADC3_AIN8P
    PB15     ------> SDADC3_AIN8M 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14|GPIO_PIN_15);

    /* SDADC3 DMA DeInit */
    HAL_DMA_DeInit(sdadcHandle->hdma);
  /* USER CODE BEGIN SDADC3_MspDeInit 1 */

  /* USER CODE END SDADC3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
