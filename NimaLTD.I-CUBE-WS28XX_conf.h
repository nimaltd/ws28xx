/**
  ******************************************************************************
  * File Name          : NimaLTD.I-CUBE-WS28XX_conf.h
  * Description        : This file provides code for the configuration
  *                      of the NimaLTD.I-CUBE-WS28XX_conf.h instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NIMALTD_I_CUBE_WS28XX_CONF_H_
#define _NIMALTD_I_CUBE_WS28XX_CONF_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define WS28XX_ORDER_RGB                        0
#define WS28XX_ORDER_BGR                        1
#define WS28XX_ORDER_GRB                        2

#define WS28XX_RTOS_DISABLE                     0
#define WS28XX_RTOS_CMSIS_V1                    1
#define WS28XX_RTOS_CMSIS_V2                    2
#define WS28XX_RTOS_THREADX                     3

/**
	MiddleWare name : NimaLTD.I-CUBE-WS28XX.3.0.0
	MiddleWare fileName : NimaLTD.I-CUBE-WS28XX_conf.h
*/
/*---------- WS28XX_PIXEL_MAX  -----------*/
#define WS28XX_PIXEL_MAX      256

/*---------- WS28XX_PULSE_LENGTH_NS  -----------*/
#define WS28XX_PULSE_LENGTH_NS      1250

/*---------- WS28XX_PULSE_0_NS  -----------*/
#define WS28XX_PULSE_0_NS      400

/*---------- WS28XX_PULSE_1_NS  -----------*/
#define WS28XX_PULSE_1_NS      800

/*---------- WS28XX_ORDER  -----------*/
#define WS28XX_ORDER      WS28XX_ORDER_GRB

/*---------- WS28XX_GAMMA  -----------*/
#define WS28XX_GAMMA      true

/*---------- WS28XX_RTOS  -----------*/
#define WS28XX_RTOS      WS28XX_RTOS_DISABLE

#ifdef __cplusplus
}
#endif
#endif /* _NIMALTD_I_CUBE_WS28XX_CONF_H_ */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

