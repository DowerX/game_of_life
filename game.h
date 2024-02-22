#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// egy cell allapotat jellemzo enum
typedef enum {
    DEAD  = false,
    ALIVE = true
} CELL_STATE;

// a jatek allapotat jellemzo struct
typedef struct {
    size_t   width;              // jatekter szelessege
    size_t   height;             // jatekter magassaga
    uint8_t* playfield;          // aktivan megjelenitett jatekter
    uint8_t* incative_playfield; // ideiglenes jatekter, a szabalyok futtatasa kozben szukseges
} GameState;

// ures jatekter leterhozasa es felszabaditasa
GameState game_allocate(size_t width, size_t height);
void      game_free(GameState game);

// jatek allapotanak betoltese es elmentes fajlba
bool game_load_state(GameState* game, const char* path);
bool game_save_state(GameState* game, const char* path);

// jatek allapotanak leptetese
void game_step(GameState* game);

// a jatekter cellainak manipulacioja
CELL_STATE playfield_get(GameState* game, int64_t x, int64_t y);
void       playfield_set(GameState* game, int64_t x, int64_t y, CELL_STATE state, bool incative);
void       playfield_print(GameState* game, bool clear_screen);

#endif