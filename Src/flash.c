/**
 * @file    flash.c
 * @author	Alex Wong Tat Hang (thwongaz@connect.ust.hk)
 * @brief   Driver for the cybathlon sensor node parameter storage in flash
 * @version 0.1
 * @date	2019-2-6
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "main.h"
#include <string.h>
#include "flash.h"


/**
 * @brief	flag controlling the save funtion of the flash module
 * @usage	set to 1 (FLASH_SAVE) to start a save operation, can be set through debug interface
 * @detail	flag is set to 2 (FLASH_SAVING) by program when a save operation is on going
 *          reverts to 0 (FLASH_SAVE_READY) if flash save is successful
 *          in case if the flag is set to error values (FLASH_SAVE_ERROR_xxx), consult
 *          the definition of flashSaveStatus_e in flash.h for diagnostics
 */
flashSaveStatus_e gFlashSaveFlag;

/**
 * @brief   freeRTOS thread definition stuff
 */
// osThreadId flashSaveThreadHandle;
// uint32_t flashSaveThreadBuffer[256];
// osStaticThreadDef_t flashSaveThreadControlBlock;

/**
 * @brief	dataset used for testing the flash module
*/
uint8_t byteTest = 0;
uint16_t halfWordTest = 0;
uint32_t wordTest = 0;
uint64_t doubleWordTest = 0;
float floatTest = 0;
double doubleTest = 0;

/**
 * @brief   helper to translate FLASH_Type_Program to memory size
 * @warning	do not modify
 */
static uint32_t flashDataTypeToSize[] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
    sizeof(uint64_t)
};

/**
 * @brief	list pf parameters to save into flash
 * @warning	only extend this list, never modify of delete the existing entries
 *          terminate with {NULL, 0}
 * 
 * @detail  data types of the following data sizes are supported
 *          FLASH_TYPEPROGRAM_BYTE	        (8 bit)
 *          FLASH_TYPEPROGRAM_HALFWORD	    (16 bit)
 *          FLASH_TYPEPROGRAM_WORD	        (32 bit)
 *          FLASH_TYPEPROGRAM_DOUBLEWORD    (64-bit) 
 */
const flashParamEntry_t savedParameters[] = {
    {&byteTest, FLASH_TYPEPROGRAM_BYTE},             //For testing, place at the end before the NULL termination
    {&halfWordTest, FLASH_TYPEPROGRAM_HALFWORD},     //For testing, place at the end before the NULL termination
    {&wordTest, FLASH_TYPEPROGRAM_WORD},             //For testing, place at the end before the NULL termination
    {&doubleWordTest, FLASH_TYPEPROGRAM_DOUBLEWORD}, //For testing, place at the end before the NULL termination
    {&floatTest, FLASH_TYPEPROGRAM_WORD},            //For testing, place at the end before the NULL termination
    {&doubleTest, FLASH_TYPEPROGRAM_DOUBLEWORD},     //For testing, place at the end before the NULL termination
    {NULL, 0}};

/**
 * @brief	for testing only
 */
const flashParamEntry_t testParameters[] = {
    {&byteTest, FLASH_TYPEPROGRAM_BYTE},             //For testing, place at the end before the NULL termination
    {&halfWordTest, FLASH_TYPEPROGRAM_HALFWORD},     //For testing, place at the end before the NULL termination
    {&wordTest, FLASH_TYPEPROGRAM_WORD},             //For testing, place at the end before the NULL termination
    {&doubleWordTest, FLASH_TYPEPROGRAM_DOUBLEWORD}, //For testing, place at the end before the NULL termination
    {&floatTest, FLASH_TYPEPROGRAM_WORD},            //For testing, place at the end before the NULL termination
    {&doubleTest, FLASH_TYPEPROGRAM_DOUBLEWORD},     //For testing, place at the end before the NULL termination
    {NULL, 0}
};

/**
 * @brief	setting for erasing the flash page used for storing parameters
 * @warning do not modify
 */
FLASH_EraseInitTypeDef userPageErase = {
    FLASH_TYPEERASE_PAGES,  //Erase pages instead of mass erase
    FLASH_PAGE_ADDR,      //Erase last flash page
    1                       //Erase 1 page only
};

/**
 * @brief	save the parameter list to FLASH_SECTOR
 * @param	paramList	list of parameters to save, defaults to savedParameters if NULL is passed
 */
uint8_t flashSave(const flashParamEntry_t *paramList)
{
    if (paramList == NULL)
        paramList = savedParameters; //Load default parameter list if no list is specified
    HAL_SuspendTick();
    gFlashSaveFlag = FLASH_SAVING;
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        gFlashSaveFlag = FLASH_SAVE_ERROR_UNLOCK;
        return 1;   //Flash unlock error
    }
    
    static uint32_t pageError = 0;
    // __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
    //                        FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    if (HAL_FLASHEx_Erase(&userSectorErase, &pageError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        gFlashSaveFlag = FLASH_SAVE_ERROR_ERASE;
        return 1; //Flash erase error
    }

    HAL_StatusTypeDef status = HAL_OK;
    static uint32_t flashAddress;
    flashAddress = FLASH_PAGE_ADDR;
    static uint64_t tempData;    
    while ((status == HAL_OK))
    {
        if (!IS_FLASH_TYPEPROGRAM(paramList->dataType))
        {
            HAL_FLASH_Lock();
            gFlashSaveFlag = FLASH_SAVE_ERROR_INVALID_DATA_TYPE;
            return 1; //Invalid dataType
        }
        if (paramList->dataType == FLASH_TYPEPROGRAM_DOUBLEWORD) 
        {
            memcpy(&tempData, paramList->data, flashDataTypeToSize[FLASH_TYPEPROGRAM_WORD]);
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress, tempData);
            memcpy(&tempData, paramList->data + sizeof(uint32_t), flashDataTypeToSize[FLASH_TYPEPROGRAM_WORD]);
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashAddress + sizeof(uint32_t), tempData);
        } else {
            memcpy(&tempData, paramList->data, flashDataTypeToSize[paramList->dataType]);
            status = HAL_FLASH_Program(paramList->dataType, flashAddress, tempData);
        }
        paramList++;
        flashAddress += sizeof(uint64_t);
        if (paramList->data == NULL)
            break;
    }

    HAL_FLASH_Lock();
    if (status != HAL_OK) {
        gFlashSaveFlag = FLASH_SAVE_ERROR_WRITE;
        return 1; //Flash write error
    }
    gFlashSaveFlag = FLASH_SAVE_READY;
    HAL_ResumeTick();
    return 0;
}

/**
 * @brief	load the parameter list from FLASH_SECTOR
 * @param	paramList	list of parameters to load, defaults to savedParameters if NULL is passed
 */
void flashLoad(const flashParamEntry_t *paramList)
{
    if(paramList == NULL)
        paramList = savedParameters;    //Load default parameter list if no list is specified
    static void *flashAddress;
    flashAddress = (void *)FLASH_PAGE_ADDR;
    while(paramList->data != NULL)
    {
        memcpy(paramList->data, flashAddress, flashDataTypeToSize[paramList->dataType]);
        paramList++;
        flashAddress += sizeof(uint64_t);
    }
}

/**
 * @brief	polls gFlashSaveFlag, saves savedParameters if gFlashSaveFlag is set to FLASH_SAVE
 */
void flashSaveCheck(void)
{        
    if(gFlashSaveFlag == FLASH_SAVE)
    {
        flashSave(savedParameters);
    }    
}