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


// Pointeur vers la carte active
static const uint16_t (*carte_actuelle)[CARTE_LARGEUR];
static MapID_t currentMapID;

/* === DEFINITION DU MONDE === */
static const uint16_t monde[MAP_COUNT][CARTE_HAUTEUR][CARTE_LARGEUR] =
{
    [MAP_CENTRE] = {
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0}, // Sortie NORD (x=9,10)
        {0,1,1,1,1,1,1,1,1,2,2,0,0,0,0,0,0,0,0,0},
        {0,2,2,2,1,1,1,1,1,2,2,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,2,2,0,0,5,6,7,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,2,2,0,0,8,9,8,0,0,0,0},
        {2,2,2,2,2,2,2,2,2,2,2,0,0,10,10,10,0,0,0,0}, // Sortie OUEST (y=5)
        {0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2}, // Sortie EST (y=6)
        {0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,0},
        {0,1,1,1,1,1,1,1,1,2,2,1,1,1,1,3,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0}, // Sortie SUD (x=9,10)
    },

    [MAP_NORD] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
 [19] = {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
 	 	{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},// Entrée vers CENTRE
    },

    [MAP_SUD] = {
  [0] = {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0}, // Entrée vers CENTRE
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0},
    },

    [MAP_OUEST] = {
        [5] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2}, // Entrée vers CENTRE (y=5)
    },

    [MAP_EST] = {
        [6] = {2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Entrée vers CENTRE (y=6)
    }
};

/* === FONCTIONS === */

void Carte_Init(void) {
    Carte_Load(MAP_CENTRE);
}

void Carte_Load(MapID_t mapID) {
    if(mapID < MAP_COUNT) {
        currentMapID = mapID;
        carte_actuelle = monde[mapID];
    }
}

uint16_t Carte_GetBloc(int x, int y) {
    if(x < 0 || x >= CARTE_LARGEUR || y < 0 || y >= CARTE_HAUTEUR)
        return 99; // Code pour "Changement de map requis"

    return carte_actuelle[y][x];
}

void draw_tile(uint16_t tileIndex, int screenX, int screenY) {
    const uint16_t* tile = Tileset_GetTile(tileIndex);
    for(int y = 0; y < TILE_SIZE; y++) {
        for(int x = 0; x < TILE_SIZE; x++) {
            uint16_t color = tile[y * TILE_SIZE + x];
            if(color != 0x0000) ILI9341_DrawPixel(screenX + x, screenY + y, color);
        }
    }
}

void Carte_Afficher(void) {
    for(int y = 0; y < CARTE_HAUTEUR; y++) {
        for(int x = 0; x < CARTE_LARGEUR; x++) {
            uint16_t currentTile = carte_actuelle[y][x];
            // Fond herbe (0) pour les maisons (5+) ou arbres (3)
            if(currentTile >= 3) {
                draw_tile(0, x * TILE_SIZE, y * TILE_SIZE);
            }
            draw_tile(currentTile, x * TILE_SIZE, y * TILE_SIZE);
        }
    }
}
