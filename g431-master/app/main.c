/**
 *******************************************************************************
 * @file 	main.c
 * @author 	jjo
 * @date 	Mar 29, 2024
 * @brief	Fichier principal de votre projet sur carte Nucléo STM32G431KB
 *******************************************************************************
 */

#include <peripheriques/button.h>
#include <peripheriques/display.h>
#include "config.h"
#include "stm32g4_sys.h"

#include "stm32g4_systick.h"
#include "stm32g4_gpio.h"
#include "stm32g4_uart.h"
#include "stm32g4_utils.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"

#include "pokelike/main.h"
#include "gameoflife/main.c"
#include "peripheriques/display.h"
#include "peripheriques/button.h"
#include <stdio.h>

#define BLINK_DELAY		100	//ms

void write_LED(bool b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool char_received(uart_id_t uart_id)
{
	if( BSP_UART_data_ready(uart_id) )	/* Si un caractère est reçu sur l'UART 2*/
	{
		/* On "utilise" le caractère pour vider le buffer de réception */
		BSP_UART_get_next_byte(uart_id);
		return true;
	}
	else
		return false;
}

//modification de test
//deuxieme modif

/**
  * @brief  Point d'entrée de votre application
  */
int main(void)
{
	/* Cette ligne doit rester la première de votre main !
	 * Elle permet d'initialiser toutes les couches basses des drivers (Hardware Abstraction Layer),
	 * condition préalable indispensable à l'exécution des lignes suivantes.
	 */
	HAL_Init();

	/* Initialisation des périphériques utilisés dans votre programme */
	BSP_GPIO_enable();
	BSP_UART_init(UART2_ID,115200);

	BUTTONS_init();
	DISPLAY_init();

	/* Indique que les printf sont dirigés vers l'UART2 */
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	// menu
	int count = 1;

	ILI9341_Fill(ILI9341_COLOR_WHITE);

	ILI9341_Puts(20, 20, "Jeu de la vie", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 40, "Pokelike", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 60, "Display On Outlook Manager", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 80, "Quitter", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	/* Tâche de fond, boucle infinie, Infinite loop,... quelque soit son nom vous n'en sortirez jamais */

	while (1)
	{
		// efface curseur
		    ILI9341_Puts(5, count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);

		    if (HAL_GPIO_ReadPin(GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN)){
		        if (count < 4){
		            count++;
		            HAL_Delay(150);
		        }
		        if (HAL_GPIO_ReadPin(GPIO_BUTTON_UP, PIN_BUTTON_UP)){
		                if (count > 1){
		                    count--;
		                    HAL_Delay(150);
		                }
		}
		    }
		// affiche curseur
		ILI9341_Puts(5, count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);

		if (HAL_GPIO_ReadPin(GPIO_BUTTON_CENTER, PIN_BUTTON_CENTER)){
			switch (count){
				case 1:
					//main_GAMEOFLIFE();
					break;
				case 2:
					main_POKELIKE();
					break;
				case 3:
					ILI9341_Puts(50, 120, "Lancement de DOOM", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
					break;
				case 4:
					ILI9341_DisplayOff();
					break;
			}

		}


	}
}

