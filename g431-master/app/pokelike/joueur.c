#include "joueur.h"
#include "carte.h"
#include "config.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "stm32g4_gpio.h"
#include "usart.h"

#define TAILLE_BLOC 16

/* Position du joueur en cases */
static int joueurX = 10;
static int joueurY = 10;

/* Ancienne position (pour effacer) */
static int ancienX = 10;
static int ancienY = 10;

static MapID_t mapActuelle = MAP_CENTRE;

void Joueur_Init(void)
{
    joueurX = 10;
    joueurY = 10;
    ancienX = joueurX;
    ancienY = joueurY;
    mapActuelle = MAP_CENTRE;
    Carte_Load(MAP_CENTRE);
}

/**
 * @brief Dessine un carré rouge 4x4 centré
 */

void Joueur_DessinerSprite(int pixelX, int pixelY)
{
    // On dessine un bloc de 4x4 pixels (plus visible qu'un seul pixel !)
    // On commence à +6 pour que le carré soit centré (6 + 4 + 6 = 16)
    for(int y = 6; y < 10; y++)
    {
        for(int x = 6; x < 10; x++)
        {
            ILI9341_DrawPixel(pixelX + x, pixelY + y, 0xF800); // Rouge
        }
    }
}


/**
 * @brief Affiche le joueur
 */

void Joueur_Afficher(void)
{
    // On multiplie par 16 pour avoir les coordonnées pixels
    int pixelX = joueurX * TAILLE_BLOC;
    int pixelY = joueurY * TAILLE_BLOC;
    Joueur_DessinerSprite(pixelX, pixelY);
}

void Joueur_Effacer(void)
{
    // Redessine la tuile de la carte à l'ancienne position
    draw_tile(Carte_GetBloc(ancienX, ancienY), ancienX * TAILLE_BLOC, ancienY * TAILLE_BLOC);
}

void Joueur_Update(void)
{
    int newX = joueurX;
    int newY = joueurY;
    int moved = 0;

    /* Sauvegarde pour l'effacement */
    ancienX = joueurX;
    ancienY = joueurY;

    /* Lecture boutons (Correction des axes Y et X) */
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_UP, PIN_BUTTON_UP))    { newY++; moved = 1; }
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN)){ newY--; moved = 1; }
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT)){ newX++; moved = 1; }
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT)){ newX--; moved = 1; }

    if (!moved) return;

    uint16_t bloc = Carte_GetBloc(newX, newY);

    // --- LOGIQUE DE CHANGEMENT DE CARTE (BLOC 99) ---
    if (bloc == 99)
    {
        MapID_t prochaineMap = mapActuelle;
        int transportX = newX;
        int transportY = newY;

        // Transitions depuis le CENTRE
        if (newY < 0 && mapActuelle == MAP_CENTRE) { prochaineMap = MAP_NORD; transportY = 19; }
        else if (newY >= 20 && mapActuelle == MAP_CENTRE) { prochaineMap = MAP_SUD; transportY = 0; }
        else if (newX < 0 && mapActuelle == MAP_CENTRE) { prochaineMap = MAP_OUEST; transportX = 19; }
        else if (newX >= 20 && mapActuelle == MAP_CENTRE) { prochaineMap = MAP_EST; transportX = 0; }

        // Retours au CENTRE
        else if (mapActuelle == MAP_NORD && newY >= 20) { prochaineMap = MAP_CENTRE; transportY = 0; }
        else if (mapActuelle == MAP_SUD && newY < 0)    { prochaineMap = MAP_CENTRE; transportY = 19; }
        else if (mapActuelle == MAP_OUEST && newX >= 20){ prochaineMap = MAP_CENTRE; transportX = 0; }
        else if (mapActuelle == MAP_EST && newX < 0)    { prochaineMap = MAP_CENTRE; transportX = 19; }

        if (prochaineMap != mapActuelle) {
            mapActuelle = prochaineMap;
            Carte_Load(mapActuelle);
            joueurX = transportX;
            joueurY = transportY;

            ILI9341_Fill(0x0000);
            Carte_Afficher();
            HAL_Delay(150); // Petit délai pour ne pas enchaîner 2 maps trop vite
            return;
        }
    }

    // --- MISE À JOUR POSITION ---
    // Pour le moment on autorise tout sauf le bloc 99 (hors map)
    if (bloc != 99)
    {
        joueurX = newX;
        joueurY = newY;
        HAL_Delay(120);
    }
}
