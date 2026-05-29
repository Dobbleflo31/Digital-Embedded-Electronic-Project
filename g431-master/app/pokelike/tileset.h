/**
 * @file : tileset.h
 * @date: 27 avr. 2026
 * @author: Florian
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
extern const uint16_t tile_sommet_toit[256];
extern const uint16_t tile_porte_milieu[256];
extern const uint16_t tile_murs_fenetre[256];
extern const uint16_t tile_toit_gauche[256];
extern const uint16_t tile_toit_droite[256];
extern const uint16_t tile_fondation[256];
extern const uint16_t tile_coin_fondation[256];


/* Fonction utilitaire */
const uint16_t* Tileset_GetTile(uint16_t index);

#endif /* POKELIKE_TILESET_H_ */
