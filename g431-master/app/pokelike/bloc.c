/*
 * bloc.c
 *
 *  Created on: 1 avr. 2026
 *      Author: flori
 */

#include "bloc.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"

/**
 * @brief Associe chaque type de bloc à une couleur
 */
/*uint16_t Bloc_GetCouleur(TypeBloc type)
{
    switch(type)
    {
        case BLOC_HERBE:
            return ILI9341_COLOR_GREEN;

        case BLOC_MUR:
            return ILI9341_COLOR_GRAY;

        case BLOC_EAU:
            return ILI9341_COLOR_BLUE;

        case BLOC_SABLE:
            return ILI9341_COLOR_YELLOW;

        default:
            return ILI9341_COLOR_RED;
    }
}*/

/**
 * @brief Définit si le bloc est traversable
 */
/*uint8_t Bloc_EstTraversable(TypeBloc type)
{
    switch(type)
    {
        case BLOC_HERBE:
        case BLOC_SABLE:
            return 1;

        case BLOC_MUR:
        case BLOC_EAU:
            return 0;

        default:
            return 0;
    }
}*/
