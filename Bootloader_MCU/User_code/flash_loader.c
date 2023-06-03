#include "flash_loader.h"  //for parsing msg from RPi and write Flash

#include <string.h>

#include "work_with_flash.h"  //for work with flash (read write + serial)
//: 020000040800F2
//: 10000000000003202D0101084D5200085152000844

// format (all data in ASCII->hex '10'=>0x10) in first msg we have extended adress 0x0800 + offset creating right adress
//':' Nbytes_2s offset_4s type_2s data_Nbytes CRC_2s
//: 02 0000 04 0800 F2
//: 10 0000 00 000003202D0101084D52000851520008 44

// calculate CRC_16
uint16_t CRC_16(const uint8_t *data_p, uint16_t length) {
  uint16_t x;
  uint16_t crc = 0xFFFF;

  while (length--) {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
  }
  return crc;
}

// Array for receiving ans parsing msg
union DataRx {
  struct DataForSendHex {
    uint8_t HeaderArr_[4];   //'boot'
    uint16_t Offset_;        // bias?
    uint8_t DataArr_[1024];  // 1024kB
    uint16_t CRC16_;
  } sDataRx;
  uint8_t ArrRx[1032];
};

// receive data by 1024B
// prepare for page write
// CREATE AND USE ONLY IN FUNCTION FOR CLEAR
union ParserMCU {
#pragma pack(push, 1)
  uint32_t Arr1k_32b[32][1024];  // uint32 *32 *1024
  // struct Sectors32k {
  //   uint8_t Arr32k[1024 * 32][4];
  // } sSectors32k;
  // struct Sectors64k {
  //   uint8_t Arr32k[1024 * 64][2];
  // } sSectors64k;

  uint32_t Arr128k_32b[1][1024 * 32];
  uint8_t Arr[1024 * 128];
  uint8_t Arr1k[128][1024];
#pragma pack(pop)
};

extern UART_HandleTypeDef huart2;
bool Updater() {
  union ParserMCU uParserMCU;
  union DataRx uDataRx;
  uint8_t AnsArrTx[5] = {'b', 'o', 'o', 't', 'x'};
  uint16_t page1kcnt = 0;
  bool ReceivingHex = true;
  while (ReceivingHex) {
    // for (unsigned int i = 0; i < 1024; i++) { //dont need  it because we are rewrite it
    //   uDataRx.ArrRx[i] = 0;
    // }
    HAL_UART_Receive(&huart2, &uDataRx.ArrRx[0], 1032, 0xffff);

    uint16_t CRC16c = CRC_16(&uDataRx.ArrRx[0], 1030);                              // crc calc
    if (uDataRx.sDataRx.CRC16_ == CRC16c) {                                         // compare
      memcpy(&uParserMCU.Arr1k[page1kcnt][0], &uDataRx.sDataRx.DataArr_[0], 1024);  // copy data
      page1kcnt++;
      AnsArrTx[4] = 0x01;
      HAL_UART_Transmit(&huart2, &AnsArrTx[0], 5, 0xffff);
    } else {
      AnsArrTx[4] = 0x00;
      HAL_UART_Transmit(&huart2, &AnsArrTx[0], 5, 0xffff);
    }

    // if last msg
    if ((uDataRx.sDataRx.HeaderArr_[0] == 'e') && (uDataRx.sDataRx.HeaderArr_[3] == 'b')) {  //'endb'
      ReceivingHex = false;
      break;
    }
  }

  // write flash
  Write_To_flash(0x08020000, FLASH_SECTOR_5, &uParserMCU.Arr128k_32b[0][0], (32 * 1024));

  AnsArrTx[4] = 2;
  HAL_UART_Transmit(&huart2, &AnsArrTx[0], 5, 0xffff);
  return true;
}
