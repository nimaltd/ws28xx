
#include "ws28xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include <string.h>



//0xE0,0...			// low speed 0				3  bit=1		21 bit=0				
//0xFC,0...			// low speed 1				6 bit=1     18 bit=0


uint8_t ws28xx_temp[WS28XX_MAX_PIXEL][72];				//72 byte for send a pixel 		( 576 bit)	(24 bit led data * 24 bit spi data)

ws28xx_Data_TypeDef	ws28xxLEDs[WS28XX_MAX_PIXEL];

///////////////////////////////////////////////////////////////////////////////////////////////////////
 __forceinline void	ws28xx_fill_a_bit(uint16_t bit_number,uint8_t bit_value,uint16_t buffer_nember)
{
	uint8_t		idx_bit = 0;
	uint16_t idx_buffer = 0;
	idx_buffer =  bit_number/8;
	idx_bit =	7-(bit_number % 8);
	if(bit_value == 0)
		ws28xx_temp[buffer_nember][idx_buffer] = 	ws28xx_temp[buffer_nember][idx_buffer] & (~(0x01<<idx_bit)) ;
	else	
		ws28xx_temp[buffer_nember][idx_buffer] = 	ws28xx_temp[buffer_nember][idx_buffer] | (0x01<<idx_bit) ;
}	
///////////////////////////////////////////////////////////////////////////////////////////////////////
 __forceinline void	ws28xx_set_buffer(uint8_t RGB_bit_number_0_23,uint8_t bit_Value_0_1,uint16_t buffer_nember)
{	 
	uint16_t fill_bit=RGB_bit_number_0_23*24;
	if(bit_Value_0_1 == 0)
	{
		for(uint8_t i=0; i<3 ; i++)
			ws28xx_fill_a_bit(fill_bit+i,1,buffer_nember);
		for(uint8_t i=3; i<24 ; i++)
			ws28xx_fill_a_bit(fill_bit+i,0,buffer_nember);	
	}
	else
	{
		for(uint8_t i=0; i<7 ; i++)
			ws28xx_fill_a_bit(fill_bit+i,1,buffer_nember);
		for(uint8_t i=7; i<24 ; i++)
			ws28xx_fill_a_bit(fill_bit+i,0,buffer_nember);	
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_Put_Pixel(ws28xx_Data_TypeDef	LED_Pixel)
{
	for(int8_t i=7; i>=0 ; i--)
		ws28xx_set_buffer( i+0 ,(LED_Pixel.R & (0x80>>i)) ,0);
	for(int8_t i=7; i>=0 ; i--)
		ws28xx_set_buffer( i+8 ,(LED_Pixel.G & (0x80>>i)) ,0);
	for(int8_t i=7; i>=0 ; i--)
		ws28xx_set_buffer( i+16 ,(LED_Pixel.B & (0x80>>i)) ,0);
	
	HAL_SPI_Transmit_DMA(WS28XX_SPI,ws28xx_temp[0],72);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_Put_Pixels(ws28xx_Data_TypeDef	*LED_Pixel,uint16_t len)
{
	
	for(uint16_t x=0 ; x<len ;x++)
	{
		for(int8_t i=0; i<8 ; i++)
			ws28xx_set_buffer( i+0 ,(LED_Pixel[x].R & (0x80>>i)) ,x);
		for(int8_t i=0; i<8 ; i++)
			ws28xx_set_buffer( i+8 ,(LED_Pixel[x].G & (0x80>>i)) ,x );
		for(int8_t i=0; i<8 ; i++)
			ws28xx_set_buffer( i+16 ,(LED_Pixel[x].B & (0x80>>i)) ,x);

	}
	
	HAL_SPI_Transmit_DMA(WS28XX_SPI,&ws28xx_temp[0][0],(72)*len);
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_Update(void)
{
	
	ws28xx_Put_Pixels(ws28xxLEDs,WS28XX_MAX_PIXEL);	
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


void	ws28xx_init(void)
{
	//	init hardware	, I init in cubeMX
	
	//	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_test(void)
{
	memset(ws28xxLEDs,50,sizeof(ws28xxLEDs));
	ws28xx_Put_Pixels(ws28xxLEDs,WS28XX_MAX_PIXEL);
	osDelay(200);
	memset(ws28xxLEDs,0,sizeof(ws28xxLEDs));
	ws28xx_Put_Pixels(ws28xxLEDs,WS28XX_MAX_PIXEL);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_SetColor(uint16_t Pixel,ws28xx_Color_TypeDef Color)
{
	if(Pixel >= WS28XX_MAX_PIXEL )
		return;	
	switch(Color)
	{
		//---------------------
		case ws28xx_Color_Black:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_White:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 255;
			ws28xxLEDs[Pixel].B = 255;
		break;
		//---------------------
		case ws28xx_Color_Red:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Green:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 255;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Blue:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 255;
		break;
		//---------------------
		case ws28xx_Color_Aqua:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 255;
			ws28xxLEDs[Pixel].B = 255;
		break;
		//---------------------
		case ws28xx_Color_Magenta:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 255;
		break;
		//---------------------
		case ws28xx_Color_Yellow:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 255;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_DarkBlue:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 128;
		break;
		//---------------------
		case ws28xx_Color_DarkGreen:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 128;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_DarkRed:
			ws28xxLEDs[Pixel].R = 128;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Gray:
			ws28xxLEDs[Pixel].R = 128;
			ws28xxLEDs[Pixel].G = 128;
			ws28xxLEDs[Pixel].B = 128;
		break;
		//---------------------
		case ws28xx_Color_Olive:
			ws28xxLEDs[Pixel].R = 128;
			ws28xxLEDs[Pixel].G = 128;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Pruple:
			ws28xxLEDs[Pixel].R = 128;
			ws28xxLEDs[Pixel].G = 0;
			ws28xxLEDs[Pixel].B = 128;
		break;
		//---------------------
		case ws28xx_Color_Silver:
			ws28xxLEDs[Pixel].R = 192;
			ws28xxLEDs[Pixel].G = 192;
			ws28xxLEDs[Pixel].B = 192;
		break;
		//---------------------
		case ws28xx_Color_Teal:
			ws28xxLEDs[Pixel].R = 0;
			ws28xxLEDs[Pixel].G = 128;
			ws28xxLEDs[Pixel].B = 128;
		break;
		//---------------------
		case ws28xx_Color_Brown:
			ws28xxLEDs[Pixel].R = 165;
			ws28xxLEDs[Pixel].G = 42;
			ws28xxLEDs[Pixel].B = 42;
		break;
		//---------------------
		case ws28xx_Color_Orange:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 165;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Gold:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 215;
			ws28xxLEDs[Pixel].B = 0;
		break;
		//---------------------
		case ws28xx_Color_Pink:
			ws28xxLEDs[Pixel].R = 255;
			ws28xxLEDs[Pixel].G = 20;
			ws28xxLEDs[Pixel].B = 147;
		break;
	

		
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_SetColorArray(uint16_t PixelStart,uint16_t PixelEnd,ws28xx_Color_TypeDef Color)
{
	for(uint16_t i=PixelStart ; i<=PixelEnd ; i++)
		ws28xx_SetColor(i,Color);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	ws28xx_SetColorArrayAndOtherOff(uint16_t PixelStart,uint16_t PixelEnd,ws28xx_Color_TypeDef Color)
{
	
	for(uint16_t i=0 ; i<PixelStart ; i++)
		ws28xx_SetColor(i,ws28xx_Color_Black);
		
	for(uint16_t i=PixelStart ; i<=PixelEnd ; i++)
		ws28xx_SetColor(i,Color);	

	
	for(uint16_t i=PixelEnd+1 ; i<WS28XX_MAX_PIXEL ; i++)
		ws28xx_SetColor(i,ws28xx_Color_Black);
	

}
