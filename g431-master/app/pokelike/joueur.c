#include "joueur.h"
#include "carte.h"
#include "bloc.h"
#include "config.h"
#include "tft_ili9341/stm32g4_ili9341.h"
#include "stm32g4_gpio.h"

/* Taille bloc */
#define TAILLE_BLOC 20

/* Position du joueur */
static int joueurX = 2;
static int joueurY = 2;

/* Ancienne position (pour effacer) */
static int ancienX = 2;
static int ancienY = 2;

/* Sprite */
#define SPRITE_TAILLE 8

#define TRANSPARENT 0x0000
#define NOIR 0x0000
#define BLANC 0xFFFF
#define ROUGE 0xF800
#define BLEU 0x001F

const uint16_t joueur_sprite[SPRITE_TAILLE][SPRITE_TAILLE] = {
    {TRANSPARENT,TRANSPARENT,ROUGE,ROUGE,ROUGE,ROUGE,TRANSPARENT,TRANSPARENT},
    {TRANSPARENT,ROUGE,BLANC,BLANC,BLANC,BLANC,ROUGE,TRANSPARENT},
    {ROUGE,BLANC,NOIR,BLANC,BLANC,NOIR,BLANC,ROUGE},
    {ROUGE,BLANC,BLANC,BLANC,BLANC,BLANC,BLANC,ROUGE},
    {ROUGE,BLANC,BLEU,BLANC,BLANC,BLEU,BLANC,ROUGE},
    {ROUGE,BLANC,BLANC,BLANC,BLANC,BLANC,BLANC,ROUGE},
    {TRANSPARENT,ROUGE,BLANC,BLANC,BLANC,BLANC,ROUGE,TRANSPARENT},
    {TRANSPARENT,TRANSPARENT,ROUGE,ROUGE,ROUGE,ROUGE,TRANSPARENT,TRANSPARENT}
};

/**
 * @brief Initialise le joueur
 */
void Joueur_Init(void)
{
    joueurX = 2;
    joueurY = 2;

    ancienX = joueurX;
    ancienY = joueurY;
}

/**
 * @brief Dessine le sprite
 */
void Joueur_DessinerSprite(int pixelX, int pixelY)
{
    for(int y = 0; y < SPRITE_TAILLE; y++)
    {
        for(int x = 0; x < SPRITE_TAILLE; x++)
        {
            uint16_t color = joueur_sprite[y][x];

            if(color != TRANSPARENT)
            {
                ILI9341_DrawPixel(pixelX + x, pixelY + y, color);
            }
        }
    }
}

/**
 * @brief Affiche le joueur
 */
void Joueur_Afficher(void)
{
    int pixelX = joueurX * TAILLE_BLOC + (TAILLE_BLOC - SPRITE_TAILLE)/2;
    int pixelY = joueurY * TAILLE_BLOC + (TAILLE_BLOC - SPRITE_TAILLE)/2;

    Joueur_DessinerSprite(pixelX, pixelY);
}

/**
 * @brief Efface l'ancien joueur (redessine le bloc dessous)
 */
void Joueur_Effacer(void)
{
    int pixelX = ancienX * TAILLE_BLOC;
    int pixelY = ancienY * TAILLE_BLOC;

    TypeBloc bloc = Carte_GetBloc(ancienX, ancienY);
    uint16_t couleur = Bloc_GetCouleur(bloc);

    for(int y = 0; y < TAILLE_BLOC; y++)
    {
        for(int x = 0; x < TAILLE_BLOC; x++)
        {
            ILI9341_DrawPixel(pixelX + x, pixelY + y, couleur);
        }
    }
}

/**
 * @brief Mise à jour du joueur
 */
void Joueur_Update(void)
{
    int newX = joueurX;
    int newY = joueurY;

    /* Sauvegarde ancienne position */
    ancienX = joueurX;
    ancienY = joueurY;

    int moved = 0;

    /* Lecture boutons */
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN))
    {
        newY--;
        moved = 1;
    }

    if (HAL_GPIO_ReadPin(GPIO_BUTTON_UP, PIN_BUTTON_UP))
    {
        newY++;
        moved = 1;
    }

    if (HAL_GPIO_ReadPin(GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT))
    {
        newX--;
        moved = 1;
    }

    if (HAL_GPIO_ReadPin(GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT))
    {
        newX++;
        moved = 1;
    }

    /* Collision */
    TypeBloc bloc = Carte_GetBloc(newX, newY);

    if (Bloc_EstTraversable(bloc))
    {
        joueurX = newX;
        joueurY = newY;

        if(moved)
        {
            HAL_Delay(120);
        }
    }
}
