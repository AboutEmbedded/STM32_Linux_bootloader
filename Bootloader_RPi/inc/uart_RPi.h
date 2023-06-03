#ifndef __UART_RPi_H__
#define __UART_RPi_H__

#include <fcntl.h>    //modify files
#include <stdio.h>    //printf putchar
#include <stdlib.h>   //exit(EXIT_FAILURE);
#include <termios.h>  //for com port
#include <unistd.h>   //read, write

#include "stdbool.h"  //add bool
#include "stdint.h"   //add int

/// @brief function for open serial port
/// @param g_fileDesc global/external dile descriptor
/// @param UART_Path path to the UART (foe example: "/dev/ttyUSB0")
void open_serial_port(int* g_fileDesc, const char* UARTfile);

/// @brief function for configuring serial port
/// @param g_fileDesc global/external dile descriptor
/// @param g_tty struct with preferences
void configure_serial_port(int g_fileDesc, struct termios* g_tty);

/// @brief clese serial port
/// @param g_fileDesc global/external dile descriptor
void close_serial_port(int g_fileDesc);

/// @brief function for write bytes in UART
/// @param g_fileDesc global/external dile descriptor
/// @param buff buffer with data for send
/// @param len_buff length of array
/// @return the number written, or -1.
int file_write_data(int g_fileDesc, uint8_t* buff, uint32_t len_buff);

/// @brief function for read bytes from UART
/// @param g_fileDesc global/external dile descriptor
/// @param buff buffer with data for send
/// @param len_buff length of array
/// @return the number written, or -1.
int file_read_data(int g_fileDesc, uint8_t* buff, uint32_t len_buff);

#endif /* __UART_RPi_H__ */