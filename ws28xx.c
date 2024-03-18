
/************************************************************************************************************
**************    Include Headers
************************************************************************************************************/

#include "ws28xx.h"
#include <string.h>

#if WS28XX_RTOS == WS28XX_RTOS_DISABLE
#elif WS28XX_RTOS == WS28XX_RTOS_CMSIS_V1
#include "cmsis_os.h"
#include "freertos.h"
#elif WS28XX_RTOS == WS28XX_RTOS_CMSIS_V2
#include "cmsis_os2.h"
#include "freertos.h"
#elif WS28XX_RTOS == WS28XX_RTOS_THREADX
#include "app_threadx.h"
#endif

/************************************************************************************************************
**************    Private Definitions
************************************************************************************************************/

// none

/************************************************************************************************************
**************    Private Variables
************************************************************************************************************/

#if (WS28XX_GAMMA == true)
const uint8_t WS28XX_GammaTable[] =
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
  2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
  6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
  11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
  19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
  29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
  40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
  90, 91, 93, 94, 95, 96, 98, 99,100,102,103,104,106,107,109,110,
  111,113,114,116,117,119,120,121,123,124,126,128,129,131,132,134,
  135,137,138,140,142,143,145,146,148,150,151,153,155,157,158,160,
  162,163,165,167,169,170,172,174,176,178,179,181,183,185,187,189,
  191,193,194,196,198,200,202,204,206,208,210,212,214,216,218,220,
  222,224,227,229,231,233,235,237,239,241,244,246,248,250,252,255
};
#endif

/************************************************************************************************************
**************    Private Functions
************************************************************************************************************/

void WS28XX_Delay(uint32_t Delay);
void WS28XX_Lock(WS28XX_HandleTypeDef *Handle);
void WS28XX_UnLock(WS28XX_HandleTypeDef *Handle);

/***********************************************************************************************************/

void WS28XX_Delay(uint32_t Delay)
{
#if WS28XX_RTOS == WS28XX_RTOS_DISABLE
  HAL_Delay(Delay);
#elif (WS28XX_RTOS == WS28XX_RTOS_CMSIS_V1) || (WS28XX_RTOS == WS28XX_RTOS_CMSIS_V2)
  uint32_t d = (configTICK_RATE_HZ * Delay) / 1000;
  if (d == 0)
      d = 1;
  osDelay(d);
#elif WS28XX_RTOS == WS28XX_RTOS_THREADX
  uint32_t d = (TX_TIMER_TICKS_PER_SECOND * Delay) / 1000;
  if (d == 0)
    d = 1;
  tx_thread_sleep(d);
#endif
}

/***********************************************************************************************************/

void WS28XX_Lock(WS28XX_HandleTypeDef *Handle)
{
  while (Handle->Lock)
  {
    WS28XX_Delay(1);
  }
  Handle->Lock = 1;
}

/***********************************************************************************************************/

void WS28XX_UnLock(WS28XX_HandleTypeDef *Handle)
{
  Handle->Lock = 0;
}

/************************************************************************************************************
**************    Public Functions
************************************************************************************************************/

/**
  * @brief  Initialize WS28XX handle
  * @note   Initialize WS28XX handle and set the Channel and number of Pixels
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  *HTim: Pointer to TIM_HandleTypeDef structure
  * @param  Channel: Selected PWM channel
  *         TIM_CHANNEL_1
  *         TIM_CHANNEL_2
  *         TIM_CHANNEL_3
  *         TIM_CHANNEL_4
  *         TIM_CHANNEL_5
  *         TIM_CHANNEL_6
  * @param  Pixel: Number of pixels
  *
  * @retval bool: true or false
  */
bool WS28XX_Init(WS28XX_HandleTypeDef *Handle, TIM_HandleTypeDef *HTim,
      uint16_t TimerBusFrequencyMHz, uint8_t Channel, uint16_t Pixel)
{
  bool answer = false;
  uint32_t aar_value;
  do
  {
    if (Handle == NULL || HTim == NULL)
    {
      break;
    }
    if (Pixel > WS28XX_PIXEL_MAX)
    {
      break;
    }
    Handle->Channel = Channel;
    Handle->MaxPixel = Pixel;
    Handle->HTim = HTim;
    aar_value = (TimerBusFrequencyMHz / (1.0f / (WS28XX_PULSE_LENGTH_NS / 1000.0f))) - 1;
    __HAL_TIM_SET_AUTORELOAD(Handle->HTim ,aar_value);
    __HAL_TIM_SET_PRESCALER(Handle->HTim, 0);
    Handle->Pulse0 = ((WS28XX_PULSE_0_NS / 1000.0f) * aar_value) / (WS28XX_PULSE_LENGTH_NS / 1000.0f);
    Handle->Pulse1 = ((WS28XX_PULSE_1_NS / 1000.0f) * aar_value) / (WS28XX_PULSE_LENGTH_NS / 1000.0f);
    memset(Handle->Pixel, 0, sizeof(Handle->Pixel));
    memset(Handle->Buffer, 0, sizeof(Handle->Buffer));
    HAL_TIM_PWM_Start_DMA(Handle->HTim, Handle->Channel, (const uint32_t*)Handle->Buffer, Pixel);
    answer = true;
  }
  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Set Pixel
  * @note   Fill the pixel By RGB Values
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  Pixel: Pixel Starts from 0 to Max - 1
  * @param  Red: Red Value, 0 to 255
  * @param  Green: Green Value, 0 to 255
  * @param  Blue: Blue Value, 0 to 255
  *
  * @retval bool: true or false
  */
bool WS28XX_SetPixel_RGB(WS28XX_HandleTypeDef *Handle, uint16_t Pixel, uint8_t Red, uint8_t Green, uint8_t Blue)
{
  bool answer = true;
  do
  {
    if (Pixel >= Handle->MaxPixel)
    {
      answer = false;
      break;
    }
#if (WS28XX_GAMMA == false)
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = Red;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Blue;
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = Blue;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Red;
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = Green;
    Handle->Pixel[Pixel][1] = Red;
    Handle->Pixel[Pixel][2] = Blue;
#endif
#else
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Blue];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Red];
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#endif
#endif
  }

  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Set Pixel
  * @note   Fill the pixel By RGB Values
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  Pixel: Pixel Starts from 0 to Max - 1
  * @param  Color: RGB565 Color Code
  *
  * @retval bool: true or false
  */
bool WS28XX_SetPixel_RGB_565(WS28XX_HandleTypeDef *Handle, uint16_t Pixel, uint16_t Color)
{
  bool answer = true;
  uint8_t Red, Green, Blue;
  do
  {
    if (Pixel >= Handle->MaxPixel)
    {
      answer = false;
      break;
    }
    Red = ((Color >> 8) & 0xF8);
    Green = ((Color >> 3) & 0xFC);
    Blue = ((Color << 3) & 0xF8);
#if (WS28XX_GAMMA == false)
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = Red;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Blue;
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = Blue;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Red;
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = Green;
    Handle->Pixel[Pixel][1] = Red;
    Handle->Pixel[Pixel][2] = Blue;
#endif
#else
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Blue];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Red];
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#endif
#endif
  }

  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Set Pixel
  * @note   Fill the pixel By RGB Values
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  Pixel: Pixel Starts from 0 to Max - 1
  * @param  Color: RGB888 Color Code
  *
  * @retval bool: true or false
  */
bool WS28XX_SetPixel_RGB_888(WS28XX_HandleTypeDef *Handle, uint16_t Pixel, uint32_t Color)
{
  bool answer = true;
  uint8_t Red, Green, Blue;
  do
  {
    if (Pixel >= Handle->MaxPixel)
    {
      answer = false;
      break;
    }
    Red = ((Color & 0xFF0000) >> 16);
    Green = ((Color & 0x00FF00) >> 8);
    Blue = (Color & 0x0000FF);
#if (WS28XX_GAMMA == false)
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = Red;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Blue;
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = Blue;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Red;
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = Green;
    Handle->Pixel[Pixel][1] = Red;
    Handle->Pixel[Pixel][2] = Blue;
#endif
#else
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Blue];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Red];
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#endif
#endif
  }

  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Set Pixel and Brightness
  * @note   Fill the pixel By RGB Values
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  Pixel: Pixel Starts from 0 to Max - 1
  * @param  Color: RGB565 Color Code
  * @param  Brightness: Brightness level, 0 to 255
  *
  * @retval bool: true or false
  */
bool WS28XX_SetPixel_RGBW_565(WS28XX_HandleTypeDef *Handle, uint16_t Pixel, uint16_t Color, uint8_t Brightness)
{
  bool answer = true;
  uint8_t Red, Green, Blue;
  do
  {
    if (Pixel >= Handle->MaxPixel)
    {
      answer = false;
      break;
    }
    Red = ((Color >> 8) & 0xF8) * Brightness / 255;
    Green = ((Color >> 3) & 0xFC)* Brightness / 255;
    Blue = ((Color << 3) & 0xF8) * Brightness / 255;
#if (WS28XX_GAMMA == false)
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = Red;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Blue;
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = Blue;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Red;
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = Green;
    Handle->Pixel[Pixel][1] = Red;
    Handle->Pixel[Pixel][2] = Blue;
#endif
#else
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Blue];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Red];
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#endif
#endif
  }

  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Set Pixel and Brightness
  * @note   Fill the pixel By RGB Values
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  * @param  Pixel: Pixel Starts from 0 to Max - 1
  * @param  Color: RGB888 Color Code
  * @param  Brightness: Brightness level, 0 to 255
  *
  * @retval bool: true or false
  */
bool WS28XX_SetPixel_RGBW_888(WS28XX_HandleTypeDef *Handle, uint16_t Pixel, uint32_t Color, uint8_t Brightness)
{
  bool answer = true;
  uint8_t Red, Green, Blue;
  do
  {
    if (Pixel >= Handle->MaxPixel)
    {
      answer = false;
      break;
    }
    Red = ((Color & 0xFF0000) >> 16) * Brightness / 255 ;
    Green = ((Color & 0x00FF00) >> 8) * Brightness / 255;
    Blue = (Color & 0x0000FF) * Brightness / 255;
#if (WS28XX_GAMMA == false)
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = Red;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Blue;
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = Blue;
    Handle->Pixel[Pixel][1] = Green;
    Handle->Pixel[Pixel][2] = Red;
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = Green;
    Handle->Pixel[Pixel][1] = Red;
    Handle->Pixel[Pixel][2] = Blue;
#endif
#else
#if WS28XX_ORDER == WS28XX_ORDER_RGB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#elif WS28XX_ORDER == WS28XX_ORDER_BGR
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Blue];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Red];
#elif WS28XX_ORDER == WS28XX_ORDER_GRB
    Handle->Pixel[Pixel][0] = WS28XX_GammaTable[Green];
    Handle->Pixel[Pixel][1] = WS28XX_GammaTable[Red];
    Handle->Pixel[Pixel][2] = WS28XX_GammaTable[Blue];
#endif
#endif
  }

  while (0);

  return answer;
}

/***********************************************************************************************************/

/**
  * @brief  Send Buffer to LEDs
  * @note   This function use PWM+DMA for Sending data
  *
  * @param  *Handle: Pointer to WS28XX_HandleTypeDef structure
  *
  * @retval bool: true or false
  */
bool WS28XX_Update(WS28XX_HandleTypeDef *Handle)
{
  bool answer = true;
  uint32_t i = 1;
  WS28XX_Lock(Handle);
  for (uint16_t pixel = 0; pixel < Handle->MaxPixel; pixel++)
  {
    for (int rgb = 0; rgb < 3; rgb ++)
    {
      for (int b = 7; b >= 0 ; b--)
      {
        if ((Handle->Pixel[pixel][rgb] & (1 << b)) == 0)
        {
          Handle->Buffer[i] = Handle->Pulse0;
        }
        else
        {
          Handle->Buffer[i] = Handle->Pulse1;
        }
        i++;
      }
    }
  }
  if (HAL_TIM_PWM_Start_DMA(Handle->HTim, Handle->Channel, (const uint32_t*)Handle->Buffer, (Handle->MaxPixel * 24) + 2) != HAL_OK)
  {
    answer = false;
  }
  WS28XX_UnLock(Handle);
  return answer;
}

/***********************************************************************************************************/
