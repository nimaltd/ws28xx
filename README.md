## NMEA library for STM32 LL
*	Author:     Nima Askari
*	WebSite:    https://www.github.com/NimaLTD
*	Instagram:  https://www.instagram.com/github.NimaLTD
*	LinkedIn:   https://www.linkedin.com/in/NimaLTD
*	Youtube:    https://www.youtube.com/@nimaltd
--------------------------------------------------------------------------------
* http://github.com/nimaltd/ws281x
-----------------------------------------------------------   
* Enable SPI and DMA.
* Set MSB and 3.3MHz of speed.
* Create a struct.
* Call Init.
* Don't forget to use ws281x_update after set pixels.
--------------------------------------------------------------------------------
## RTOS example:
```
#include "ws281x.h"

ws281x_t pixel;

int main()
{
  ...  
}

void task_other(void const * argument)
{
  ws281x_init(&pixel, &hspi1, 216, ws281x_order_grb);
  ws281x_test_slow(&pixel);
  for(;;)
  {
    osDelay(1);
  }
}
```
