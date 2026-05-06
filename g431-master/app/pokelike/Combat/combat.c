/*
 * combat.c
 *
 *  Created on: 29 avr. 2026
 *      Author: maxim
 */

#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "../Pokemons/Eau/Carapuce.h"
#include "../Pokemons/Eau/Carabaffe.h"
#include "../Pokemons/Eau/Tortank.h"
#include "../Pokemons/Feu/Salameche.h"
#include "../Pokemons/Feu/Reptincel.h"
#include "../Pokemons/Feu/Dracaufeu.h"
#include "../Pokemons/Plante/Bulbizarre.h"
#include "../Pokemons/Plante/Herbizarre.h"
#include "../Pokemons/Plante/Florizarre.h"
#include <stdlib.h>
#include <time.h>

#define RAND_MAX 8
#define HEIGHT 56
#define WIDTH 56

static void combat_DessinerSprite(int pixelX, int pixelY, uint16_t pokemon_map[]);

void combat_main(void){
	ILI9341_Fill(ILI9341_COLOR_BLACK);
	srand(time(NULL));
	int n=rand();
	switch(n){
	case 0:
		combat_DessinerSprite(260,180,carapuce_map);
		break;
	case 1:
		combat_DessinerSprite(260,180,carabaffe_map);
		break;
	case 2:
		combat_DessinerSprite(260,180,tortank_map);
		break;
	case 3:
		combat_DessinerSprite(260,180,salameche_map);
		break;
	case 4:
		combat_DessinerSprite(260,180,reptincel_map);
		break;
	case 5:
		combat_DessinerSprite(260,180,dracaufeu_map);
		break;
	case 6:
		combat_DessinerSprite(260,180,bulbizarre_map);
		break;
	case 7:
		combat_DessinerSprite(260,180,herbizarre_map);
		break;
	case 8:
		combat_DessinerSprite(260,180,florizarre_map);
		break;
	}


}

void combat_DessinerSprite(int pixelX, int pixelY, uint16_t pokemon_map[])
{
    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
        	ILI9341_DrawPixel(pixelX,pixelY,pokemon_map[y*WIDTH+x]);
        }
    }
}

