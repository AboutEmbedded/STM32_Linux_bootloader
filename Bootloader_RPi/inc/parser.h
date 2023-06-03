#ifndef __PARSER_H__
#define __PARSER_H__

#include <fcntl.h>    //modify files
#include <stdio.h>    //printf putchar
#include <stdlib.h>   //exit(EXIT_FAILURE);
#include <string.h>   //memcpy
#include <termios.h>  //for uart
#include <unistd.h>   //read, write

#include "stdbool.h"  //add bool
#include "stdint.h"   //add int
#include "time.h"

/// @brief function for parsing hex file and loading in MCU
/// @param HexFile path to the hex file
/// @param UART_FileDesc file descriptor for UART
/// @return -1 if unsuccess flash write, 0 if success
int FlashLoader(const char* HexFile, int UART_FileDesc);
#endif /* __PARSER_H__ */