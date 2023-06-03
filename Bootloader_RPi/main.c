#include "parser.h"
#include "uart_RPi.h"

// #define SERIAL_PORT_PATH "/dev/ttyS0"

#define Dev_MyLaptop false
#if Dev_MyLaptop == true
const char Uart[] = "/dev/ttyUSB0";  // my adapter
#else
const char Uart[] = "/dev/ttyAMA3";  // on the RPi
#endif

char HexFile[] = "./Firmware.hex";

struct UARTpref {
  int fileDesc;
  struct termios str_tty;
  char UartPath[13];
} sUARTpref;

int main(void) {
  printf("Starting the loopback application...\r\n");
  struct UARTpref sUARTpref;
  memcpy(&sUARTpref.UartPath[0], &Uart[0], 13);

  open_serial_port(&sUARTpref.fileDesc, sUARTpref.UartPath);
  if (sUARTpref.fileDesc < 0) {
    return -1;  // file busy
  }

  configure_serial_port(sUARTpref.fileDesc, &sUARTpref.str_tty);

  uint8_t ArrStartBoot[12] = "mcu;bt;none";
  file_write_data(sUARTpref.fileDesc, &ArrStartBoot[0], (sizeof(ArrStartBoot) - 1));  // send packet
  int FlashLoaded = FlashLoader(&HexFile[0], sUARTpref.fileDesc);  // parser from hex file and sending in MCU

  if (FlashLoaded == 0) {
    printf("Flash was recorded\r\n");
  } else {
    printf("Flash errors\r\n");
  }

  printf("Close port\r\n");
  close_serial_port(sUARTpref.fileDesc);

  return FlashLoaded;
}
