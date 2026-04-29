/*
 * carte.c
 *
 *  Created on: 1 avr. 2026
 *      Author: flori
 */

#include "carte.h"
#include "tileset.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "tft_ili9341/stm32g4_fonts.h"

/* === CONFIG TILESET === */
#define TILE_SIZE     16

/* Dimensions écran en tiles */
#define CARTE_LARGEUR  20
#define CARTE_HAUTEUR  20

/* === MAP (INDEX DE TILE) === */
static uint16_t carte[CARTE_HAUTEUR][CARTE_LARGEUR] =
{
    {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
    {0,2,2,2,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,2,2,2,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,3,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,3,1,1,1,0},
	{0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},

};

/* === INIT === */
void Carte_Init(void)
{
}

/* =========================================================
 *  DRAW TILE
 * ========================================================= */
void draw_tile(uint16_t tileIndex, int screenX, int screenY)
{
    const uint16_t* tile = Tileset_GetTile(tileIndex);

    for(int y = 0; y < TILE_SIZE; y++)
    {
        for(int x = 0; x < TILE_SIZE; x++)
        {
            uint16_t color = tile[y * TILE_SIZE + x];
            ILI9341_DrawPixel(screenX + x, screenY + y, color);
        }
    }
}

/* === AFFICHAGE COMPLET === */
void Carte_Afficher(void)
{
    for(int y = 0; y < CARTE_HAUTEUR; y++)
    {
        for(int x = 0; x < CARTE_LARGEUR; x++)
        {
            draw_tile(
                carte[y][x],
                x * TILE_SIZE,
                y * TILE_SIZE
            );
        }
    }
}

/* === GET BLOC (pour joueur) === */
uint16_t Carte_GetBloc(int x, int y)
{
    if(x < 0 || x >= CARTE_LARGEUR || y < 0 || y >= CARTE_HAUTEUR)
        return 0;

    return carte[y][x];
}
