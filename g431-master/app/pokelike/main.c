/**
 *  @file: main.c
 *	@date: 27 mars 2026
 *  @author: Maxim, Florian
 */

#include "pokelike/main.h"

#include "peripheriques/button.h"
#include "peripheriques/display.h"
#include <stdio.h>
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "carte.h"
#include "joueur.h"
#include "Pokemons/Eau/Carapuce.h"
#include "Pokemons/Feu/Salameche.h"
#include "Pokemons/Plante/Bulbizarre.h"


/**
 * @brief Fonction principale du jeu Pokelike
 */
void main_POKELIKE(void){
	BUTTONS_init();
	DISPLAY_init();
	Carte_Init();
	Joueur_Init();
	Carte_Afficher();

	FILE *fichier=NULL;
	fichier = fopen("player_data.csv","w+");
	int x = 0;
	uintptr_t y =(uintptr_t)&x;
	srand(y);
	int n=rand()%3;
	switch(n){
	case 0:
		fprintf(fichier,"1;1;null;null;null;1;200");
	case 1:
		fprintf(fichier,"4;1;null;null;null;1;200");
	case 2:
		fprintf(fichier,"7;1;null;null;null;1;200");
	}

	while(1){
		Joueur_Update();
		Joueur_Effacer();
		Joueur_Afficher();
		HAL_Delay(50);
	}
}
