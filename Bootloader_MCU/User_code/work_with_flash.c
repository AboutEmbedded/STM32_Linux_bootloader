
#include "work_with_flash.h"  //for work with flash (read write + serial)

void Erase_Flash(uint32_t StartSector) {
  int i = 0;
  HAL_FLASH_Unlock();  // разблокировать флеш
  FLASH_Erase_Sector(StartSector, FLASH_VOLTAGE_RANGE_3);
  while ((HAL_FLASH_GetError() != HAL_FLASH_ERROR_NONE) && (i < 10)) {
    i++;
    HAL_Delay(200);
  }
  HAL_FLASH_Lock();  // заблокировать флеш
}

void Write_To_flash(uint32_t StartAdress, uint32_t StartSector, uint32_t *DataArr, uint32_t SizeData) {
  Erase_Flash(StartSector);  // затираем флешку
  HAL_FLASH_Unlock();        // разблокировали флешку для записи

  // Для облегчения чтения
  uint32_t StartPageWrite = StartAdress;
  uint8_t flash_ok = HAL_ERROR;

  for (uint32_t i = 0; i < SizeData; i++) {
    flash_ok = HAL_ERROR;
    uint32_t buf = *(DataArr + i);
    while (flash_ok != HAL_OK) {
      // FLASH_TYPEPROGRAM_WORD - 32bit число
      flash_ok = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageWrite, buf);
    }
    StartPageWrite += 4;
  }

  HAL_FLASH_Lock();  // заблокировали флешку
}

