/*
 * pokemon.c
 *
 *  Created on: 6 mai 2026
 *      Author: maxim
 */

#include "../Pokemons/Eau/Carapuce.h"
#include "../Pokemons/Eau/Carabaffe.h"
#include "../Pokemons/Eau/Tortank.h"
#include "../Pokemons/Feu/Salameche.h"
#include "../Pokemons/Feu/Reptincel.h"
#include "../Pokemons/Feu/Dracaufeu.h"
#include "../Pokemons/Plante/Bulbizarre.h"
#include "../Pokemons/Plante/Herbizarre.h"
#include "../Pokemons/Plante/Florizarre.h"

uint16_t * pokemon_get_sprite(int n){
	switch(n){
		case 0:
			return carapuce_map;
		case 1:
			return carabaffe_map;
		case 2:
			return tortank_map;
		case 3:
			return salameche_map;
		case 4:
			return reptincel_map;
		case 5:
			return dracaufeu_map;
		case 6:
			return bulbizarre_map;
		case 7:
			return herbizarre_map;
		case 8:
			return florizarre_map;
		default:
			return carapuce_map;
	}
}
