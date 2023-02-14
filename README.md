## NMEA library for STM32 LL
*	Author:     Nima Askari
*	WebSite:    https://www.github.com/NimaLTD
*	Instagram:  https://www.instagram.com/github.NimaLTD
*	LinkedIn:   https://www.linkedin.com/in/NimaLTD
*	Youtube:    https://www.youtube.com/@nimaltd
--------------------------------------------------------------------------------
* http://github.com/nimaltd/ws281x
-----------------------------------------------------------   
* This library using HEAP memory. If could not init, increase your HEAP size. (35 byte per pixel)
* Enable SPI and DMA.
* Set MSB with 3.3MHz of speed.
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
// 216 * 35 = 7560 bytes heap usage
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
![image](https://user-images.githubusercontent.com/4112219/218713303-9bbe7af3-d65d-4c3a-b77d-9a382d98a9ea.png)
