#include "uart_RPi.h"

// function for open file and get file descriptor
// fname array with filename
// return file descriptor of UART
int file_open_and_get_descriptor(const char *fname) {
  int fd;  // buf variable

  fd = open(fname, O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    printf("Could not open file %s...%d\r\n", fname, fd);
  }
  return fd;
}

// Write N bytes of BUF to FD. Return the number written, or -1.
// This function is a cancellation point and therefore not marked with __THROW.
int file_write_data(int g_fileDesc, uint8_t *buff, uint32_t len_buff) { return write(g_fileDesc, buff, len_buff); }

// Read NBYTES into BUF from FD. Return the number read, -1 for errors or 0 for EOF.
// This function is a cancellation point and therefore not marked with__THROW.
int file_read_data(int g_fileDesc, uint8_t *buff, uint32_t len_buff) { return read(g_fileDesc, buff, len_buff); }

// Close the file descriptor FD.
// This function is a cancellation point and therefore not marked with __THROW.
int file_close(int g_fileDesc) { return close(g_fileDesc); }

// function for open serial port
// g_fileDesc -file descriptor
// UART_Path - path to the uart file
void open_serial_port(int *g_fileDesc, const char *UART_Path) {
  *g_fileDesc = file_open_and_get_descriptor(UART_Path);
  if (*g_fileDesc < 0) {
    printf("Something went wrong while opening the port...\r\n");
    exit(EXIT_FAILURE);
  }
}

// function for configuration serial port
// g_fileDesc -file descriptor for UART
// g_tty - pointer on the termios struct for UART
void configure_serial_port(int g_fileDesc, struct termios *g_tty) {
  if (tcgetattr(g_fileDesc, g_tty)) {
    printf("Something went wrong while getting port attributes...\r\n");
    exit(EXIT_FAILURE);
  }

  cfsetispeed(g_tty, B115200);
  cfsetospeed(g_tty, B115200);

  cfmakeraw(g_tty);

  if (tcsetattr(g_fileDesc, TCSANOW, g_tty)) {
    printf("Something went wrong while setting port attributes...\r\n");
    exit(EXIT_FAILURE);
  }
}

// function for close serial port
// g_fileDesc file descriptor for UART
void close_serial_port(int g_fileDesc) { file_close(g_fileDesc); }

// function for sending data in UART
// ArrTx -pointer on the data for send
// size - counts of bytes for send
void SendinUart(uint8_t *ArrTx, uint8_t size) {
  for (char i = 0; i < size; i++) {
    putchar(ArrTx[i]);
  }
}
