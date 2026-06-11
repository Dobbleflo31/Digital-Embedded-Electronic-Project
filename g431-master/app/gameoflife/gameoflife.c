/*
 * sokoban.c
 *
 *  Created on: Mar 11, 2024
 *      Author: Nirgal
 */

/**
 * @file gameoflife.c
 * @brief Implémentation du Jeu de la Vie de Conway sur STM32.
 *
 * Ce module gère l'exécution du Jeu de la Vie ainsi que les
 * fonctionnalités associées :
 * - Calcul des générations
 * - Gestion des menus
 * - Gestion des motifs prédéfinis
 * - Gestion de l'écran tactile
 * - Contrôle de la vitesse d'exécution
 * - Affichage des informations de simulation
 *
 * Deux modes de grille sont disponibles :
 * - Grille classique (bords non connectés)
 * - Grille torique (bords connectés)
 *
 * Deux modes de règles sont également proposés :
 * - Mode classique Conway
 * - Mode arcade configurable
 *
 * @author Axel
 * @version 1.0
 * @date 04/06/2026
 */

/* Includes ------------------------------------------------------------------*/
#include "gameoflife.h"
#include "stm32g4xx_hal.h"
#include "button.h"
#include "display.h"
#include "stm32g4_timer.h"
#include "stm32g4_systick.h"
#include "TFT_ili9341/stm32g4_xpt2046.h"

/* Definition of constants ---------------------------------------------------*/

/**
 * @def TIME_LONG_PRESS
 * @brief Durée minimale d'appui pour déclencher un appui long.
 *
 * Valeur exprimée en millisecondes.
 */
#define TIME_LONG_PRESS 1000

/* Private types -------------------------------------------------------------*/

/**
 * @enum period_e
 * @brief Liste des périodes possibles entre deux générations.
 *
 * Les valeurs correspondent au temps d'attente en millisecondes.
 */
typedef enum
{
	GENERATION_PERIOD_10_MS = 10,
	GENERATION_PERIOD_20_MS = 20,
	GENERATION_PERIOD_50_MS = 50,
	GENERATION_PERIOD_100_MS = 100,
	GENERATION_PERIOD_200_MS = 200,
	GENERATION_PERIOD_300_MS = 300,
	GENERATION_PERIOD_400_MS = 400,
	GENERATION_PERIOD_500_MS = 500,
	GENERATION_PERIOD_1000_MS = 1000
}period_e;

/**
 * @enum states_e
 * @brief États de la machine à états principale.
 *
 * Cette énumération décrit les différents modes de fonctionnement
 * de l'application.
 */
typedef enum
{
	RESET_GRILLE=0,
	MAIN=1,
	ARRET=2,
	STOP=3,
	GO=4,
	MENU=5,
	MENU_VAISSEAUX=6,
	PLACEMENT_VAISSEAU=7
}states_e;

/**
 * @enum pattern_e
 * @brief Motifs prédéfinis disponibles dans le jeu.
 *
 * Chaque valeur représente une structure classique du Jeu de la Vie.
 */
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

/* Private variables ---------------------------------------------------------*/
static grid_t grid_dead;				/* Grille vide utilisée comme modèle de réinitialisation. */
static grid_t grid0;					/* Première grille de calcul. */
static grid_t grid1;					/* Seconde grille de calcul. */
static grid_t grid2;					/* Grille utilisée pour les aperçus de motifs. */
static volatile bool flag_generation;	/* Indique qu'une nouvelle génération doit être calculée. */
static bool toggle;						/* Sélectionne la grille active du double buffering. */
static volatile uint8_t t_generation;	/* Compteur temporel utilisé pour générer les générations. */
static volatile int reset_init;			/* Compteur temporel utilisé pour détecter un appui long. */
static bool button_enable;				/* Verrou logiciel évitant les répétitions de commandes. */
static volatile bool flag_reset;		/* Indique qu'une réinitialisation a été demandée. */
static period_e period;					/* Période actuelle entre deux générations. */
static states_e state;					/* État courant de la machine à états. */
static pattern_e pattern_menu_index;	/* Motif actuellement sélectionné dans le menu. */
static int16_t x_pattern;				/* Coordonnée horizontale du motif en cours de placement. */
static int16_t y_pattern;				/* Coordonnée verticale du motif en cours de placement. */
static bool pattern_init;				/* Indique si le menu des motifs doit être initialisé. */
static bool pattern_placed;				/* Indique si le motif sélectionné a déjà été placé. */
static settings_t settings;				/* Paramètres utilisateur du Jeu de la Vie. */
static info_e info;						/* Informations affichées à l'écran. */

/* Prototypes of private functions -------------------------------------------*/
static void GAMEOFLIFE_process_ms(void);
static void GAMEOFLIFE_next_generation(grid_t * grid_prev, grid_t * grid_next);
static void GAMEOFLIFE_next_generation_v_2(grid_t * grid_prev, grid_t * grid_next);
static void GAMEOFLIFE_BUTTON(void);
static void GAMEOFLIFE_create_pattern(grid_t * grid, int16_t x, int16_t y, pattern_e pattern);
static void GAMEOFLIFE_create_cell_XPT2046(grid_t * grid_next);
static void GAMEOFLIFE_change_period(void);
static void GAMEOFLIFE_reset(void);
static void GAMEOFLIFE_modulo(int16_t * x, int16_t * y);

/* Public functions ----------------------------------------------------------*/

/**
 * @brief Initialise le module Game of Life.
 *
 * Configure les paramètres par défaut du jeu, initialise les grilles,
 * crée quelques motifs de démonstration, initialise l'affichage et
 * enregistre les callbacks périodiques utilisés par l'application.
 *
 * @note Cette fonction doit être appelée une seule fois au démarrage.
 */
void GAMEOFLIFE_init(void)
{
	uint8_t i,j;

	DISPLAY_init();
	XPT2046_init();

	state=ARRET;

	settings.controls=false;
	settings.exit=false;
	settings.infos=true;
	settings.grid=LINKED;
	settings.mode=CLASIC;
	settings.index_survive=0;
	settings.index_born=0;
	settings.index=6;
	for (int i = 0; i < 8; i++) {
		settings.survive[i]=false;
		settings.born[i]=false;
	}

	toggle=false;
	t_generation=0;
	reset_init=0;
	button_enable=false;
	pattern_menu_index = PATTERN_BOX;
	flag_reset = false;
	period=GENERATION_PERIOD_100_MS;
	info.period=GENERATION_PERIOD_100_MS;
	info.nb_generation=0;
	pattern_placed=true;
	for(i = 0; i < NB_COLUMN; i++){
		for(j = 0; j < NB_LINE; j++){
			grid_dead.cells[i][j] = CELL_DEAD;
		}
	}
	grid0 = grid_dead;
	grid2 = grid0;

	while(BUTTON_center_read() == 0){
	}

	GAMEOFLIFE_create_pattern(&grid0,10,10,PATTERN_BOX);
	GAMEOFLIFE_create_pattern(&grid0,20,20,PATTERN_GLIDER_GUN);
	GAMEOFLIFE_create_pattern(&grid0,30,10,PATTERN_TOAD);
	GAMEOFLIFE_create_pattern(&grid0,40,10,PATTERN_BLINKER);

	DISPLAY_reset();
	DISPLAY_refresh(&grid0);

	BSP_systick_add_callback_function(&GAMEOFLIFE_process_ms);
	BSP_systick_add_callback_function(&GAMEOFLIFE_BUTTON);
	//TODO
}

/**
 * @brief Gère la machine à états principale du jeu.
 *
 * Analyse les entrées utilisateur et effectue les transitions entre
 * les différents états de fonctionnement :
 * - Exécution
 * - Pause
 * - Menus
 * - Placement de motifs
 * - Réinitialisation
 *
 * Cette fonction doit être appelée périodiquement dans la boucle principale.
 */
void GAMEOFLIFE_state_machine(void){
	switch(state){
		case MAIN:
			if(BUTTON_center_read() == 1 && !button_enable){
				state = STOP;
			}
			else if(BUTTON_left_read() == 1 && !button_enable){
				button_enable = true;
				DISPLAY_menu(&settings);
				state = MENU;
			}
			break;
		case ARRET:
			if(BUTTON_center_read() == 1 && !button_enable){
				state = GO;
			}
			else if(BUTTON_up_read() == 1 && !button_enable){
				button_enable = true;
				DISPLAY_menu(&settings);
				state = MENU;
			}
			else if(BUTTON_left_read() == 1 && !button_enable){
				button_enable = true;
				pattern_init=true;
				state = MENU_VAISSEAUX;
			}
			break;
		case STOP:
			if(BUTTON_center_read() == 0){
				if(flag_reset){
					state = RESET_GRILLE;
				}
				else if(!flag_reset){
					info.state_info=PAUSE;
					state = ARRET;
				}
			}
			break;
		case GO:
			if(BUTTON_center_read() == 0){
				if(flag_reset){
					state = RESET_GRILLE;
				}
				else if(!flag_reset){
					info.state_info=LIVE;
					state = MAIN;
				}
			}
			break;
		case MENU:
			if(settings.exit){
				settings.exit=false;
				info.state_info=PAUSE;
				DISPLAY_reset();
				state = ARRET;
			}
			break;
		case MENU_VAISSEAUX:
			if(BUTTON_right_read() == 1 && !button_enable){
				button_enable=true;
				pattern_menu_index=0;
				info.state_info=PAUSE;
				DISPLAY_reset();
				state = ARRET;
			}
			else if(BUTTON_center_read() == 1 && !button_enable){
				button_enable=true;
				DISPLAY_reset();
				y_pattern = NB_LINE/2;
				x_pattern = NB_COLUMN/2;
				grid2=grid_dead;
				GAMEOFLIFE_create_pattern(&grid2, x_pattern, y_pattern, pattern_menu_index);
				DISPLAY_refresh(&grid2);
				state = PLACEMENT_VAISSEAU;
			}
			break;
		case PLACEMENT_VAISSEAU:
			if(BUTTON_center_read() == 1 && !button_enable){
				button_enable = true;
				info.state_info=LIVE;
				pattern_placed=false;
				DISPLAY_reset();
				state = MAIN;
			}
			break;
		case RESET_GRILLE:
			info.state_info=PAUSE;
			info.nb_generation=0;
		   	flag_reset=false;
			GAMEOFLIFE_reset();
			state = ARRET;
		default:
			break;
	}

	if(BUTTON_center_read() == 0 && BUTTON_up_read() == 0 && BUTTON_down_read() == 0 && BUTTON_right_read() == 0 && BUTTON_left_read() == 0){
		button_enable = false;
	}
}

/**
 * @brief Fonction principale de traitement du jeu.
 *
 * Exécute les actions associées à chaque état de la machine à états :
 * calcul des générations, gestion des menus, placement des motifs,
 * gestion de l'écran tactile et mise à jour de l'affichage.
 *
 * Cette fonction doit être appelée en continu dans la boucle principale.
 */
void GAMEOFLIFE_process_main(void)
{
	static grid_t *grid_prev;
	static grid_t *grid_next;
	static grid_t *grid_pattern;
	if(flag_generation || info.nb_generation < 1){
		if(!toggle){
			grid_prev = &grid0;
			grid_next = &grid1;
		}
		else{
			grid_prev = &grid1;
			grid_next = &grid0;
		}
	}
	switch(state){
		case MAIN:
			if(!pattern_placed){
				GAMEOFLIFE_create_pattern(grid_prev, x_pattern, y_pattern, pattern_menu_index);
				pattern_menu_index=0;
				pattern_placed=true;
			}
			GAMEOFLIFE_change_period();
			if(flag_generation || info.nb_generation < 1){
				info.nb_generation++;
				if(settings.grid==LINKED){
					GAMEOFLIFE_next_generation_v_2(grid_prev,grid_next);
				}
				else if(settings.grid==UNLINKED){
					GAMEOFLIFE_next_generation(grid_prev,grid_next);
				}
				toggle = !toggle;
				GAMEOFLIFE_create_cell_XPT2046(grid_next);

				if(settings.infos){
					DISPLAY_info(&info);
				}
				flag_generation = false;
			}
			DISPLAY_refresh(grid_next);
			break;
		case ARRET:
			if(flag_generation){
				GAMEOFLIFE_create_cell_XPT2046(grid_prev);
				DISPLAY_refresh(grid_prev);

				if(settings.infos){
					DISPLAY_info(&info);
				}

				flag_generation = false;
			}
			break;
		case MENU:
			if(!button_enable){
				if(BUTTON_down_read()==1 && settings.index<6){
					settings.controls=false;
					if(settings.mode==CLASIC){
						if(settings.index==1){
							settings.index+=3;
						}
						else{
							settings.index++;
						}
					}
					else{
						settings.index++;
					}
					button_enable=true;
					DISPLAY_menu(&settings);
				}
				else if(BUTTON_up_read()==1 && settings.index>0){
					settings.controls=false;
					if(settings.mode==CLASIC){
						if(settings.index==4){
							settings.index-=3;
						}
						else{
							settings.index--;
						}
					}
					else{
						settings.index--;
					}
					button_enable=true;
					DISPLAY_menu(&settings);
				}
				else if(BUTTON_right_read()==1){
					button_enable=true;
					if(settings.index==0){
						settings.grid=(settings.grid+1)%2;
					}
					else if(settings.index==1){
						settings.mode=(settings.mode+1)%2;
					}
					else if(settings.index==2){
						settings.index_born=(settings.index_born+1)%SETTINGS_TABLE_SIZE;
					}
					else if(settings.index==3){
						settings.index_survive=(settings.index_survive+1)%SETTINGS_TABLE_SIZE;
					}
					DISPLAY_menu(&settings);
				}
				else if(BUTTON_left_read()==1){
					button_enable=true;
					if(settings.index==0){
						settings.grid=(settings.grid+1)%2;
					}
					else if(settings.index==1){
						settings.mode=(settings.mode+1)%2;
					}
					if(settings.mode==ARCADE){
						if(settings.index==2){
							settings.index_born--;
							settings.index_born=(settings.index_born+SETTINGS_TABLE_SIZE)%SETTINGS_TABLE_SIZE;
						}
						else if(settings.index==3){
							settings.index_survive--;
							settings.index_survive=(settings.index_survive+SETTINGS_TABLE_SIZE)%SETTINGS_TABLE_SIZE;
						}
					}
					DISPLAY_menu(&settings);
				}
				else if(BUTTON_center_read()==1){
					button_enable=true;
					if(settings.mode==ARCADE){
						if(settings.index==2){
							settings.born[settings.index_born]=!settings.born[settings.index_born];
						}
						else if(settings.index==3){
							settings.survive[settings.index_survive]=!settings.survive[settings.index_survive];
						}
					}
					if(settings.index==4){
						settings.controls=!settings.controls;
					}
					else if(settings.index==5){
						settings.infos=!settings.infos;
					}
					else if(settings.index==6){
						settings.exit=true;
					}
					if(settings.index!=6){
						DISPLAY_menu(&settings);
					}
				}
			}
			break;
		case MENU_VAISSEAUX:
			static bool up;
			if(pattern_init){
				up=true;
				grid2=grid_dead;
				grid_pattern = &grid2;
				GAMEOFLIFE_create_pattern(grid_pattern, 62, 15, pattern_menu_index);
				GAMEOFLIFE_create_pattern(grid_pattern, 62, 40, pattern_menu_index+1);
				pattern_init=false;
				DISPLAY_refresh(grid_pattern);
				DISPLAY_menu_vaiseaux(up);
			}
			if(!button_enable){
				if(BUTTON_down_read()==1 && pattern_menu_index < PATTERN_NB-1){
					grid2=grid_dead;
					grid_pattern = &grid2;
					button_enable = true;
					pattern_menu_index++;
					up=false;
					GAMEOFLIFE_create_pattern(grid_pattern, 62, 15, pattern_menu_index-1);
					GAMEOFLIFE_create_pattern(grid_pattern, 62, 40, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
					DISPLAY_menu_vaiseaux(up);
				}
				else if(BUTTON_up_read()==1 && pattern_menu_index > 0){
					grid2=grid_dead;
					grid_pattern = &grid2;
					button_enable = true;
					pattern_menu_index--;
					up=true;
					GAMEOFLIFE_create_pattern(grid_pattern, 62, 15, pattern_menu_index);
					GAMEOFLIFE_create_pattern(grid_pattern, 62, 40, pattern_menu_index+1);
					DISPLAY_refresh(grid_pattern);
					DISPLAY_menu_vaiseaux(up);
				}
			}
			break;
		case PLACEMENT_VAISSEAU:{
			int16_t x_touch, y_touch;
			if(!button_enable){
				if(BUTTON_up_read()==1){
					button_enable=true;
					y_pattern=y_pattern-4;
					grid2=grid_dead;
					grid_pattern = &grid2;
					GAMEOFLIFE_create_pattern(grid_pattern, x_pattern, y_pattern, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
				}
				else if(BUTTON_down_read()==1){
					button_enable=true;
					y_pattern=y_pattern+4;
					grid2=grid_dead;
					grid_pattern = &grid2;
					GAMEOFLIFE_create_pattern(grid_pattern, x_pattern, y_pattern, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
				}
				else if(BUTTON_left_read()==1){
					button_enable=true;
					x_pattern=x_pattern-4;
					grid2=grid_dead;
					grid_pattern = &grid2;
					GAMEOFLIFE_create_pattern(grid_pattern, x_pattern, y_pattern, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
				}
				else if(BUTTON_right_read()==1){
					button_enable=true;
					x_pattern=x_pattern+4;
					grid2=grid_dead;
					grid_pattern = &grid2;
					GAMEOFLIFE_create_pattern(grid_pattern, x_pattern, y_pattern, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
				}
				else if(XPT2046_getMedianCoordinates(&x_touch, &y_touch, XPT2046_COORDINATE_SCREEN_RELATIVE)){
					x_pattern = SCREEN_WIDTH - (x_touch*SCREEN_TOUCH_RATIO_x-SCREEN_SHIFT_x);
					y_pattern = y_touch*SCREEN_TOUCH_RATIO_y-SCREEN_SHIFT_y;
					x_pattern = x_pattern/CELL_SIZE;
					y_pattern = y_pattern/CELL_SIZE;
					grid2=grid_dead;
					grid_pattern = &grid2;
					GAMEOFLIFE_create_pattern(grid_pattern, x_pattern, y_pattern, pattern_menu_index);
					DISPLAY_refresh(grid_pattern);
				}
			}
		}
			break;
		default:
			break;

		if(BUTTON_center_read() == 0 && BUTTON_up_read() == 0 && BUTTON_down_read() == 0 && BUTTON_right_read() == 0 && BUTTON_left_read() == 0){
			button_enable = false;
		}
	}
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Réinitialise complètement la grille.
 *
 * Toutes les cellules sont remises à l'état mort puis l'affichage
 * est rafraîchi afin de présenter une grille vide.
 */
static void GAMEOFLIFE_reset(void){
	uint8_t i,j;
	for(i = 0; i < NB_COLUMN; i++){
		for(j = 0; j < NB_LINE; j++){
			grid0.cells[i][j] = CELL_DEAD;
		}
	}
	grid1 = grid0;
	DISPLAY_reset();
	DISPLAY_refresh(&grid0);
}

/**
 * @brief Callback exécuté toutes les millisecondes.
 *
 * Génère un événement périodique permettant de déclencher
 * le calcul d'une nouvelle génération en fonction de la
 * période actuellement sélectionnée.
 */
static void GAMEOFLIFE_process_ms(void)
{
	t_generation = (t_generation + 1) % period;
	if(t_generation == 0){
		flag_generation = true;
	}
	//TODO
}

/**
 * @brief Détecte les appuis longs sur le bouton central.
 *
 * Permet d'identifier un appui prolongé afin de déclencher
 * une demande de réinitialisation de la grille.
 *
 * @note La durée d'appui est définie par TIME_LONG_PRESS.
 */
static void GAMEOFLIFE_BUTTON(void){
	//calcul appuie boutton prolonger (1sec)
	if(BUTTON_center_read() == 1 && (state == STOP || state == GO)){
	    if(reset_init < TIME_LONG_PRESS){
	        reset_init++;
	    }
	    if(reset_init >= TIME_LONG_PRESS && !button_enable){
	    	flag_reset=true;
	    	button_enable = true; // évite répétition
	    }
	}
	else{
		if(reset_init < TIME_LONG_PRESS && reset_init > 0){
		   	flag_reset=false;
		   	button_enable = true;
		}
	   	// bouton relâché → reset du système
	    reset_init = 0;
	}
}

/**
 * @brief Calcule la génération suivante avec des bords non connectés.
 *
 * Applique les règles du Jeu de la Vie sur la grille courante afin
 * de produire la génération suivante.
 *
 * Les cellules situées sur les bords ne possèdent pas de continuité
 * avec le côté opposé de la grille.
 *
 * @param[in]  grid_prev Grille représentant la génération courante.
 * @param[out] grid_next Grille recevant la génération suivante.
 */
static void GAMEOFLIFE_next_generation(grid_t * grid_prev, grid_t * grid_next)
{
	uint8_t x,y,i,j;
	uint8_t cell_alive_count;
	for(x=0; x<NB_COLUMN; x++){ //Parcours colonnes
		for(y=0; y<NB_LINE; y++){ //Parcours lignes
			cell_alive_count = 0;
			if(x > 0 && x+1 < NB_COLUMN){ //Evite de out of range dans la grille
				for(i=x-1; i<=x+1; i++){ //Parcours voisins colonnes
					if(y > 0 && y+1 < NB_LINE){ //Evite de out of range dans la grille
						for(j=y-1; j<=y+1; j++){ //Parcours voisins lignes
							if((grid_prev->cells[i][j] == CELL_ALIVE) && ((i!=x) || (j!=y))){
									cell_alive_count ++;
							}
						}
					}
				}
			}
			if(settings.mode == CLASIC){
				if(((grid_prev->cells[x][y] == CELL_ALIVE) && (cell_alive_count == 2)) || (cell_alive_count == 3)){ //Etat suivant de la cellule
					grid_next->cells[x][y] = CELL_ALIVE;
				}
				else{
					grid_next->cells[x][y] = CELL_DEAD;
				}
			}
			else if(settings.mode == ARCADE){
				if((grid_prev->cells[x][y] == CELL_ALIVE && settings.survive[cell_alive_count]) || (grid_prev->cells[x][y] == CELL_DEAD && settings.born[cell_alive_count])){
					grid_next->cells[x][y] = CELL_ALIVE;
				}
				else{
					grid_next->cells[x][y] = CELL_DEAD;
				}
			}
		}
	}
}

/**
 * @brief Calcule la génération suivante avec une grille torique.
 *
 * Applique les règles du Jeu de la Vie en considérant que les
 * bords de la grille sont connectés entre eux.
 *
 * Une cellule située sur un bord peut donc avoir comme voisins
 * des cellules situées sur le bord opposé.
 *
 * @param[in]  grid_prev Grille représentant la génération courante.
 * @param[out] grid_next Grille recevant la génération suivante.
 */
static void GAMEOFLIFE_next_generation_v_2(grid_t * grid_prev, grid_t * grid_next)
{
	int16_t i,j;
	int16_t a,b;
	uint8_t x,y,cell_alive_count;
	for(x=0; x<NB_COLUMN; x++){ //Parcours colonnes
		for(y=0; y<NB_LINE; y++){ //Parcours lignes
			cell_alive_count = 0;
			for(i=x-1; i<=x+1; i++){ //Parcours voisins colonnes
				for(j=y-1; j<=y+1; j++){ //Parcours voisins lignes
					a=i;
					b=j;
					GAMEOFLIFE_modulo(&i,&j);
					if((grid_prev->cells[i][j] == CELL_ALIVE) && ((i!=x) || (j!=y))){
						cell_alive_count ++;
					}
					if(i < x-1 || i > x+1){
						i=a;
					}
					if(j < y-1 || j > y+1){
						j=b;
					}
				}
			}

			if(settings.mode == CLASIC){
				if(((grid_prev->cells[x][y] == CELL_ALIVE) && (cell_alive_count == 2)) || (cell_alive_count == 3)){ //Etat suivant de la cellule
					grid_next->cells[x][y] = CELL_ALIVE;
				}
				else{
					grid_next->cells[x][y] = CELL_DEAD;
				}
			}
			else if(settings.mode == ARCADE){
				if((grid_prev->cells[x][y] == CELL_ALIVE && settings.survive[cell_alive_count]) || (grid_prev->cells[x][y] == CELL_DEAD && settings.born[cell_alive_count])){
					grid_next->cells[x][y] = CELL_ALIVE;
				}
				else{
					grid_next->cells[x][y] = CELL_DEAD;
				}
			}
		}
	}
}

/**
 * @brief Ramène des coordonnées dans les limites de la grille.
 *
 * Effectue un repliement modulo des coordonnées afin de simuler
 * une grille torique.
 *
 * @param[in,out] x Coordonnée horizontale.
 * @param[in,out] y Coordonnée verticale.
 */
static void GAMEOFLIFE_modulo(int16_t * x, int16_t * y){
	if((*x < 0 || *x >= NB_COLUMN) && (*y < 0 || *y >= NB_LINE)){
		*x = ((*x) + (NB_COLUMN)) % (NB_COLUMN);
		*y = ((*y) + (NB_LINE)) % (NB_LINE);
	}
	else if(*x < 0 || *x >= NB_COLUMN){
		*x = ((*x) + (NB_COLUMN)) % (NB_COLUMN);
	}
	else if(*y < 0 || *y >= NB_LINE){
		*y = ((*y) + (NB_LINE)) % (NB_LINE);
	}
}

/**
 * @brief Crée une cellule vivante à partir d'un appui tactile.
 *
 * Convertit les coordonnées tactiles en coordonnées de grille
 * puis active la cellule correspondante.
 *
 * @param[in,out] grid_next Grille à modifier.
 */
static void GAMEOFLIFE_create_cell_XPT2046(grid_t * grid_next){
	int16_t x, y;
	if(state == MAIN || state == ARRET){
		if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE)){
			x=x*SCREEN_TOUCH_RATIO_x-SCREEN_SHIFT_x;
			x=SCREEN_WIDTH-x;
			y=y*SCREEN_TOUCH_RATIO_y-SCREEN_SHIFT_y;
			if(x < 320 && y < 240){
				int16_t grid_x = x/CELL_SIZE;
				int16_t grid_y = y/CELL_SIZE;
				grid_next->cells[grid_x][grid_y] = CELL_ALIVE;
			}
		}
	}
}

/**
 * @brief Écrit l'état d'une cellule dans la grille.
 *
 * Vérifie que les coordonnées sont valides avant de modifier
 * la cellule demandée.
 *
 * @param[in,out] g Grille à modifier.
 * @param[in] x Coordonnée horizontale.
 * @param[in] y Coordonnée verticale.
 * @param[in] c Nouvel état de la cellule.
 */
static void grid_write(grid_t * g, uint16_t x, uint16_t y, cell_e c)
{
	if(x < NB_COLUMN && y < NB_LINE)
		g->cells[x][y] = c;
	else
		printf("niark\n");
}

/**
 * @brief Génère un motif prédéfini dans la grille.
 *
 * Place dans la grille un ensemble de cellules vivantes
 * correspondant au motif sélectionné.
 *
 * Les coordonnées fournies représentent le point d'ancrage
 * du motif.
 *
 * @param[in,out] grid Grille dans laquelle créer le motif.
 * @param[in] x Position horizontale du motif.
 * @param[in] y Position verticale du motif.
 * @param[in] pattern Motif à créer.
 */
static void GAMEOFLIFE_create_pattern(grid_t * grid, int16_t x, int16_t y, pattern_e pattern)
{
	switch(pattern)
	{
		case PATTERN_BOX:
			grid_write(grid, x+0, y+0, CELL_ALIVE);
			grid_write(grid, x+0, y+1, CELL_ALIVE);
			grid_write(grid, x+1, y+0, CELL_ALIVE);
			grid_write(grid, x+1, y+1, CELL_ALIVE);
			break;

		case PATTERN_BEEHIVE:
			grid_write(grid, x+2, y+4, CELL_ALIVE);
			grid_write(grid, x+3, y+3, CELL_ALIVE);
			grid_write(grid, x+3, y+5, CELL_ALIVE);
			grid_write(grid, x+4, y+3, CELL_ALIVE);
			grid_write(grid, x+5, y+4, CELL_ALIVE);
			grid_write(grid, x+4, y+5, CELL_ALIVE);
			break;

	    case PATTERN_TOAD:
	    	grid_write(grid, x+1, y+2, CELL_ALIVE);
			grid_write(grid, x+1, y+3, CELL_ALIVE);
			grid_write(grid, x+1, y+4, CELL_ALIVE);
			grid_write(grid, x+2, y+1, CELL_ALIVE);
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+3, CELL_ALIVE);
			break;

		case PATTERN_SHIP:
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+3, CELL_ALIVE);
			grid_write(grid, x+3, y+2, CELL_ALIVE);
			grid_write(grid, x+4, y+3, CELL_ALIVE);
			grid_write(grid, x+4, y+4, CELL_ALIVE);
			grid_write(grid, x+3, y+4, CELL_ALIVE);
			break;

		case PATTERN_GLIDER:
			grid_write(grid, x+8, y+3, CELL_ALIVE);
			grid_write(grid, x+7, y+4, CELL_ALIVE);
			grid_write(grid, x+6, y+4, CELL_ALIVE);
			grid_write(grid, x+7, y+5, CELL_ALIVE);
			grid_write(grid, x+8, y+5, CELL_ALIVE);
			break;

	    case PATTERN_QUEEN_BEE_SHUTTLE:
	    	grid_write(grid, x+0, y+8, CELL_ALIVE);
			grid_write(grid, x+0, y+9, CELL_ALIVE);
			grid_write(grid, x+1, y+10, CELL_ALIVE);
			grid_write(grid, x+2, y+11, CELL_ALIVE);
			grid_write(grid, x+3, y+11, CELL_ALIVE);
			grid_write(grid, x+4, y+11, CELL_ALIVE);
			grid_write(grid, x+5, y+10, CELL_ALIVE);
			grid_write(grid, x+6, y+9, CELL_ALIVE);
			grid_write(grid, x+6, y+8, CELL_ALIVE);
			break;

		case PATTERN_PULSAR:
			grid_write(grid, x+2, y+4, CELL_ALIVE);
			grid_write(grid, x+3, y+3, CELL_ALIVE);
			grid_write(grid, x+3, y+4, CELL_ALIVE);
			grid_write(grid, x+3, y+5, CELL_ALIVE);
			grid_write(grid, x+4, y+3, CELL_ALIVE);
			grid_write(grid, x+4, y+5, CELL_ALIVE);
			grid_write(grid, x+5, y+3, CELL_ALIVE);
			grid_write(grid, x+5, y+4, CELL_ALIVE);
			grid_write(grid, x+5, y+5, CELL_ALIVE);
			grid_write(grid, x+6, y+4, CELL_ALIVE);
			break;

		case PATTERN_BLINKER:
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+3, CELL_ALIVE);
			grid_write(grid, x+2, y+4, CELL_ALIVE);
			break;

		case PATTERN_PENTADECATHLON:
			grid_write(grid, x+5, y+0, CELL_ALIVE);
			grid_write(grid, x+5, y+1, CELL_ALIVE);
			grid_write(grid, x+5, y+2, CELL_ALIVE);
			grid_write(grid, x+5, y+3, CELL_ALIVE);
			grid_write(grid, x+5, y+4, CELL_ALIVE);
			grid_write(grid, x+5, y+5, CELL_ALIVE);
			grid_write(grid, x+5, y+6, CELL_ALIVE);
			grid_write(grid, x+5, y+7, CELL_ALIVE);
			grid_write(grid, x+5, y+8, CELL_ALIVE);
			grid_write(grid, x+5, y+9, CELL_ALIVE);
			break;

		case PATTERN_GLIDER_GUN:
			grid_write(grid, x+0, y+3, CELL_ALIVE);
			grid_write(grid, x+0, y+4, CELL_ALIVE);
			grid_write(grid, x+1, y+3, CELL_ALIVE);
			grid_write(grid, x+1, y+4, CELL_ALIVE);
			grid_write(grid, x+10, y+2, CELL_ALIVE);
			grid_write(grid, x+10, y+3, CELL_ALIVE);
			grid_write(grid, x+10, y+4, CELL_ALIVE);
			grid_write(grid, x+11, y+1, CELL_ALIVE);
			grid_write(grid, x+11, y+5, CELL_ALIVE);
			grid_write(grid, x+12, y+0, CELL_ALIVE);
			grid_write(grid, x+12, y+6, CELL_ALIVE);
			grid_write(grid, x+13, y+0, CELL_ALIVE);
			grid_write(grid, x+13, y+6, CELL_ALIVE);
			grid_write(grid, x+14, y+3, CELL_ALIVE);
			grid_write(grid, x+15, y+1, CELL_ALIVE);
			grid_write(grid, x+15, y+5, CELL_ALIVE);
			grid_write(grid, x+16, y+2, CELL_ALIVE);
			grid_write(grid, x+16, y+3, CELL_ALIVE);
			grid_write(grid, x+16, y+4, CELL_ALIVE);
			grid_write(grid, x+17, y+3, CELL_ALIVE);
			grid_write(grid, x+20, y+4, CELL_ALIVE);
			grid_write(grid, x+20, y+5, CELL_ALIVE);
			grid_write(grid, x+20, y+6, CELL_ALIVE);
			grid_write(grid, x+21, y+4, CELL_ALIVE);
			grid_write(grid, x+21, y+5, CELL_ALIVE);
			grid_write(grid, x+21, y+6, CELL_ALIVE);
			grid_write(grid, x+22, y+3, CELL_ALIVE);
			grid_write(grid, x+22, y+7, CELL_ALIVE);
			grid_write(grid, x+24, y+2, CELL_ALIVE);
			grid_write(grid, x+24, y+3, CELL_ALIVE);
			grid_write(grid, x+24, y+7, CELL_ALIVE);
			grid_write(grid, x+24, y+8, CELL_ALIVE);
			grid_write(grid, x+34, y+5, CELL_ALIVE);
			grid_write(grid, x+34, y+6, CELL_ALIVE);
			grid_write(grid, x+35, y+5, CELL_ALIVE);
			grid_write(grid, x+35, y+6, CELL_ALIVE);
			break;

		case PATTERN_LIGHT_WEIGHT_SPACESHIP:
			grid_write(grid, x+0, y+1, CELL_ALIVE);
			grid_write(grid, x+0, y+2, CELL_ALIVE);
			grid_write(grid, x+1, y+1, CELL_ALIVE);
			grid_write(grid, x+1, y+2, CELL_ALIVE);
			grid_write(grid, x+1, y+3, CELL_ALIVE);
			grid_write(grid, x+2, y+0, CELL_ALIVE);
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+3, CELL_ALIVE);
			grid_write(grid, x+3, y+0, CELL_ALIVE);
			grid_write(grid, x+3, y+1, CELL_ALIVE);
			grid_write(grid, x+3, y+2, CELL_ALIVE);
			grid_write(grid, x+4, y+1, CELL_ALIVE);
			break;

		case PATTERN_MIDDLE_WEIGHT_SPACESHIP:
			grid_write(grid, x+0, y+1, CELL_ALIVE);
			grid_write(grid, x+0, y+2, CELL_ALIVE);
			grid_write(grid, x+1, y+0, CELL_ALIVE);
			grid_write(grid, x+1, y+1, CELL_ALIVE);
			grid_write(grid, x+1, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+0, CELL_ALIVE);
			grid_write(grid, x+2, y+1, CELL_ALIVE);
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+3, y+0, CELL_ALIVE);
			grid_write(grid, x+3, y+1, CELL_ALIVE);
			grid_write(grid, x+3, y+3, CELL_ALIVE);
			grid_write(grid, x+4, y+1, CELL_ALIVE);
			grid_write(grid, x+4, y+2, CELL_ALIVE);
			grid_write(grid, x+4, y+3, CELL_ALIVE);
			grid_write(grid, x+5, y+2, CELL_ALIVE);
			break;

		case PATTERN_HEAVY_WEIGHT_SPACESHIP:
			grid_write(grid, x+0, y+1, CELL_ALIVE);
			grid_write(grid, x+0, y+2, CELL_ALIVE);
			grid_write(grid, x+1, y+0, CELL_ALIVE);
			grid_write(grid, x+1, y+1, CELL_ALIVE);
			grid_write(grid, x+1, y+2, CELL_ALIVE);
			grid_write(grid, x+2, y+0, CELL_ALIVE);
			grid_write(grid, x+2, y+1, CELL_ALIVE);
			grid_write(grid, x+2, y+2, CELL_ALIVE);
			grid_write(grid, x+3, y+0, CELL_ALIVE);
			grid_write(grid, x+3, y+1, CELL_ALIVE);
			grid_write(grid, x+3, y+2, CELL_ALIVE);
			grid_write(grid, x+4, y+0, CELL_ALIVE);
			grid_write(grid, x+4, y+1, CELL_ALIVE);
			grid_write(grid, x+4, y+3, CELL_ALIVE);
			grid_write(grid, x+5, y+1, CELL_ALIVE);
			grid_write(grid, x+5, y+2, CELL_ALIVE);
			grid_write(grid, x+5, y+3, CELL_ALIVE);
			grid_write(grid, x+6, y+2, CELL_ALIVE);
			break;

		default:
			break;
	}
}

/**
 * @brief Modifie la vitesse de simulation.
 *
 * Permet à l'utilisateur d'augmenter ou de diminuer la période
 * séparant deux générations successives à l'aide des boutons.
 *
 * La nouvelle période est également reportée dans la structure
 * d'informations affichée à l'écran.
 */
static void GAMEOFLIFE_change_period(void){
	if(!button_enable){
	    if(BUTTON_down_read() == 1){
	        button_enable = true;
	        switch(period){
	            case GENERATION_PERIOD_10_MS:
	                period = GENERATION_PERIOD_20_MS;
	                break;
	            case GENERATION_PERIOD_20_MS:
	                period = GENERATION_PERIOD_50_MS;
	                break;
	            case GENERATION_PERIOD_50_MS:
	                period = GENERATION_PERIOD_100_MS;
	                break;
	            case GENERATION_PERIOD_100_MS:
	                period = GENERATION_PERIOD_200_MS;
	                break;
	            case GENERATION_PERIOD_200_MS:
	                period = GENERATION_PERIOD_300_MS;
	                break;
	            case GENERATION_PERIOD_300_MS:
	                period = GENERATION_PERIOD_400_MS;
	                break;
	            case GENERATION_PERIOD_400_MS:
	                period = GENERATION_PERIOD_500_MS;
	                break;
	            case GENERATION_PERIOD_500_MS:
	                period = GENERATION_PERIOD_1000_MS;
	                break;
	            default:
	                break;
	        }
	    }
	    else if(BUTTON_up_read() == 1){
	        button_enable = true;
	        switch(period){
	            case GENERATION_PERIOD_1000_MS:
	                period = GENERATION_PERIOD_500_MS;
	                break;
	            case GENERATION_PERIOD_500_MS:
	                period = GENERATION_PERIOD_400_MS;
	                break;
	            case GENERATION_PERIOD_400_MS:
	                period = GENERATION_PERIOD_300_MS;
	                break;
	            case GENERATION_PERIOD_300_MS:
	                period = GENERATION_PERIOD_200_MS;
	                break;
	            case GENERATION_PERIOD_200_MS:
	                period = GENERATION_PERIOD_100_MS;
	                break;
	            case GENERATION_PERIOD_100_MS:
	                period = GENERATION_PERIOD_50_MS;
	                break;
	            case GENERATION_PERIOD_50_MS:
	                period = GENERATION_PERIOD_20_MS;
	                break;
	            case GENERATION_PERIOD_20_MS:
	                period = GENERATION_PERIOD_10_MS;
	                break;
	            default:
	                break;
	        }
	    }
	}
	info.period=period;
}
