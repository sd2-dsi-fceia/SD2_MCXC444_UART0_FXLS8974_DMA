/* Copyright 2022, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2017-2019, Gustavo Muro - Daniel Márquez
 * Copyright 2025, Guido Cicconi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/

// Standard C Included Files
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Project Included Files
#include "SD2_board.h"
#include "SD2_I2C.h"
#include "clock_config.h"
#include "uart0_drv.h"
#include "fxls8974.h"

#define _UMBRAL_EJE_X_POSITIVO 50
#define _UMBRAL_EJE_X_NEGATIVO -50
#define _RETARDO_MSEG 100

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

static uint32_t cuenta;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void)
{
    uint8_t buffer[32];
    int16_t acc; // Variable de lectura de un eje del acelerometro

	// Se inicializan funciones de la placa
	BOARD_BootClockRUN();
	board_init();
	SD2_I2C_init();

	// Se inicializa el acelerómetro
	fxls8974_init();
	fxls8974_setDataRate(DR_12p5hz);

	//Inicializa driver de UART0
	uart0_drv_init();

	//inicializa interrupción de systick cada 1 ms
	SysTick_Config(SystemCoreClock / 1000U);

	//Lazo infinito
	while(1)
	{
		// Led testigo de transmisión por UART0
		board_setLed(BOARD_LED_ID_VERDE, BOARD_LED_MSG_OFF);

		//Desde aquí se genera un retardo de X mSeg con el Systick Timer
		cuenta = _RETARDO_MSEG;
		while(cuenta!=0);

		//Lee valor de aceleración del eje X
	    acc = fxls8974_getAcX();

	    //Enciende led rojo si la lectura supera el umbral positivo
	    if (acc > _UMBRAL_EJE_X_POSITIVO)
	    {
	    	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_ON);
	    }
	    else
	    {
	    	board_setLed(BOARD_LED_ID_ROJO, BOARD_LED_MSG_OFF);
	    }

	    //Enciende led azul si la lectura supera el umbral negativo
	    if (acc < _UMBRAL_EJE_X_NEGATIVO)
	    {
	    	board_setLed(BOARD_LED_ID_AZUL, BOARD_LED_MSG_ON);
	    }
	    else
	    {
	    	board_setLed(BOARD_LED_ID_AZUL, BOARD_LED_MSG_OFF);
	    }

	    //Se da formato al string a transmitir y se almacena en el arrelgo de char buffer
	    sprintf((char*)buffer, "EJE X:%d \r\n", acc);

	    //Envia datos por UART0 mediante DMA
	    uart0_drv_envDatos(buffer, strlen((char*)buffer));

	    //Desde aquí se genera un retardo de X mSeg con el Systick Timer
	    cuenta = _RETARDO_MSEG;
	    while(cuenta!=0);
	}
}


void SysTick_Handler(void)
{
   if(cuenta!=0)
	   cuenta--;
}

/*==================[end of file]============================================*/
