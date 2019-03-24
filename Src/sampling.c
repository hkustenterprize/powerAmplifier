/**
 * @file sampling.c
 * @author Alex Au (alex_acw@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "stm32f3xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "can.h"
#include "dac.h"
#include "dma.h"
#include "sdadc.h"
#include "gpio.h"
#include "sampling.h"

#define SAMPLE_INTERVAL_MS 5
#define CURRENT_SAMPLE_AMOUNT 50
#define VOLTAGE_SAMPLE_AMOUNT 50
const uint16_t dacValue = 1125;
uint32_t currentSamples[CURRENT_SAMPLE_AMOUNT];
uint32_t voltageSamples[VOLTAGE_SAMPLE_AMOUNT];
float currentAvgsum = 0;
int count;
float voltage = 0;
float w = 0;
float buffer = 0;

void SamplingCANTransmitThd(void const *argument)
{
    CAN_TxHeaderTypeDef pHeader;
    static volatile uint8_t data[8];
    volatile static float current;

    while (1)
    {
        voltage = HAL_SDADC_GetValue(&hsdadc1) / 357.25;
        if (count)
            current = currentAvgsum / count;
        currentAvgsum = 0;
        count = 0;

        w = voltage * current;
        if (w <= 20)
        {
            buffer += (float)(20 - w) * SAMPLE_INTERVAL_MS / 1000;
            if (buffer > 200.0)
                buffer = 200.0;
        }
        else
        {
            buffer -= (float)(w - 20) * SAMPLE_INTERVAL_MS / 1000;
            if (buffer < 0.0)
                buffer = 0.0;
        }

        *(float *)data = current;
        *(float *)&data[4] = voltage;

        pHeader.StdId = 0x301;
        // pHeader.ExtId =
        pHeader.IDE = CAN_ID_STD;
        pHeader.RTR = CAN_RTR_DATA;
        pHeader.DLC = 4;
        pHeader.TransmitGlobalTime = DISABLE;

        uint32_t mailbox;
        HAL_CAN_AddTxMessage(&hcan, &pHeader, data, &mailbox);

        *(float *)data = buffer;
        pHeader.StdId = 0x302;
        // pHeader.ExtId =
        pHeader.IDE = CAN_ID_STD;
        pHeader.RTR = CAN_RTR_DATA;
        pHeader.DLC = 4;
        pHeader.TransmitGlobalTime = DISABLE;
        HAL_CAN_AddTxMessage(&hcan, &pHeader, data, &mailbox);
        osDelay(SAMPLE_INTERVAL_MS);
    }
}

void SamplingStart(void)
{
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)dacValue);

    HAL_SDADC_CalibrationStart(&hsdadc3, SDADC_CALIBRATION_SEQ_1);
    HAL_SDADC_PollForCalibEvent(&hsdadc3, HAL_MAX_DELAY);

    HAL_SDADC_CalibrationStart(&hsdadc1, SDADC_CALIBRATION_SEQ_1);
    HAL_SDADC_PollForCalibEvent(&hsdadc1, HAL_MAX_DELAY);

    HAL_SDADC_Start_DMA(&hsdadc3, currentSamples, CURRENT_SAMPLE_AMOUNT);
    HAL_SDADC_Start_DMA(&hsdadc1, voltageSamples, VOLTAGE_SAMPLE_AMOUNT);

    //start CAN current transmission thread
    osThreadDef(samplingCANTx, SamplingCANTransmitThd, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(samplingCANTx), NULL);
}

void HAL_SDADC_ConvCpltCallback(SDADC_HandleTypeDef *hsdadc)
{
    static int32_t currentSum = 0;
    static int32_t voltageSum = 0;
    static volatile float currentAvg = 0;

    if (hsdadc == &hsdadc3)
    { //current
        currentSum = 0;
        for (uint8_t i = 0; i < CURRENT_SAMPLE_AMOUNT; i++)
        {
            currentSum += (int16_t)(currentSamples[i] & 0xFFFF);
        }
        currentAvg = (-currentSum * 1.0 / CURRENT_SAMPLE_AMOUNT + 25.0) / 3885.5;
        currentAvgsum += currentAvg;
        count += 1;
        return;
    }
    if (hsdadc == &hsdadc1)
    { //voltage
        voltageSamples[0]++;
        return;
    }
}
