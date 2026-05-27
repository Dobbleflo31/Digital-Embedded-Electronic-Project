/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */
#include <peripheriques/display.h>
#include "stm32g4_systick.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_fonts.h"

static bool reset = false;

/**
 * @brief Initialise l'écran
 */
void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	reset = true;
}

/**
 * @brief  Met a jour l'écran avec une nouvelle matrice
 * @param  grid : matrice a afficher
 */
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
}


