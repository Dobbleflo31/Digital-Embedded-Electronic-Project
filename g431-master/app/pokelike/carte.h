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
TypeBloc Carte_GetBloc(int x, int y);



#endif /* POKELIKE_CARTE_H_ */
