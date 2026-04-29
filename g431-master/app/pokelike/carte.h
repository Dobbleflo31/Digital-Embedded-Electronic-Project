#ifndef POKELIKE_CARTE_H_
#define POKELIKE_CARTE_H_
#include "bloc.h"
/**
 * @brief Initialise la carte
 */
void Carte_Init(void);

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
