/* USER CODE BEGIN Header */
/*
#include "parser_cmd.h" //for parsing CMD from RPi
*/
#ifndef __MY_PARSER_CMD_H__
#define __MY_PARSER_CMD_H__

#include "main.h"
#include "stdbool.h"
#include "stdint.h"

union DataParse {
  struct DataRx {
    uint8_t Header[4];  // mcu;
    uint16_t CMD;       // xx
    uint8_t End[6];     //;none+\0
  } sCMDDataRx;
  uint8_t DataRx[12];
};
extern union DataParse uDataParse;

/// @brief function for parsing command from RPi
/// @return true if we must start main program
bool ParserCMD(void);

#endif /* __MY_PARSER_CMD_H__ */
/* USER CODE END Header */
