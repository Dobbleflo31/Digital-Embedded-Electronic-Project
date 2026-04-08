/*
 * carte.c
 *
 *  Created on: 1 avr. 2026
 *      Author: flori
 */
#include "carte.h"
#include "bloc.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"

/* Taille d’un bloc en pixels */
#define TAILLE_BLOC 20

/* Dimensions de la carte */
#define CARTE_LARGEUR  16
#define CARTE_HAUTEUR  12

/**
 * @brief Matrice de la carte généré par chat faut pas déconner
 */
static TypeBloc carte[CARTE_HAUTEUR][CARTE_LARGEUR] = {
    {BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_EAU,   BLOC_EAU,   BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_EAU,   BLOC_EAU,   BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_SABLE, BLOC_SABLE, BLOC_SABLE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_SABLE, BLOC_SABLE, BLOC_SABLE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_HERBE, BLOC_MUR},

    {BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR, BLOC_MUR}
};

/**
 * @brief Initialise la carte (vide pour l’instant)
 */
void Carte_Init(void)
{
}

TypeBloc Carte_GetBloc(int x, int y)
{
    return carte[y][x];
}
/**
 * @brief Affiche toute la carte
 */
void Carte_Afficher(void)
{
    for(int y = 0; y < CARTE_HAUTEUR; y++)
    {
        for(int x = 0; x < CARTE_LARGEUR; x++)
        {
            TypeBloc type = carte[y][x];

            //uint16_t couleur = Bloc_GetCouleur(type);

            uint16_t couleur;

            if(type == BLOC_MUR)
                couleur = ILI9341_COLOR_GRAY;
            else
                couleur = ILI9341_COLOR_GREEN;

            /* Calcul position écran */
            int pixelX = x * TAILLE_BLOC;
            int pixelY = y * TAILLE_BLOC;

            /* Dessine un rectanbgle */
           // ILI9341_DrawFilledRectangle(pixelX, pixelY, TAILLE_BLOC, TAILLE_BLOC, couleur);

            for(int i = 0; i < TAILLE_BLOC; i++)
            {
                for(int j = 0; j < TAILLE_BLOC; j++)
                {
                    ILI9341_DrawPixel(pixelX + j, pixelY + i, couleur);
                }
            }
        }
    }
}
