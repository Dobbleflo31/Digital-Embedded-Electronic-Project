/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include "gameoflife.h"
#include "stm32g4xx_hal.h"
#include "stm32g4_systick.h"
#include "display.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_fonts.h"


//Constantes privées


//Prototype des fonctions privées
static bool reset = false;

//_____________________________________

//Fonctions publiques

void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	reset = true;
}


void DISPLAY_refresh(grid_t *grid)
{
	uint8_t i,j;
	cell_e displayed_cells[NB_COLUMN][NB_LINE] = {{CELL_DEAD}};
	for(i = 0; i < NB_COLUMN; i++){
		for(j = 0; j < NB_LINE; j++){
			int x = x*CELL_SIZE;
			int y = ILI9341_WIDTH - (y + 1) * CELL_SIZE;
			if(reset || grid->cells[i][j] != displayed_cells[i][j]){
				displayed_cells[i][j] = grid->cells[i][j];
				ILI9341_DrawFilledRectangle(x, y, x + CELL_SIZE - 1, y + CELL_SIZE - 1, ILI9341_COLOR_BLACK);
				}
			}
		}
	reset = false;
	//TODO
}


//_____________________________________

//Fonctions privées

