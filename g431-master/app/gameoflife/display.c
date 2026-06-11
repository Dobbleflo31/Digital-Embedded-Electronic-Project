/*
 * display.c
 *
 *  Created on: Feb 12, 2024
 *      Author: Nirgal
 */

/**
 * @file display.c
 * @brief Gestion de l'affichage graphique du Jeu de la Vie.
 *
 * Ce module assure l'ensemble des interactions avec l'écran TFT ILI9341 :
 * - Affichage de la grille de jeu
 * - Affichage des informations de simulation
 * - Affichage et gestion visuelle des menus
 * - Affichage des motifs prédéfinis
 * - Affichage de l'aide utilisateur
 *
 * L'objectif est de limiter les rafraîchissements en ne mettant à jour
 * que les cellules ayant changé d'état.
 *
 * @author Axel
 * @version 1.0
 * @date 04/06/2026
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g4_systick.h"
#include "stdio.h"
#include "display.h"
#include "TFT_ili9341/stm32g4_ili9341.h"
#include "TFT_ili9341/stm32g4_fonts.h"

/* Definition of constants ---------------------------------------------------*/

/**
 * @brief Couleurs utilisées dans les differents menus.
 */
#define COLOR_MENU 0xcfc0							/* Couleur de fond du menu principal. */
#define COLOR_SELECTION 0xfdff						/* Couleur utilisée pour mettre en évidence un élément sélectionné. */
#define COLOR_OUTSIDE_BORDER_PATTERN_MENU 0x4a49	/* Couleur de la bordure extérieure du menu des motifs. */
#define COLOR_INTSIDE_BORDER_PATTERN_MENU 0x9cf3	/* Couleur de la bordure intérieure du menu des motifs. */
#define COLOR_PATTERN_MENU 0xbdf7					/* Couleur de fond du menu des motifs. */
#define COLOR_INFO_BACKGROUND 0xfdf0				/* Couleur de fond de la zone d'informations. */

//_____________________________________

/* Private variables ---------------------------------------------------------*/
static cell_e displayed_cells[NB_COLUMN][NB_LINE] = {{CELL_DEAD}};	/* Dernier état affiché de la grille. */

static bool reset;	/* Force un rafraîchissement complet de l'écran. */

static bool controls_prev = false; /* Etat précedant de controls */

//_____________________________________

/* Prototypes of private functions -------------------------------------------*/
static void DISPLAY_menu_born_survive(settings_t *settings, uint8_t index, uint16_t COLOR);
static void DISPLAY_controls(void);

//_____________________________________

/* Public functions ----------------------------------------------------------*/

/**
 * @brief Initialise l'écran et l'interface graphique.
 *
 * Configure le contrôleur ILI9341, efface l'écran, affiche le message
 * de bienvenue et présente les commandes disponibles à l'utilisateur.
 *
 */
void DISPLAY_init(void) {
	ILI9341_Init();
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	reset = true;
	ILI9341_Puts(90, 10, "Jeu de la vie", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(110, 33, "Pour commencer", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 43, "veuillez appuier sur le bouton du centre", &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	DISPLAY_controls();
}

/**
 * @brief Force le rafraîchissement complet de l'affichage.
 *
 * Active le drapeau interne permettant de redessiner toutes les cellules
 * lors du prochain appel à DISPLAY_refresh().
 *
 */
void DISPLAY_reset(void){
	reset = true;
}

/**
 * @brief Met à jour l'affichage de la grille.
 *
 * Compare la grille courante avec la dernière grille affichée afin de
 * redessiner uniquement les cellules dont l'état a changé.
 *
 * @param[in] grid Grille à afficher.
 *
 */
void DISPLAY_refresh(grid_t *grid)
{
	uint8_t i,j;
	uint16_t x,y;
	for(i = 0; i < NB_COLUMN; i++){
		for(j = 0; j < NB_LINE; j++){
			x = i*CELL_SIZE;
			y = j* CELL_SIZE;
			if(reset || grid->cells[i][j] != displayed_cells[i][j]){
				displayed_cells[i][j] = grid->cells[i][j];
				if(displayed_cells[i][j] == CELL_ALIVE){
					ILI9341_DrawFilledRectangle(x, y, x + CELL_SIZE - 1, y + CELL_SIZE - 1, ILI9341_COLOR_BLACK);
				}
				else if(displayed_cells[i][j] == CELL_DEAD){
					ILI9341_DrawFilledRectangle(x, y, x + CELL_SIZE - 1, y + CELL_SIZE - 1, ILI9341_COLOR_WHITE);
				}
			}
		}
	}
	reset = false;
	//TODO
}

/**
 * @brief Affiche les informations de simulation.
 *
 * Affiche l'état du jeu (LIVE ou PAUSE), le nombre de générations
 * calculées ainsi que la période actuelle de génération.
 *
 * @param[in] info Structure contenant les informations à afficher.
 *
 */
void DISPLAY_info(info_e *info){
	char str[20];
	snprintf(str, sizeof(str), "%s%d", "generation: ",info->nb_generation);
	ILI9341_Puts(60, 10, str , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_INFO_BACKGROUND);
	snprintf(str, sizeof(str), "%s%d%s", "periode: ",info->period," ms");
	ILI9341_Puts(180, 10, str , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_INFO_BACKGROUND);
	if(info->state_info==LIVE){
		ILI9341_Puts(10, 10, "LIVE " , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_INFO_BACKGROUND);
	}
	else if(info->state_info==PAUSE){
		ILI9341_Puts(10, 10, "PAUSE" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_INFO_BACKGROUND);
	}
}

/**
 * @brief Affiche le menu principal des paramètres.
 *
 * Dessine l'ensemble des éléments graphiques du menu et met en évidence
 * l'option actuellement sélectionnée.
 *
 * Ce menu permet notamment de :
 * - Choisir le type de grille
 * - Choisir le mode de jeu
 * - Configurer les règles personnalisées
 * - Afficher l'aide
 * - Activer ou désactiver les informations
 *
 * @param[in] settings Structure contenant les paramètres du jeu.
 *
 */
void DISPLAY_menu(settings_t *settings){
	static bool enter;
	enter = settings->controls != controls_prev;
	controls_prev = settings->controls;
	if(enter && !settings->controls){
		ILI9341_DrawRectangle(5, 55, SCREEN_WIDTH-5, 236, ILI9341_COLOR_WHITE);
		ILI9341_DrawFilledRectangle(6, 56, SCREEN_WIDTH-6, 235, ILI9341_COLOR_WHITE);
	}
	if(!settings->controls){
		ILI9341_DrawRectangle(SCREEN_WIDTH - 90, SCREEN_HEIGHT - 20, SCREEN_WIDTH - 231, SCREEN_HEIGHT - 210, ILI9341_COLOR_BLACK);//Bord exterieur
		ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 91, SCREEN_HEIGHT - 21, SCREEN_WIDTH - 230, SCREEN_HEIGHT - 209, COLOR_MENU);//Rectangle interieur
		if(settings->mode==CLASIC){
			if(settings->index==1){
				ILI9341_Puts((SCREEN_WIDTH/2) - 22, 115, "CLASIC" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);

			}else{
				ILI9341_Puts((SCREEN_WIDTH/2) - 22, 115, "CLASIC" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
			}
			ILI9341_Puts((SCREEN_WIDTH/2) - 60, 142, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_GRAY);
			ILI9341_Puts((SCREEN_WIDTH/2) - 60, 169, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_GRAY);
			ILI9341_Puts((SCREEN_WIDTH/2) - 18, 142, "3" , &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_RED);
			ILI9341_Puts((SCREEN_WIDTH/2) - 32, 169, "2" , &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_RED);
			ILI9341_Puts((SCREEN_WIDTH/2) - 18, 169, "3" , &Font_7x10, ILI9341_COLOR_WHITE, ILI9341_COLOR_RED);
		}
		else if(settings->mode==ARCADE){
			if(settings->index==1){
				ILI9341_Puts((SCREEN_WIDTH/2) - 22, 115, "ARCADE" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
			}else{
				ILI9341_Puts((SCREEN_WIDTH/2) - 22, 115, "ARCADE" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
			}
			if(settings->index==2 || settings->index==3){
				uint8_t index_surv_born;
				if(settings->index==2){
					ILI9341_Puts((SCREEN_WIDTH/2) - 60, 142, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
					ILI9341_Puts((SCREEN_WIDTH/2) - 60, 169, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
					index_surv_born = settings->index_born;
				}
				else if(settings->index==3){
					ILI9341_Puts((SCREEN_WIDTH/2) - 60, 169, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
					ILI9341_Puts((SCREEN_WIDTH/2) - 60, 142, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
					index_surv_born = settings->index_survive;
				}
				for(uint8_t i=0; i<SETTINGS_TABLE_SIZE; i++){
					if(settings->index==2){
						if(settings->born[i]){
							DISPLAY_menu_born_survive(settings, i, ILI9341_COLOR_RED);
						}
					}
					else if(settings->index==3){
						if(settings->survive[i]){
							DISPLAY_menu_born_survive(settings, i, ILI9341_COLOR_RED);
						}
					}
				}
				DISPLAY_menu_born_survive(settings, index_surv_born, ILI9341_COLOR_BLACK);
			}
			else{
				ILI9341_Puts((SCREEN_WIDTH/2) - 60, 142, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
				ILI9341_Puts((SCREEN_WIDTH/2) - 60, 169, "0 1 2 3 4 5 6 7 8" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
				for(uint8_t i=0; i<SETTINGS_TABLE_SIZE; i++){
					if(settings->index==2){
						if(settings->born[i]){
							DISPLAY_menu_born_survive(settings, i, ILI9341_COLOR_RED);
						}
					}
					else if(settings->index==3){
						if(settings->survive[i]){
							DISPLAY_menu_born_survive(settings, i, ILI9341_COLOR_RED);
						}
					}
				}
			}
		}
		if(settings->grid==LINKED){
			if(settings->index==0){
				ILI9341_Puts((SCREEN_WIDTH/2) - 21, 85, "LINKED" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
			}else{
				ILI9341_Puts((SCREEN_WIDTH/2) - 21, 85, "LINKED" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
			}
		}
		else if(settings->grid==UNLINKED){
			if(settings->index==0){
				ILI9341_Puts((SCREEN_WIDTH/2) - 26, 85, "UNLIKED" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
			}else{
				ILI9341_Puts((SCREEN_WIDTH/2) - 26, 85, "UNLIKED" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
			}
		}

		ILI9341_Puts((SCREEN_WIDTH/2) - 22, 40, "Menu" , &Font_11x18, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Puts((SCREEN_WIDTH/2) - 22, 70, "Grille" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) - 34, 85, '<' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) + 24, 85, '>' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Puts((SCREEN_WIDTH/2) - 15, 100, "Mode" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) - 32, 115, '<' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) + 22, 115, '>' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Puts((SCREEN_WIDTH/2) - 15, 130, "Born" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) - 68, 142, '<' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) + 60, 142, '>' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Puts((SCREEN_WIDTH/2) - 25, 157, "Survive" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) - 68, 169, '<' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		ILI9341_Putc((SCREEN_WIDTH/2) + 60, 169, '>' , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);

		if(settings->index==4){
			ILI9341_Puts((SCREEN_WIDTH/2) - 55, 186, "Controls" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
		}else{
			ILI9341_Puts((SCREEN_WIDTH/2) - 55, 186, "Controls" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		}
		if(settings->index==5){
			ILI9341_Puts((SCREEN_WIDTH/2) + 15, 186, "Info" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
		}else{
			ILI9341_Puts((SCREEN_WIDTH/2) + 15, 186, "Info" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		}
		if(settings->index==6){
			ILI9341_Puts((SCREEN_WIDTH/2) - 22, 200, "Quiter" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
		}else{
			ILI9341_Puts((SCREEN_WIDTH/2) - 22, 200, "Quiter" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_MENU);
		}
	}
	else{
		DISPLAY_controls();
	}
}

/**
 * @brief Affiche le menu de sélection des motifs.
 *
 * Dessine l'interface permettant de parcourir les différents motifs
 * prédéfinis disponibles dans l'application.
 *
 * @param[in] up Indique quel motif est actuellement sélectionné.
 *               true : motif supérieur sélectionné.
 *               false : motif inférieur sélectionné.
 *
 */
void DISPLAY_menu_vaiseaux(bool up){
	ILI9341_DrawRectangle(SCREEN_WIDTH - 86, SCREEN_HEIGHT, SCREEN_WIDTH, 0, COLOR_OUTSIDE_BORDER_PATTERN_MENU);//Bord exterieur
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 85, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 83, 1, COLOR_INTSIDE_BORDER_PATTERN_MENU);//Bord lateral gauche
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 3, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, 1, COLOR_INTSIDE_BORDER_PATTERN_MENU);//Bord lateral doite
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 82, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 4, SCREEN_HEIGHT - 3, COLOR_INTSIDE_BORDER_PATTERN_MENU);//Bord lateral haut
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 82, 1, SCREEN_WIDTH - 4, 3, COLOR_INTSIDE_BORDER_PATTERN_MENU);//Bord lateral bas
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 4, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 40, COLOR_PATTERN_MENU);//Rectangle interieur inferieur
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 90, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 130, COLOR_PATTERN_MENU);//Rectangle interieur milieu
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 180, SCREEN_WIDTH - 14, 4, COLOR_PATTERN_MENU);//Rectangle interieur superieur
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 82, SCREEN_HEIGHT - 4, SCREEN_WIDTH - 73, 4, COLOR_PATTERN_MENU);//Rectangle interieur lateral gauche
	ILI9341_DrawFilledRectangle(SCREEN_WIDTH - 13, SCREEN_HEIGHT - 4, SCREEN_WIDTH - 4, 4, COLOR_PATTERN_MENU);//Rectangle interieur lateral droit

	if(up){
		ILI9341_DrawRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 132, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 178, COLOR_SELECTION);
		ILI9341_DrawRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 42, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 88, ILI9341_COLOR_WHITE);
	}
	else if(!up){
		ILI9341_DrawRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 42, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 88, COLOR_SELECTION);
		ILI9341_DrawRectangle(SCREEN_WIDTH - 72, SCREEN_HEIGHT - 132, SCREEN_WIDTH - 14, SCREEN_HEIGHT - 178, ILI9341_COLOR_WHITE);
	}

	ILI9341_DrawRectangle(SCREEN_WIDTH - 73, SCREEN_HEIGHT - 41, SCREEN_WIDTH - 13, SCREEN_HEIGHT - 89, ILI9341_COLOR_BLACK);
	ILI9341_DrawRectangle(SCREEN_WIDTH - 73, SCREEN_HEIGHT - 131, SCREEN_WIDTH - 13, SCREEN_HEIGHT - 179, ILI9341_COLOR_BLACK);

	ILI9341_Puts(SCREEN_WIDTH - 58, 10, "Menu" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_PATTERN_MENU);
	ILI9341_Puts(SCREEN_WIDTH - 76, 25, "vaisseaux" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_PATTERN_MENU);
}
//_____________________________________

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Affiche un indice de règle Born ou Survive.
 *
 * Met en évidence une valeur dans les tableaux de configuration
 * du mode Arcade.
 *
 * @param[in] settings Structure contenant les paramètres du jeu.
 * @param[in] index Valeur à afficher.
 * @param[in] COLOR Couleur utilisée pour la mise en évidence.
 *
 */
static void DISPLAY_menu_born_survive(settings_t *settings, uint8_t index, uint16_t COLOR){
	switch (index){
		case 0:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) - 60, 142, "0" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) - 60, 169, "0" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 1:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) - 46, 142, "1" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) - 46, 169, "1" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 2:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) - 32, 142, "2" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) - 32, 169, "2" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 3:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) - 18, 142, "3" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) - 18, 169, "3" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 4:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) - 4, 142, "4" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) - 4, 169, "4" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 5:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) + 10, 142, "5" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) + 10, 169, "5" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 6:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) + 24, 142, "6" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) + 24, 169, "6" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 7:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) + 38, 142, "7" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) + 38, 169, "7" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		case 8:
			if(settings->index==2){
				ILI9341_Puts((SCREEN_WIDTH/2) + 52, 142, "8" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			else if(settings->index==3){
				ILI9341_Puts((SCREEN_WIDTH/2) + 52, 169, "8" , &Font_7x10, ILI9341_COLOR_WHITE, COLOR);
			}
			break;
		default:
			break;
	}
}

/**
 * @brief Affiche la fenêtre d'aide utilisateur.
 *
 * Présente les différentes commandes disponibles ainsi que
 * les interactions possibles avec les boutons physiques et
 * l'écran tactile.
 *
 */
static void DISPLAY_controls(void){
	ILI9341_DrawRectangle(5, 55, SCREEN_WIDTH-5, 236, ILI9341_COLOR_BLACK);
	ILI9341_DrawFilledRectangle(6, 56, SCREEN_WIDTH-6, 235, COLOR_MENU);
	ILI9341_Puts(115, 58,"Controls",&Font_11x18,COLOR_MENU,ILI9341_COLOR_BLACK);
	ILI9341_Puts(10, 80,"-Marche/Arret/Selection: Milieu",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 100,"-Menu: Gauche",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 150,"-Menu: Haut",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 160,"-Ouverture du menu des vaisseaux: Gauche",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 170,"-Fermeture du menu des vaisseaux: Droite",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 110,"-Augmenter la periode de generation: Haut",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 120,"-Diminuer la periode de generation: Bas",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 185,"Pour se deplacer dans les menus: Haut/Bas",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(140, 95,"Marche",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(140, 145,"Arret",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 130,"-Reset de la grille: Milieu (1sec)",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 200,"Amuser vous avec le tactile pour creer vos",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts(10, 210,"propres cellules!",&Font_7x10,ILI9341_COLOR_BLACK,COLOR_MENU);
	ILI9341_Puts((SCREEN_WIDTH/2) - 22, 223, "Quiter" , &Font_7x10, ILI9341_COLOR_BLACK, COLOR_SELECTION);
}

//_____________________________________
