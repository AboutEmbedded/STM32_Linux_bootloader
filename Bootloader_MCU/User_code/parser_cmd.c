
#include "parser_cmd.h"  //for parsing CMD from RPi

#include "flash_loader.h"     //for parsing msg from RPi and write Flash
#include "work_with_flash.h"  //for work with flash (read write + serial)
union DataParse uDataParse;

bool ParserCMD() {
  bool parserEND = false;
  switch (uDataParse.sCMDDataRx.CMD) {
    case 0x7462:              //"mcu;bt;none"; //little endian
      parserEND = Updater();  // update firmware
      break;
    case 0x7473:  //"mcu;st;none";
      parserEND = true;
      break;

    default:
      break;
  }
  return parserEND;
}
