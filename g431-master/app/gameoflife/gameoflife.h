/*
 * breakbricks.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

#ifndef GAMEOFLIFE_H_
#define GAMEOFLIFE_H_

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240
#define CELL_SIZE 		4


#define NB_COLUMN		SCREEN_WIDTH/CELL_SIZE
#define NB_LINE			SCREEN_HEIGHT/CELL_SIZE
#define NB_CELLS 		(NB_COLUMN*NB_LINE)

typedef enum
{
	CELL_DEAD = 0,
	CELL_ALIVE =1
}cell_e;

typedef struct{
	cell_e cells [NB_COLUMN][NB_LINE];
}grid_t;


/*		^		column
 * 		| y		  v
 * 		_____________________________________
 * 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|		<---- line
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		|__|__|__|__|__|__|__|__|__|__|__|__|
* 		o__|__|__|__|__|__|__|__|__|__|__|__|--->   x
*
*/


void GAMEOFLIFE_init(void);

void GAMEOFLIFE_process_main(void);


#endif /* GAMEOFLIFE_H_ */
