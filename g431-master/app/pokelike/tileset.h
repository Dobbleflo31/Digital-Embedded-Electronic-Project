/*
 * tileset.h
 *
 *  Created on: 27 avr. 2026
 *      Author: flori
 */
#ifndef TILESET_H
#define TILESET_H

#include <stdint.h>

/* Taille d'une tile */
#define TILE_SIZE 16

/* Tiles individuelles */
extern const uint16_t tile_herbe[256];
extern const uint16_t tile_eau[256];
extern const uint16_t tile_route[256];
extern const uint16_t tile_arbre[256];
extern const uint16_t tile_etang[256];

/* Fonction utilitaire */
const uint16_t* Tileset_GetTile(uint16_t index);

#endif /* POKELIKE_TILESET_H_ */
