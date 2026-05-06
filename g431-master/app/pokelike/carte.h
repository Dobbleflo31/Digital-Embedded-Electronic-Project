#ifndef POKELIKE_CARTE_H_
#define POKELIKE_CARTE_H_
#include "bloc.h"
#include <stdint.h>
/* === CONFIG TILESET === */
#define TILE_SIZE     16
#define TILE_SIZE      16
/* Dimensions écran en tiles */
#define CARTE_LARGEUR  20
#define CARTE_HAUTEUR  20


// Énumération pour identifier nos 5 zones
typedef enum {
    MAP_CENTRE = 0,
    MAP_NORD,
    MAP_SUD,
    MAP_EST,
    MAP_OUEST,
    MAP_COUNT
} MapID_t;
/**
 * @brief Initialise la carte
 */
void Carte_Init(void);
void Carte_Load(MapID_t mapID);
/**
 * @brief Affiche toute la carte à l'écran
 */
void Carte_Afficher(void);

/**
 * @brief Retourne le type de bloc à une position
 */

uint16_t Carte_GetBloc(int x, int y);
void draw_tile(uint16_t tileIndex, int screenX, int screenY);


#endif /* POKELIKE_CARTE_H_ */
