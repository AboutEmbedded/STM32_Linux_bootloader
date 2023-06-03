
#include "jump_addr.h"  //for jump into main program

#define def_APP_START_ADDR 0x08020000  // recheck this adress in hex 0x08001A60
void JumpNewAddr() {
  uint32_t jumpAdress;
  // var for function jump adress
  jumpAdress = *((volatile uint32_t *)(def_APP_START_ADDR + 4));  // start addr + offset (reset)

  // HAL_RCC_DeInit();  // reset RCC
  // HAL_DeInit();      // reset HAL

  void (*GoToApp)(void);                 // pointer on the new start function
  GoToApp = (void (*)(void))jumpAdress;  // set this adress as new function adress

  // GoToApp = gpioToggle;

  // gpioToggle();

  __disable_irq();                                        // disable nvic
  __set_MSP(*((volatile uint32_t *)def_APP_START_ADDR));  // change stack pointer on new firmware
  SCB->VTOR = def_APP_START_ADDR;                         // change NVIC vector adress on new firmware
  GoToApp();                                              // go on the new instruction adress
}