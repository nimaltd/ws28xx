#ifndef	_WS28XX_H
#define	_WS28XX_H


#include "spi.h"
#include "gpio.h"


// init SPI @ 10.0 MHZ				SPI mode:MSB first				enable DMA:Normal mode    data width:Byte
//	output @ MOSI 
//	test with WS2811S

#define			WS28XX_MAX_PIXEL								147
#define			WS28XX_SPI											&hspi1


typedef	struct
{
	uint8_t			R	:	8	;
	uint8_t			G	:	8	;
	uint8_t			B	:	8	;
	
}ws28xx_Data_TypeDef;


typedef	enum
{
	ws28xx_Color_Black	=	0  	,
	ws28xx_Color_Red				 	,
	ws28xx_Color_Green			 	,
	ws28xx_Color_Blue					,	
	ws28xx_Color_Yellow				,	
	ws28xx_Color_Aqua					,	
	ws28xx_Color_Magenta			,	
	ws28xx_Color_DarkGreen		,	
	ws28xx_Color_DarkRed			,	
	ws28xx_Color_DarkBlue			,	
	ws28xx_Color_Olive				,	
	ws28xx_Color_Pruple				,	
	ws28xx_Color_Teal					,	
	ws28xx_Color_Silver				,	
	ws28xx_Color_Gray					,	
	ws28xx_Color_Brown				,	
	ws28xx_Color_Orange				,	
	ws28xx_Color_Gold					,	
	ws28xx_Color_Pink					,	
	ws28xx_Color_White	 			,
	
}ws28xx_Color_TypeDef;




extern ws28xx_Data_TypeDef	ws28xxLEDs[WS28XX_MAX_PIXEL];



void	ws28xx_init(void);
void	ws28xx_test(void);
void	ws28xx_Put_Pixel(ws28xx_Data_TypeDef	LED_Pixel);
void	ws28xx_Put_Pixels(ws28xx_Data_TypeDef	*LED_Pixel,uint16_t len);
void	ws28xx_Update(void);



// execute  ws28xx_Update() to see														
void	ws28xx_SetColor(uint16_t Pixel,ws28xx_Color_TypeDef Color);
void	ws28xx_SetColorArray(uint16_t PixelStart,uint16_t PixelEnd,ws28xx_Color_TypeDef Color);
void	ws28xx_SetColorArrayAndOtherOff(uint16_t PixelStart,uint16_t PixelEnd,ws28xx_Color_TypeDef Color);

#endif
