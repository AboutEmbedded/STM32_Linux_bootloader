/* USER CODE BEGIN Header */
/*
#include "work_with_flash.h" //for work with flash (read write + serial)
*/
#ifndef __WORK_WITH_FLASH_H__
#define __WORK_WITH_FLASH_H__

#include "main.h"
#include "stdbool.h"
#include "stdint.h"

void Erase_Flash(uint32_t StartSector);

void Write_To_flash(uint32_t StartAdress, uint32_t StartSector, uint32_t *DataArr, uint32_t SizeData);

#endif /* __WORK_WITH_FLASH_H__ */
       /* USER CODE END Header */