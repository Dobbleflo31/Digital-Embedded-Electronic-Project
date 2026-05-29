/**
 *******************************************************************************
 * @file 	main.c
 * @author 	Maxim Florian Axel
 * @date 	Mar 29, 2025
 * @brief	Fichier principal du projet
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
#include "stm32g4_adc.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "HC-05/stm32g4_hc05.h"
#include "pokelike/main.h"
#include "gameoflife/main.c"
#include "peripheriques/display.h"
#include "peripheriques/button.h"
#include <stdio.h>
#include "stdbool.h"


/**
 * @brief Point d'entrée du programme et initialisation des periphériques
 */
int main(void)
{
	BSP_ADC_init();
	HAL_Init();


	/* Initialisation des périphériques utilisés dans le programme */
	BSP_GPIO_enable();
	BSP_UART_init(UART1_ID, 9600);   // HC-05 AT MODE
	BSP_UART_init(UART2_ID, 115200);  // Docklight PC

	/* Indique que les printf sont dirigés vers l'UART2 */
	BSP_SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	BUTTONS_init();
	DISPLAY_init();

	/*while (1){
		int x = 0;
		uintptr_t y =(uintptr_t)&x;
		srand(y);
		int n=rand()%9;
		printf("%d"+n);
	}*/


	/** menu */
	int count = 1;

	ILI9341_Fill(ILI9341_COLOR_WHITE);

	ILI9341_Puts(20, 20, "Jeu de la vie", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(5, 20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 40, "Pokelike", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 60, "Display On Outlook Manager", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 80, "Quitter", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	/* Tâche de fond, boucle infinie, Infinite loop,... quelque soit son nom vous n'en sortirez jamais */

	while (1)
	{
		    if (BUTTON_down_read()){
		        if (count < 4){
		        	// efface curseur
		        	ILI9341_Puts(5, count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
		            count++;
		            // affiche curseur
		            ILI9341_Puts(5, count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		            HAL_Delay(150);
		        }
		    }

		    if (BUTTON_up_read()){
		        if (count > 1){
		            // efface curseur
		            ILI9341_Puts(5, count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
		            count--;
		            // affiche curseur
		            ILI9341_Puts(5, count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		            HAL_Delay(150);
		        }
		    }

		if (BUTTON_center_read()){
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

