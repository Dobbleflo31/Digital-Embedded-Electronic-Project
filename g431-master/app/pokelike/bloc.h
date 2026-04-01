#ifndef POKELIKE_BLOC_H_
#define POKELIKE_BLOC_H_

#include <stdint.h>

/**
 * @brief Types de blocs disponibles dans le jeu
 */
typedef enum {
    BLOC_HERBE = 0,
    BLOC_MUR,
    BLOC_EAU,
    BLOC_SABLE,
    BLOC_MAX
} TypeBloc;

/**
 * @brief Retourne la couleur associée à un bloc
 * @param type Type du bloc
 * @return Couleur au format ILI9341
 */
uint16_t Bloc_GetCouleur(TypeBloc type);

/**
 * @brief Indique si le bloc est traversable
 * @param type Type du bloc
 * @return 1 si traversable, 0 sinon
 */
uint8_t Bloc_EstTraversable(TypeBloc type);

#endif /* POKELIKE_BLOCK_H_ */
