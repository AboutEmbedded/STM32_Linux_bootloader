#include "parser.h"

#include "uart_RPi.h"

/*
'0'=48  '1'=49  '2'=50 ... '9'=57
'A'=65  'B'=66 ... 'F'=70
65-49=16
*/
// convert ASCII to decimal for 1 symbol
uint8_t AsciiToDec_1sym(char* ASCII_sym) {
  uint8_t result = 0;
  if ((*ASCII_sym) < 60) {
    result = (*ASCII_sym) - 48;
  } else {
    result = (*ASCII_sym) - 55;  // 65=10
  }
  return result;
}

// convert ASCII to decimal for pair (2 symbols)
uint8_t AsciiToDec_byte(char* ASCII_sym) {
  uint8_t OneByteResult = AsciiToDec_1sym(ASCII_sym) * 16;
  OneByteResult += AsciiToDec_1sym((ASCII_sym + 1));

  return OneByteResult;
}

// calculate CRC_16
uint16_t CRC_16(const uint8_t* data_p, uint16_t length) {
  uint16_t x;
  uint16_t crc = 0xFFFF;

  while (length--) {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
  }
  return crc;
}

/// struct for parsing msg from hex file
struct hex_parser {
  uint8_t Nbytes_;       // counts of bytes
  uint16_t Offset_;      // offset
  uint8_t Type_;         // type of command
  uint8_t DataArr_[16];  // data array
  uint8_t CRC_;          // crc
} shex_parser;

// convert string from hex file to bytes
// return false if no mistakes
bool hex_str_parser(struct hex_parser* str_hex_parser, char* hexstr) {
  uint8_t CRC_check = 0;

  str_hex_parser->Nbytes_ = AsciiToDec_byte(hexstr + 1);
  CRC_check += str_hex_parser->Nbytes_;  // for crc calculation

  uint8_t buf = 0;
  buf = AsciiToDec_byte(hexstr + 1 + 2);
  CRC_check += buf;
  str_hex_parser->Offset_ = buf * 256;

  buf = AsciiToDec_byte(hexstr + 1 + 4);
  CRC_check += buf;
  str_hex_parser->Offset_ += buf;

  str_hex_parser->Type_ = AsciiToDec_byte(hexstr + 1 + 6);
  CRC_check += str_hex_parser->Type_;

  for (uint8_t i = 0; i < str_hex_parser->Nbytes_; i++) {
    str_hex_parser->DataArr_[i] = AsciiToDec_byte(hexstr + 1 + 8 + (2 * i));
    CRC_check += str_hex_parser->DataArr_[i];
  }

  str_hex_parser->CRC_ = AsciiToDec_byte(hexstr + 1 + 8 + 2 * str_hex_parser->Nbytes_);  //+1+10+...-2
  CRC_check += str_hex_parser->CRC_;
  return CRC_check;
}

/*
:020000040800F2
// format (all data in ASCII->hex '10'=>0x10) in first msg we have extended adress 0x0800 + offset creating right adress
//':' Nbytes_2s offset_4s type_2s data_Nbytes CRC_2s
//: 02 0000 04 0800 F2
//: 10 0000 00 000003202D0101084D52000851520008 44
*/

uint8_t g_ArrBig[1024 * 128];  // global structure foe parsing firmware

// function for farsing data from hex
// HexFileDesc file descriptor for firmware
// UART_FileDesc file descriptor for uart
// g_CntBytesInHex counter of bytes for load in mcu
// return 0 if ready, -1 if file unavailable
int ParseDataFromHex(int HexFileDesc, int UART_FileDesc, uint32_t* g_CntBytesInHex) {
  int result = 0;
  // проверка что файл существует?
  memset(&g_ArrBig[0], 0xFF, 1024 * 128);  // чистим память

  uint32_t CntBytesInHex = 0;  // counter for all bytes in hex
  char ch = 0;                 // current char variable
  uint8_t cnt = 0;             // counter for symbols in row
  ssize_t ret;                 // returned value
  char ArrBuf[44] = {'\0'};

  while (((ret = read(HexFileDesc, &ch, 1)) > 0)) {
    if (ch == ':') {
      cnt = 0;
      for (int i = 0; i < 44; i++) {
        ArrBuf[i] = '\0';
      }
    }

    if (ch == '\r') {
      memset(&shex_parser.DataArr_[0], 0xFF, 16);
      bool CRCcheckErr = hex_str_parser(&shex_parser, &ArrBuf[0]);
      cnt = 0;

      if (shex_parser.Type_ == 0) {  // if its data
        if (!CRCcheckErr) {
          static uint32_t offset32b = 0;
          static uint32_t offset_old = 0;
          offset32b = (offset32b & 0xffff0000) + shex_parser.Offset_;  // create bias
          if ((offset_old & 0x0000FFFF) > (offset32b & 0x0000FFFF)) {  // if next 64k, then +1
            offset32b = offset32b + 0x00010000;
          }
          // копируем в большую структуру
          memcpy(&g_ArrBig[offset32b], &shex_parser.DataArr_[0], shex_parser.Nbytes_);
          CntBytesInHex += shex_parser.Nbytes_;
          offset_old = offset32b;  // old bias
        }

        memset(&shex_parser.DataArr_[0], 0xFF, 16);
      }
    } else if (ch != '\n') {
      ArrBuf[cnt] = ch;
      cnt++;
    }
  }
  *g_CntBytesInHex = CntBytesInHex;
  return result;
}

// function for receiving answer from device
// UART_FileDesc -file descriptor for UART
// partTx - part of transmitted firmware (1k)
// return int value -1 if repeat (autoreload), 1-success, 2 - flash was recorded
int ReceiveAnswer(int UART_FileDesc, int* partTx) {
  uint8_t compare = 0;  //"boot0 boot1"
  char ArrRx[6] = {"*****"};
  uint8_t ArrRxAnsGood[] = {'b', 'o', 'o', 't'};

  // receive answer
  while (compare != 4) {
    int buf = file_read_data(UART_FileDesc, &ArrRx[0], 5);
    if (ArrRx[0] == 'b') {
      for (uint8_t i = 0; i < 4; i++) {
        if (ArrRx[i] == ArrRxAnsGood[i]) {
          compare++;
        }
      }
    }
    // sleep(1);
  }

  int retValue = 0;

  switch (ArrRx[4]) {
    case 0:                      // load part unsuccess
      printf(" repeat...\r\n");  // repeat
      retValue = -1;
      break;
    case 1:                            // load part success
      printf(" success load...\r\n");  // next step;
      *(partTx) = *partTx + 1;
      retValue = 1;
      break;
    case 2:  // flash was wrote
      retValue = 2;
      break;

    default:
      break;
  }
  return retValue;
}

//--------
// structure for prepare and send data to MCU
union ParserTxSend {
  struct DataForSendHex {    // 4+2+1024+2=1024+8=1032
    uint8_t HeaderArr_[4];   //'boot'
    uint16_t Offset_;        // bias - not used now (jnly 1 page)
    uint8_t DataArr_[1024];  // 1024kB
    uint16_t CRC16_;
  } sDataForSendHex;
  uint8_t Arr[1032];
};
//--------

int FlashLoader(const char* HexFile, int UART_FileDesc) {
  union ParserTxSend uParserTxSend;
  int g_hexfileDesc = open(HexFile, O_RDONLY);  // get file descriptor
  uint32_t g_CntBytesInHex;
  ParseDataFromHex(g_hexfileDesc, UART_FileDesc, &g_CntBytesInHex);

  // count numbers of 1k parts
  unsigned int part1k_Tx = 0;  // variable for 1k transmitted parts
  unsigned int PartsCount = g_CntBytesInHex / 1024;
  if ((g_CntBytesInHex % 1024) != 0) {
    PartsCount += 1;
  }
  while (part1k_Tx < PartsCount) {
    // create header
    // memcpy(&un_ParserTxSend->sDataForSendHex.HeaderArr_[0], "boot", 4);  // copy header
    if (part1k_Tx == (PartsCount - 1)) {
      printf("last msg... ");                                           // last step;
      memcpy(&uParserTxSend.sDataForSendHex.HeaderArr_[0], "endb", 4);  // header for last msg
    } else {
      memcpy(&uParserTxSend.sDataForSendHex.HeaderArr_[0], "boot", 4);  // copy header
    }

    // copy data
    for (int i = 0; i < 1024; i++) {
      uParserTxSend.sDataForSendHex.DataArr_[i] = g_ArrBig[1024 * part1k_Tx + i];
    }

    // send
    uParserTxSend.sDataForSendHex.CRC16_ = CRC_16(&uParserTxSend.Arr[0], 1030);  // calc crc
    file_write_data(UART_FileDesc, &uParserTxSend.Arr[0], 1032);                 // send packet

    // receive answer, if dont successful-repeat msg
    ReceiveAnswer(UART_FileDesc, &part1k_Tx);  // receive answer
  }
  int ans = ReceiveAnswer(UART_FileDesc, &part1k_Tx);  // receive answer
  if (ans == 2) {
    ans = 0;
  } else {
    ans = -1;
  }

  close(g_hexfileDesc);
  return ans;
}
