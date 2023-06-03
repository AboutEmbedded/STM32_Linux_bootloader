/* USER CODE BEGIN Header */
/*
#include "flash_loader.h" //for parsing msg from RPi and write Flash
*/
#ifndef __MY_FLASH_LOADER_H__
#define __MY_FLASH_LOADER_H__

#include "main.h"
#include "stdbool.h"
#include "stdint.h"

/// @brief function for update firmware
/// @return return true if update was succesfull
bool Updater();

#endif /* __MY_FLASH_LOADER_H__ */
       /* USER CODE END Header */