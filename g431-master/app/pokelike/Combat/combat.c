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
#include "peripheriques/display.h"
#include "peripheriques/button.h"


#define HEIGHT 56
#define WIDTH 56

static void combat_DessinerSprite(int pixelX, int pixelY, uint16_t pokemon_map[]);
static void combat_affichage(void);
static void combat_menu(void);
static void combat_menu_attaque(void);

void combat_main(void){
	combat_affichage();
	combat_menu();
}

void combat_menu(){
	ILI9341_Puts(250, 160, "Attaque", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(235, 160, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(250, 180, "Pokeball", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(250, 200, "Soin", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	int count=3;
	while (1){
		if (BUTTON_down_read()){
			if (count > 1){
				// efface curseur
				ILI9341_Puts(235,240-count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
				count--;
				// affiche curseur
				ILI9341_Puts(235,240-count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				HAL_Delay(150);
				        }
				    }

		if (BUTTON_up_read()){
			if (count < 3){
				// efface curseur
				ILI9341_Puts(235,240-count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
				count++;
				// affiche curseur
				ILI9341_Puts(235,240-count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
				HAL_Delay(150);
				        }
				    }

		if (BUTTON_center_read()){
			switch (count){
				case 1:
					combat_menu_attaque();
					break;
				case 2:
					break;
				case 3:
					break;
			}
			break;
		}
	}
}

void combat_menu_attaque(){
		char * Attaque1="attaque";
		char * Attaque2="attaque";
		char * Attaque3="attaque";
		char * Attaque4="attaque";
		ILI9341_Puts(250, 160, Attaque1, &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(235, 160, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(250, 180, Attaque2, &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(250, 200, Attaque3, &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(250, 220, Attaque4, &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		int count=3;
		while (1){
			if (BUTTON_down_read()){
				if (count > 1){
					// efface curseur
					ILI9341_Puts(235,240-count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
					count--;
					// affiche curseur
					ILI9341_Puts(235,240-count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
					HAL_Delay(150);
					        }
					    }

			if (BUTTON_up_read()){
				if (count < 4){
					// efface curseur
					ILI9341_Puts(235,240-count*20, " ", &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_WHITE);
					count++;
					// affiche curseur
					ILI9341_Puts(235,240-count*20, ">", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
					HAL_Delay(150);
					        }
					    }

			if (BUTTON_center_read()){
				switch (count){
					case 1:
						//return Attaque1;
						break;
					case 2:
						//return Attaque2;
						break;
					case 3:
						//return Attaque3;
						break;
					case 4:
						//return Attaque4;
						break;
				}
				break;
			}
		}
}
void combat_affichage(){
	ILI9341_Fill(ILI9341_COLOR_BLACK);
	int x = 0;
	uintptr_t y =(uintptr_t)&x;
	srand(y);
	int n=rand()%9;
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
