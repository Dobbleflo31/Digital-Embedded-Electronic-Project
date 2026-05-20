/*

 * combat.c
 *
 *  Created on: 29 avr. 2026
 *      Author: maxim

*/
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"
#include "../Pokemons/pokemon.h"
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
		combat_DessinerSprite(260,180,pokemon_get_sprite(0));
		break;
	case 1:
		combat_DessinerSprite(260,180,pokemon_get_sprite(1));
		break;
	case 2:
		combat_DessinerSprite(260,180,pokemon_get_sprite(2));
		break;
	case 3:
		combat_DessinerSprite(260,180,pokemon_get_sprite(3));
		break;
	case 4:
		combat_DessinerSprite(260,180,pokemon_get_sprite(4));
		break;
	case 5:
		combat_DessinerSprite(260,180,pokemon_get_sprite(5));
		break;
	case 6:
		combat_DessinerSprite(260,180,pokemon_get_sprite(6));
		break;
	case 7:
		combat_DessinerSprite(260,180,pokemon_get_sprite(7));
		break;
	case 8:
		combat_DessinerSprite(260,180,pokemon_get_sprite(8));
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


