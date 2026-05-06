/*
 * combat.c
 *
 *  Created on: 29 avr. 2026
 *      Author: maxim
 */

#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "Pokemons/Eau/Carapuce.h"
#include "Pokemons/Eau/Carabaffe.h"
#include "Pokemons/Eau/Tortank.h"
#include "Pokemons/Feu/Salameche.h"
#include "Pokemons/Feu/Reptincel.h"
#include "Pokemons/Feu/Dracaufeu.h"
#include "Pokemons/Plante/Bulbizarre.h"
#include "Pokemons/Plante/Herbizarre.h"
#include "Pokemons/Plante/Florizarre.h"
#include <stdlib.h>
#include <time.h>

#define RAND_MAX 8;
ILI9341_Fill(ILI9341_COLOR_BLACK);

int combat_main(){
	srand(time(NULL));
	n=rand();
	switch(n){
	case 0:
		DessinerSprite(260,180,carapuce_map);
		break;
	case 1:
		DessinerSprite(260,180,carabaffe_map);
		break;
	case 2:
		DessinerSprite(260,180,tortank_map);
		break;
	case 3:
		DessinerSprite(260,180,salamèche_map);
		break;
	case 4:
		DessinerSprite(260,180,reptincel_map);
		break;
	case 5:
		DessinerSprite(260,180,dracaufeu_map);
		break;
	case 6:
		DessinerSprite(260,180,bulbizarre_map);
		break;
	case 7:
		DessinerSprite(260,180,herbizarre_map);
		break;
	case 8:
		DessinerSprite(260,180,florizarre_map);
		break;
	}
}

void combat_DessinerSprite(int pixelX, int pixelY, uint16_t pokemon_map)
{
    for(int y = 0; y < SPRITE_HEIGHT; y++)
    {
        for(int x = 0; x < SPRITE_WIDTH; x++)
        {
            uint16_t color = pokemon_map[y * SPRITE_WIDTH + x];

            if(color != TRANSPARENT)
            {
                ILI9341_DrawPixel(pixelX + x, pixelY + y, color);
            }
        }
    }
}

