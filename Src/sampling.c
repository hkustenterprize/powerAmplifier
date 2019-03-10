#include "main.h"
#include "can.h"
#include "dac.h"
#include "dma.h"
#include "sdadc.h"
#include "gpio.h"
#include "sampling.h"
#include "stm32f3xx_hal.h"

uint16_t dacValue = 1125;
uint32_t currentSamples[CURRENT_SAMPLE_AMOUNT];
int32_t currentSum = 0;
float currentAvg = 0;

void SamplingStart(void)
{
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)dacValue);
    HAL_SDADC_CalibrationStart(&hsdadc3, SDADC_CALIBRATION_SEQ_1);
    HAL_SDADC_PollForCalibEvent(&hsdadc3, HAL_MAX_DELAY);
    HAL_SDADC_Start_DMA(&hsdadc3, currentSamples, CURRENT_SAMPLE_AMOUNT);
    while (1)
    {
    }
}

void HAL_SDADC_ConvCpltCallback(SDADC_HandleTypeDef *hsdadc)
{
    if (hsdadc == &hsdadc3)
    { //current
        currentSum = 0;
        for (uint8_t i = 0; i < CURRENT_SAMPLE_AMOUNT; i++)
        {
            currentSum += (int16_t)(currentSamples[i] & 0xFFFF);
        }
        currentAvg = (-currentSum * 1.0 / CURRENT_SAMPLE_AMOUNT + 25.0) / 3885.5;
    }
}
