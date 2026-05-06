/*
 * main.c
 *
 *  Created on: 27 mars 2026
 *      Author: maxim
 *
 */
#include "pokelike/main.h"

#include "peripheriques/button.h"
#include "peripheriques/display.h"
#include <stdio.h>
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "carte.h"
#include "joueur.h"

void main_POKELIKE(void){
	BUTTONS_init();
	DISPLAY_init();
	Carte_Init();
	Joueur_Init();
	Carte_Afficher();

	while(1){
		Joueur_Update();

		Joueur_Effacer();
		Joueur_Afficher();

		HAL_Delay(50);
	}
}
