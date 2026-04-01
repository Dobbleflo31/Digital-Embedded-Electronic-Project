/*
 * sokoban.c
 *
 *  Created on: Mar 11, 2024
 *      Author: Nirgal
 */

#include <peripheriques/button.h>
#include <peripheriques/display.h>
#include "gameoflife.h"
#include "stm32g4xx_hal.h"
#include "stm32g4_systick.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"

//Definition of constants
#define NB_LEVELS 11
#define GENERATION_PERIOD_MS 100

//Private types

static bool flag_generation;

static grid_t grid0;
static grid_t grid1;

typedef enum
{
	PATTERN_BOX,
	PATTERN_BEEHIVE,
	PATTERN_TOAD,
	PATTERN_SHIP,
	PATTERN_GLIDER,
	PATTERN_QUEEN_BEE_SHUTTLE,
	PATTERN_PULSAR,
	PATTERN_BLINKER,
	PATTERN_PENTADECATHLON,
	PATTERN_GLIDER_GUN,
	PATTERN_LIGHT_WEIGHT_SPACESHIP,
	PATTERN_MIDDLE_WEIGHT_SPACESHIP,
	PATTERN_HEAVY_WEIGHT_SPACESHIP,
	PATTERN_NB
}pattern_e;

//Private variables (all variable must be private ^^)

//Prototypes of private functions
static void GAMEOFLIFE_process_ms(void);
static void GAMEOFLIFE_next_generation(grid_t * grid_prev, grid_t * grid_next);
void GAMEOFLIFE_create_pattern(grid_t * grid, uint16_t x, uint16_t y, pattern_e pattern);
static void GAMEOFLIFE_create_cell_XPT2046(grid_t * grid_next);

void XPT2046_init(void);

//Public functions
void GAMEOFLIFE_init(void)
{
	uint8_t i,j;
	DISPLAY_init();
	for(i = 0; i < NB_COLUMN; i++){
		for(j = 0; j < NB_LINE; j++){
			grid0.cells[i][j] = CELL_DEAD;
		}
	}
	GAMEOFLIFE_create_pattern(&grid0,10,10,PATTERN_BOX);
	GAMEOFLIFE_create_pattern(&grid0,20,10,PATTERN_BEEHIVE);
	GAMEOFLIFE_create_pattern(&grid0,30,10,PATTERN_TOAD);
	GAMEOFLIFE_create_pattern(&grid0,40,10,PATTERN_BLINKER);

	DISPLAY_refresh(&grid0);

	BSP_systick_add_callback_function(&GAMEOFLIFE_process_ms);
	//TODO
}


void GAMEOFLIFE_process_main(void)
{
	static bool toggle;
	grid_t *grid_prev;
	grid_t *grid_next;
	if(!toggle){
		grid_prev = &grid0;
		grid_next = &grid1;
	}else{
		grid_prev = &grid1;
		grid_next = &grid0;
	}
	toggle = !toggle;
	GAMEOFLIFE_next_generation(grid_prev,grid_next);
	GAMEOFLIFE_create_cell_XPT2046(grid_next);
	DISPLAY_refresh(grid_next);
	flag_generation = false;
	//TODO
}


//Private functions
static void GAMEOFLIFE_process_ms(void)
{
	static uint8_t t_generation=0;
	t_generation += t_generation % GENERATION_PERIOD_MS;
	if(t_generation % GENERATION_PERIOD_MS == 0){
		flag_generation = true;
	}
	//TODO
}


static void GAMEOFLIFE_next_generation(grid_t * grid_prev, grid_t * grid_next)
{
	uint8_t x,y,i,j;
	for(x=0; x<NB_COLUMN; x++){ //Parcours colonnes
		for(y=0; y<NB_LINE; y++){ //Parcours lignes
			uint8_t cell_alive_count = 0;
			if(x-1 > 0 && x+1 < NB_COLUMN){ //Evite de out of range dans la grille
				for(i=x-1; i<=x+1; i++){ //Parcours voisins colonnes
					if(y-1 > 0 && y+1 < NB_LINE){ //Evite de out of range dans la grille
						for(j=y-1; j<=y+1; j++){ //Parcours voisins lignes
							if(grid_prev->cells[i][j] == CELL_ALIVE){
								cell_alive_count ++;
							}
						}
					}
				}
			}
			if((grid_prev->cells[x][y] == CELL_ALIVE && cell_alive_count == 2) || (cell_alive_count == 3)){ //Etat suivant de la cellule
				grid_next->cells[x][y] = CELL_ALIVE;
			}
			else{
				grid_next->cells[x][y] = CELL_DEAD;
			}
		}
	}
}

static void GAMEOFLIFE_create_cell_XPT2046(grid_t * grid_next){
	int16_t x, y;
	if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)){
		y = SCREEN_HEIGHT - y;
		int16_t grid_x = x/CELL_SIZE;
		int16_t grid_y = y/CELL_SIZE;
		grid_next->cells[grid_x][grid_y] = CELL_ALIVE;
	}
}



void GAMEOFLIFE_create_pattern(grid_t * grid, uint16_t x, uint16_t y, pattern_e pattern)
{
	switch(pattern)
	{
		case PATTERN_BOX:
			grid->cells[x+0][y+0] = CELL_ALIVE;
			grid->cells[x+0][y+1] = CELL_ALIVE;
			grid->cells[x+1][y+0] = CELL_ALIVE;
			grid->cells[x+1][y+1] = CELL_ALIVE;
			break;
		case PATTERN_BEEHIVE:
			grid->cells[x+2][y+4] = CELL_ALIVE;
			grid->cells[x+3][y+3] = CELL_ALIVE;
			grid->cells[x+3][y+5] = CELL_ALIVE;
			grid->cells[x+4][y+3] = CELL_ALIVE;
			grid->cells[x+5][y+4] = CELL_ALIVE;
			grid->cells[x+4][y+5] = CELL_ALIVE;
			break;
	    case PATTERN_TOAD:
	    	grid->cells[x+1][y+2] = CELL_ALIVE;
			grid->cells[x+1][y+3] = CELL_ALIVE;
			grid->cells[x+1][y+4] = CELL_ALIVE;
			grid->cells[x+2][y+1] = CELL_ALIVE;
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+3] = CELL_ALIVE;
			break;
		case PATTERN_SHIP:
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+3] = CELL_ALIVE;
			grid->cells[x+3][y+2] = CELL_ALIVE;
			grid->cells[x+4][y+3] = CELL_ALIVE;
			grid->cells[x+4][y+4] = CELL_ALIVE;
			grid->cells[x+3][y+4] = CELL_ALIVE;
			break;
		case PATTERN_GLIDER:
			grid->cells[x+8][y + 1] = CELL_ALIVE;
			grid->cells[x+7][4] = CELL_ALIVE;
			grid->cells[x+6][4] = CELL_ALIVE;
			grid->cells[x+7][5] = CELL_ALIVE;
			grid->cells[x+8][5] = CELL_ALIVE;
			break;
	    case PATTERN_QUEEN_BEE_SHUTTLE:
	    	grid->cells[x+0][y+8] = CELL_ALIVE;
			grid->cells[x+0][y+9] = CELL_ALIVE;
			grid->cells[x+1][y+10] = CELL_ALIVE;
			grid->cells[x+2][y+11] = CELL_ALIVE;
			grid->cells[x+3][y+11] = CELL_ALIVE;
			grid->cells[x+4][y+11] = CELL_ALIVE;
			grid->cells[x+5][y+10] = CELL_ALIVE;
			grid->cells[x+6][y+9] = CELL_ALIVE;
			grid->cells[x+6][y+8] = CELL_ALIVE;
			break;
		case PATTERN_PULSAR:
			grid->cells[x+2][y+4] = CELL_ALIVE;
			grid->cells[x+3][y+3] = CELL_ALIVE;
			grid->cells[x+3][y+4] = CELL_ALIVE;
			grid->cells[x+3][y+5] = CELL_ALIVE;
			grid->cells[x+4][y+3] = CELL_ALIVE;
			grid->cells[x+4][y+5] = CELL_ALIVE;
			grid->cells[x+5][y+3] = CELL_ALIVE;
			grid->cells[x+5][y+4] = CELL_ALIVE;
			grid->cells[x+5][y+5] = CELL_ALIVE;
			grid->cells[x+6][y+4] = CELL_ALIVE;
			break;
		case PATTERN_BLINKER:
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+3] = CELL_ALIVE;
			grid->cells[x+2][y+4] = CELL_ALIVE;
		break;
		case PATTERN_PENTADECATHLON:
			grid->cells[x+5][y+0] = CELL_ALIVE;
			grid->cells[x+5][y+1] = CELL_ALIVE;
			grid->cells[x+5][y+2] = CELL_ALIVE;
			grid->cells[x+5][y+3] = CELL_ALIVE;
			grid->cells[x+5][y+4] = CELL_ALIVE;
			grid->cells[x+5][y+5] = CELL_ALIVE;
			grid->cells[x+5][y+6] = CELL_ALIVE;
			grid->cells[x+5][y+7] = CELL_ALIVE;
			grid->cells[x+5][y+8] = CELL_ALIVE;
			grid->cells[x+5][y+9] = CELL_ALIVE;
			break;
		case PATTERN_GLIDER_GUN:
			grid->cells[x+0][y+3] = CELL_ALIVE;
			grid->cells[x+0][y+4] = CELL_ALIVE;
			grid->cells[x+1][y+3] = CELL_ALIVE;
			grid->cells[x+1][y+4] = CELL_ALIVE;
			grid->cells[x+10][y+2] = CELL_ALIVE;
			grid->cells[x+10][y+3] = CELL_ALIVE;
			grid->cells[x+10][y+4] = CELL_ALIVE;
			grid->cells[x+11][y+1] = CELL_ALIVE;
			grid->cells[x+11][y+5] = CELL_ALIVE;
			grid->cells[x+12][y+0] = CELL_ALIVE;
			grid->cells[x+12][y+6] = CELL_ALIVE;
			grid->cells[x+13][y+0] = CELL_ALIVE;
			grid->cells[x+13][y+6] = CELL_ALIVE;
			grid->cells[x+14][y+3] = CELL_ALIVE;
			grid->cells[x+15][y+1] = CELL_ALIVE;
			grid->cells[x+15][y+5] = CELL_ALIVE;
			grid->cells[x+16][y+2] = CELL_ALIVE;
			grid->cells[x+16][y+3] = CELL_ALIVE;
			grid->cells[x+16][y+4] = CELL_ALIVE;
			grid->cells[x+17][y+3] = CELL_ALIVE;
			grid->cells[x+20][y+4] = CELL_ALIVE;
			grid->cells[x+20][y+5] = CELL_ALIVE;
			grid->cells[x+20][y+6] = CELL_ALIVE;
			grid->cells[x+21][y+4] = CELL_ALIVE;
			grid->cells[x+21][y+5] = CELL_ALIVE;
			grid->cells[x+21][y+6] = CELL_ALIVE;
			grid->cells[x+22][y+3] = CELL_ALIVE;
			grid->cells[x+22][y+7] = CELL_ALIVE;
			grid->cells[x+24][y+2] = CELL_ALIVE;
			grid->cells[x+24][y+3] = CELL_ALIVE;
			grid->cells[x+24][y+7] = CELL_ALIVE;
			grid->cells[x+24][y+8] = CELL_ALIVE;
			grid->cells[x+34][y+5] = CELL_ALIVE;
			grid->cells[x+34][y+6] = CELL_ALIVE;
			grid->cells[x+35][y+5] = CELL_ALIVE;
			grid->cells[x+35][y+6] = CELL_ALIVE;
			break;
		case PATTERN_LIGHT_WEIGHT_SPACESHIP:
			grid->cells[x][y+1] = CELL_ALIVE;
			grid->cells[x][y+2] = CELL_ALIVE;
			grid->cells[x+1][y+1] = CELL_ALIVE;
			grid->cells[x+1][y+2] = CELL_ALIVE;
			grid->cells[x+1][y+3] = CELL_ALIVE;
			grid->cells[x+2][y+0] = CELL_ALIVE;
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+3] = CELL_ALIVE;
			grid->cells[x+3][y+0] = CELL_ALIVE;
			grid->cells[x+3][y+1] = CELL_ALIVE;
			grid->cells[x+3][y+2] = CELL_ALIVE;
			grid->cells[x+4][y+1] = CELL_ALIVE;

			break;
		case PATTERN_MIDDLE_WEIGHT_SPACESHIP:
			grid->cells[x+0][y+1] = CELL_ALIVE;
			grid->cells[x+0][y+2] = CELL_ALIVE;
			grid->cells[x+1][y+0] = CELL_ALIVE;
			grid->cells[x+1][y+1] = CELL_ALIVE;
			grid->cells[x+1][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+0] = CELL_ALIVE;
			grid->cells[x+2][y+1] = CELL_ALIVE;
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+3][y+0] = CELL_ALIVE;
			grid->cells[x+3][y+1] = CELL_ALIVE;
			grid->cells[x+3][y+3] = CELL_ALIVE;
			grid->cells[x+4][y+1] = CELL_ALIVE;
			grid->cells[x+4][y+2] = CELL_ALIVE;
			grid->cells[x+4][y+3] = CELL_ALIVE;
			grid->cells[x+5][y+2] = CELL_ALIVE;
			break;
		case PATTERN_HEAVY_WEIGHT_SPACESHIP:
			grid->cells[x+0][y+1] = CELL_ALIVE;
			grid->cells[x+0][y+2] = CELL_ALIVE;
			grid->cells[x+1][y+0] = CELL_ALIVE;
			grid->cells[x+1][y+1] = CELL_ALIVE;
			grid->cells[x+1][y+2] = CELL_ALIVE;
			grid->cells[x+2][y+0] = CELL_ALIVE;
			grid->cells[x+2][y+1] = CELL_ALIVE;
			grid->cells[x+2][y+2] = CELL_ALIVE;
			grid->cells[x+3][y+0] = CELL_ALIVE;
			grid->cells[x+3][y+1] = CELL_ALIVE;
			grid->cells[x+3][y+2] = CELL_ALIVE;
			grid->cells[x+4][y+0] = CELL_ALIVE;
			grid->cells[x+4][y+1] = CELL_ALIVE;
			grid->cells[x+4][y+3] = CELL_ALIVE;
			grid->cells[x+5][y+1] = CELL_ALIVE;
			grid->cells[x+5][y+2] = CELL_ALIVE;
			grid->cells[x+5][y+3] = CELL_ALIVE;
			grid->cells[x+6][y+2] = CELL_ALIVE;
			break;
		default:
			break;
	}
}

