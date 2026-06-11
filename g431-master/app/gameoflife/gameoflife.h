/*
 * breakbricks.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

/**
 * @file gameoflife.h
 * @brief Déclarations des types et constantes du Jeu de la Vie.
 *
 * Ce fichier contient :
 * - Les dimensions de l'écran et de la grille
 * - Les paramètres de conversion tactile
 * - Les structures de données principales
 * - Les énumérations utilisées par l'application
 *
 * @author Axel
 * @version 1.0
 * @date 04/06/2026
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GAMEOFLIFE_H_
#define GAMEOFLIFE_H_

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "stdint.h"

/**
 * @brief Dimensions de l'écran TFT en pixels.
 */
#define SCREEN_WIDTH	320	/* Largeur de l'écran TFT en pixels. */
#define SCREEN_HEIGHT	240 /* Hauteur de l'écran TFT en pixels. */

/**
 * @brief Valeurs de calibrage de l'écran TFT.
 */
#define REAL_SIZE_SCEEN_TOUCH_x 370	/* Largeur utile de la surface tactile. */
#define REAL_SIZE_SCEEN_TOUCH_y 280 /* Hauteur utile de la surface tactile. */

#define SCREEN_TOUCH_RATIO_x REAL_SIZE_SCEEN_TOUCH_x/SCREEN_WIDTH 	/* Rapport de conversion tactile horizontal. */
#define SCREEN_TOUCH_RATIO_y REAL_SIZE_SCEEN_TOUCH_y/SCREEN_HEIGHT	/* Rapport de conversion tactile vertical. */

#define SCREEN_SHIFT_x 16 /*  Décalage horizontal appliqué aux coordonnées tactiles. */
#define SCREEN_SHIFT_y 20 /* Décalage vertical appliqué aux coordonnées tactiles. */

/* Public define ------------------------------------------------------------*/
/**
 * @brief Taille d'une cellule en pixels.
 */
#define CELL_SIZE 4

/**
 * @brief Taille des tableaux de règles Born et Survive.
 */
#define SETTINGS_TABLE_SIZE 9

/**
 * @brief Dimensions de la grille.
 */
#define NB_COLUMN		SCREEN_WIDTH/CELL_SIZE	/* Nombre de colonnes de la grille. */
#define NB_LINE			SCREEN_HEIGHT/CELL_SIZE	/* Nombre de lignes de la grille. */
#define NB_CELLS 		(NB_COLUMN*NB_LINE)		/* Nombre total de cellules de la grille. */

/* Public types -------------------------------------------------------------*/

/**
 * @enum grid_type_e
 * @brief Type de comportement des bords de la grille.
 */
typedef enum
{
	LINKED,		/* Grille torique. */
	UNLINKED	/* Grille classique. */
}grid_type_e;

/**
 * @enum mode_e
 * @brief Modes de fonctionnement du Jeu de la Vie.
 */
typedef enum{
	CLASIC,		/* Règles classiques de Conway. */
	ARCADE,		/* Règles personnalisables par l'utilisateur. */
}mode_e;

/**
 * @struct settings_t
 * @brief Paramètres configurables du Jeu de la Vie.
 *
 * Cette structure regroupe l'ensemble des options accessibles
 * depuis le menu utilisateur.
 */
typedef struct
{
	grid_type_e grid;
	mode_e mode;
	bool exit;
	bool infos;
	bool controls;
	bool survive[SETTINGS_TABLE_SIZE];
	bool born[SETTINGS_TABLE_SIZE];
	uint8_t index_survive;
	uint8_t index_born;
	uint8_t index;
}settings_t;

/**
 * @enum state_info_e
 * @brief État de la simulation affiché à l'écran.
 */
typedef enum
{
	PAUSE,
	LIVE
}state_info_e;

/**
 * @struct info_e
 * @brief Informations affichées à l'utilisateur.
 */
typedef struct
{
	state_info_e state_info;
	int nb_generation;
	uint16_t period;
}info_e;

/**
 * @enum cell_e
 * @brief États possibles d'une cellule.
 */
typedef enum
{
	CELL_DEAD = 0,
	CELL_ALIVE =1
}cell_e;

/**
 * @struct grid_t
 * @brief Représente une génération du Jeu de la Vie.
 *
 * Chaque élément du tableau correspond à l'état d'une cellule.
 */
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

/* Prototypes of public functions -------------------------------------------*/
void GAMEOFLIFE_init(void);

void GAMEOFLIFE_process_main(void);

void GAMEOFLIFE_state_machine(void);


#endif /* GAMEOFLIFE_H_ */
