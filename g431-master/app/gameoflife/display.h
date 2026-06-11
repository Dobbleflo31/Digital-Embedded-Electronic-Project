/*
 * display.h
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

/**
 * @file display.h
 * @brief Interface du module d'affichage du Jeu de la Vie.
 *
 * Ce module assure l'affichage graphique sur l'écran TFT :
 * - Affichage de la grille
 * - Affichage des informations de simulation
 * - Affichage des menus de configuration
 * - Affichage du menu des motifs
 *
 * @author Axel
 * @version 1.0
 * @date 04/06/2026
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DISPLAY_H_
#define DISPLAY_H_

/* Includes ------------------------------------------------------------------*/
#include "gameoflife.h"

/* Prototypes of public functions -------------------------------------------*/
void DISPLAY_init(void);
void DISPLAY_refresh(grid_t *grid);
void DISPLAY_menu_vaiseaux(bool up);
void DISPLAY_menu(settings_t *settings);
void DISPLAY_reset(void);
void DISPLAY_info(info_e *info);

#endif /* DISPLAY_H_ */
