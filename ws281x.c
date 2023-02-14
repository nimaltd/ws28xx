
#include "ws281x.h"
#include "ws281x_conf.h"
#include <stdlib.h>
#include <string.h>
#if WS281X_FREERTOS == 1
#include "cmsis_os.h"
#define ws281x_delay(x)	osDelay(x)
#else
#define ws281x_delay(x)	HAL_Delay(x)
#endif
//###########################################################################################################
bool ws281x_init(ws281x_t *ws281x, SPI_HandleTypeDef *spi_handle, uint16_t max_pixel, ws281x_order_t ws281x_order)
{
	ws281x->spi_handle = spi_handle;
#if WS281X_FREERTOS == 0
	ws281x->pixels = malloc(max_pixel * 3);
	ws281x->buffer = malloc(max_pixel * 32);	
#else
	ws281x->pixels = pvPortMalloc(max_pixel * 3);	
	ws281x->buffer = pvPortMalloc(max_pixel * 32);	
#endif		
	if (ws281x->pixels != NULL && ws281x->buffer != NULL)
	{
		ws281x->max_pixel = max_pixel;
		ws281x->order = ws281x_order;
		memset(ws281x->pixels, 0, max_pixel * 3); 
		memset(ws281x->buffer, 0, max_pixel * 32); 
		ws281x_update(ws281x);
		return true;
	}
	else
	{
#if WS281X_FREERTOS == 0
		if (ws281x->pixels != NULL)
			free(ws281x->pixels);
		if (ws281x->buffer != NULL)
			free(ws281x->buffer);
#else
		if (ws281x->pixels != NULL)
			vPortFree(ws281x->pixels);
		if (ws281x->buffer != NULL)
			vPortFree(ws281x->buffer);
#endif		
			
		return false;
	}
}
//###########################################################################################################
bool ws281x_update(ws281x_t *ws281x)
{
	int buff_cnt = 0;
	/* reset buffer	*/
	memset(ws281x->buffer, 0, ws281x->max_pixel * 32); 
	switch (ws281x->order)
	{
		/* order RGB BEGIN */
		case ws281x_order_rgb:
			for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
			{
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].r & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].r & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].g & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].g & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].b & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].b & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
			}
		break;
		/* order RGB END */
			
		/* order GRB BEGIN */
		case ws281x_order_grb:
			for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
			{
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].g & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].g & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].r & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].r & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
				for (int i = 7; i >= 0; i-=2)
				{		
					if (ws281x->pixels[pixel].b & (1 << i))
						ws281x->buffer[buff_cnt] |= 0xC0;
					else
						ws281x->buffer[buff_cnt] |= 0x80;
					if (ws281x->pixels[pixel].b & (1 << (i - 1)))
						ws281x->buffer[buff_cnt] |= 0x0C;
					else
						ws281x->buffer[buff_cnt] |= 0x08;
					buff_cnt++;
				}		
			}
		break;			
		/* order GRB END */
	}	
	/* send via SPI */
	if (HAL_SPI_Transmit_DMA(ws281x->spi_handle, ws281x->buffer, ws281x->max_pixel * 32) == HAL_OK)
		return true;
	else
		return false;
}
//###########################################################################################################
void ws281x_set_pixel_rgb(ws281x_t *ws281x, uint8_t select_pixel, uint8_t red, uint8_t green, uint8_t blue)
{
	if(select_pixel >= ws281x->max_pixel)
		return;	
	ws281x->pixels[select_pixel].r = red;	
	ws281x->pixels[select_pixel].g = green;
	ws281x->pixels[select_pixel].b = blue;
}
//###########################################################################################################
void ws281x_set_pixel_color(ws281x_t *ws281x, uint8_t select_pixel, ws281x_color_t ws281x_color)
{
	if(select_pixel >= ws281x->max_pixel)
		return;	
	switch(ws281x_color)
	{
		case ws281x_color_black:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_white:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 255;
			ws281x->pixels[select_pixel].b = 255;
		break;
		case ws281x_color_red:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_green:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 255;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_blue:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 255;
		break;
		case ws281x_color_aqua:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 255;
			ws281x->pixels[select_pixel].b = 255;
		break;
		case ws281x_color_magenta:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 255;
		break;
		case ws281x_color_yellow:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 255;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_darkBlue:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 255;
		break;
		case ws281x_color_darkGreen:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 128;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_darkRed:
			ws281x->pixels[select_pixel].r = 128;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_gray:
			ws281x->pixels[select_pixel].r = 128;
			ws281x->pixels[select_pixel].g = 128;
			ws281x->pixels[select_pixel].b = 128;
		break;
		case ws281x_color_olive:
			ws281x->pixels[select_pixel].r = 128;
			ws281x->pixels[select_pixel].g = 128;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_pruple:
			ws281x->pixels[select_pixel].r = 128;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 128;
		break;
		case ws281x_color_silver:
			ws281x->pixels[select_pixel].r = 192;
			ws281x->pixels[select_pixel].g = 192;
			ws281x->pixels[select_pixel].b = 192;
		break;
		case ws281x_color_teal:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 128;
			ws281x->pixels[select_pixel].b = 128;
		break;
		case ws281x_color_brown:
			ws281x->pixels[select_pixel].r = 165;
			ws281x->pixels[select_pixel].g = 42;
			ws281x->pixels[select_pixel].b = 42;
		break;
		case ws281x_color_orange:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 165;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_gold:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 215;
			ws281x->pixels[select_pixel].b = 0;
		break;
		case ws281x_color_pink:
			ws281x->pixels[select_pixel].r = 255;
			ws281x->pixels[select_pixel].g = 20;
			ws281x->pixels[select_pixel].b = 147;
		break;
		default:
			ws281x->pixels[select_pixel].r = 0;
			ws281x->pixels[select_pixel].g = 0;
			ws281x->pixels[select_pixel].b = 0;
		break;
	}
}
//#########################################################################################################
void ws281x_off(ws281x_t *ws281x)
{
	ws281x_delay(20);
	memset(ws281x->pixels, 0, ws281x->max_pixel * 3);
	ws281x_update(ws281x);
}
//#########################################################################################################
void ws281x_test_slow(ws281x_t *ws281x)
{
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
	{
		ws281x_delay(50);		
		if (pixel > 0)
			ws281x_set_pixel_rgb(ws281x, pixel - 1, 0, 0, 0);
		ws281x_set_pixel_rgb(ws281x, pixel, 255, 0, 0);
		ws281x_update(ws281x);		
	}
	ws281x_set_pixel_rgb(ws281x, ws281x->max_pixel - 1, 0, 0, 0);
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
	{
		ws281x_delay(50);
		if (pixel > 0)
			ws281x_set_pixel_rgb(ws281x, pixel - 1, 0, 0, 0);
		ws281x_set_pixel_rgb(ws281x, pixel, 0, 255, 0);
		ws281x_update(ws281x);		
	}
	ws281x_set_pixel_rgb(ws281x, ws281x->max_pixel - 1, 0, 0, 0);
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
	{
		ws281x_delay(50);
		if (pixel > 0)
			ws281x_set_pixel_rgb(ws281x, pixel - 1, 0, 0, 0);
		ws281x_set_pixel_rgb(ws281x, pixel, 0, 0, 255);
		ws281x_update(ws281x);		
	}
	ws281x_off(ws281x);
}
//#########################################################################################################
void ws281x_test_fast(ws281x_t *ws281x)
{
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
		ws281x_set_pixel_rgb(ws281x, pixel, 255, 0, 0);
	ws281x_update(ws281x);
	ws281x_delay(250);
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
		ws281x_set_pixel_rgb(ws281x, pixel, 0, 255, 0);
	ws281x_update(ws281x);
	ws281x_delay(250);
	for (int pixel = 0; pixel < ws281x->max_pixel; pixel++)
		ws281x_set_pixel_rgb(ws281x, pixel, 0, 0, 255);
	ws281x_update(ws281x);
	ws281x_delay(250);
	ws281x_off(ws281x);
}
