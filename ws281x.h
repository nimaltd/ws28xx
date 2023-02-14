#ifndef __WS281X_H_
#define __WS281X_H_

/*
  Author:     Nima Askari
  GitHub:     http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/@nimaltd
	Linkedin:   https://www.linkedin.com/in/nimaltd
  
  Version:    2.0.0
  
  
  Reversion History:
  
	(2.0.0)
	Rewrite again.
*/

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
	 
#include "main.h"
#include <stdbool.h>

typedef struct
{
	uint8_t								r;	
	uint8_t								g;
	uint8_t								b;
	
}ws281x_pixel_t;

typedef enum
{
	ws281x_order_rgb,
	ws281x_order_grb
	
}ws281x_order_t;

typedef	enum
{
	ws281x_color_black = 0,
	ws281x_color_red,
	ws281x_color_green,
	ws281x_color_blue,	
	ws281x_color_yellow,	
	ws281x_color_aqua,	
	ws281x_color_magenta,	
	ws281x_color_darkGreen,	
	ws281x_color_darkRed,	
	ws281x_color_darkBlue,	
	ws281x_color_olive,	
	ws281x_color_pruple,	
	ws281x_color_teal,	
	ws281x_color_silver,	
	ws281x_color_gray,	
	ws281x_color_brown,	
	ws281x_color_orange,	
	ws281x_color_gold,	
	ws281x_color_pink,	
	ws281x_color_white,
	
}ws281x_color_t;

typedef struct
{
	uint16_t							max_pixel;
	ws281x_order_t				order;
	SPI_HandleTypeDef			*spi_handle;
	uint8_t								*buffer;
	ws281x_pixel_t				*pixels;
	
}ws281x_t; 	 

//###########################################################################################################
bool	ws281x_init(ws281x_t *ws281x, SPI_HandleTypeDef *spi_handle, uint16_t max_pixel, ws281x_order_t ws281x_order);
bool 	ws281x_update(ws281x_t *ws281x);
void 	ws281x_set_pixel_rgb(ws281x_t *ws281x, uint8_t select_pixel, uint8_t red, uint8_t green, uint8_t blue);
void 	ws281x_set_pixel_color(ws281x_t *ws281x, uint8_t select_pixel, ws281x_color_t ws281x_color);
void 	ws281x_off(ws281x_t *ws281x);
void 	ws281x_test_slow(ws281x_t *ws281x);
void 	ws281x_test_fast(ws281x_t *ws281x);
//###########################################################################################################
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
