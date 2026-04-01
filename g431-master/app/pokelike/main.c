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

void main_POKELIKE(void){
	BUTTONS_init();
	DISPLAY_init();
	Carte_Init();
	Carte_Afficher();

	while(1){

	}
}
