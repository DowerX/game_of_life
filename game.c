#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// a terminalban valo megjeleniteshez
// szukeseges specialis escape sequencek
#define CLEAR_SCREEN "\033[2J"
#define HOME_CURSOR  "\033[H"
#define CLEAR        "\033[0m"
#define FULL_WHITE   "\033[37;47m"
#define FULL_BLACK   "\033[30;40m"

GameState game_allocate(size_t width, size_t height) {
    // struktura lefoglalasa
    GameState game = {
        width,
        height,
        (uint8_t*) malloc(sizeof(uint8_t) * (((width * height) / 8) + 1)),
        (uint8_t*) malloc(sizeof(uint8_t) * (((width * height) / 8) + 1))
    };

    // jatekter uresre inicalizalasa
    memset(game.playfield, 0, sizeof(uint8_t) * (((width * height) / 8) + 1));
    memset(game.incative_playfield, 0, sizeof(uint8_t) * (((width * height) / 8) + 1));

    return game;
}

void game_free(GameState game) {
    if (game.playfield != NULL)
        free(game.playfield);
    if (game.incative_playfield != NULL)
        free(game.incative_playfield);
}

bool game_load_state(GameState* game, const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL)
        return false;

    // jatekter meretenek beolvasasa
    if (2 != fscanf(file, "%lu %lu", &game->width, &game->height))
        return false;

    // uj jatekter foglalasa
    game_free(*game);
    game->playfield = (uint8_t*) malloc(sizeof(uint8_t) * (((game->width * game->height) / 8) + 1));
    game->incative_playfield = (uint8_t*) malloc(sizeof(uint8_t) * (((game->width * game->height) / 8) + 1));

    // cellak beallitasa
    for(size_t y = 0; y < game->height; y++) {
        for(size_t x = 0; x < game->width; x++) {
            char c = fgetc(file);
            if (c == '\n')
                c = fgetc(file);
            CELL_STATE state = DEAD;
            switch (c) {
            case EOF:
                return false;
            case '1':
                state = ALIVE;
            }

            playfield_set(game, x, y, state, false);
        }
    }

    fclose(file);

    return true;
}

bool game_save_state(GameState* game, const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL)
        return false;

    // jatekter meretenek kiirasa
    fprintf(file, "%lu %lu\n", game->width, game->height);

    // cellak kiirasa
    for(size_t y = 0; y < game->height; y++) {
        for(size_t x = 0; x < game->width; x++) {
            CELL_STATE state = playfield_get(game, x, y);
            fputc(state ? '1' : '0', file);
        }
        fputc('\n', file);
    }

    fclose(file);

    return true;
}

void game_step(GameState* game) {
    // minden cellanak megszamoljuk, hogy hany szomszedja van
    for(int64_t y = 0; y < game->height; y++) {
        for(int64_t x = 0; x < game->width; x++) {
            uint8_t count = 0;
            count += playfield_get(game, x-1, y-1) ? 1 : 0;
            count += playfield_get(game, x, y-1) ? 1 : 0;
            count += playfield_get(game, x+1, y-1) ? 1 : 0;
            count += playfield_get(game, x-1, y) ? 1 : 0;
            count += 0;
            count += playfield_get(game, x+1, y) ? 1 : 0;
            count += playfield_get(game, x-1, y+1) ? 1 : 0;
            count += playfield_get(game, x, y+1) ? 1 : 0;
            count += playfield_get(game, x+1, y+1) ? 1 : 0;

            // ellenorizzuk a jatek szabalyait es cselekszunk
            CELL_STATE state = playfield_get(game, x, y);
            if (count < 2) {
                state = DEAD;
            } else if (2 <= count && count <= 3 && state == ALIVE) {
                state = ALIVE;
            } else if (count > 3) {
                state = DEAD;
            } else if (count == 3 && state == DEAD) {
                state = ALIVE;
            }
            playfield_set(game, x, y, state, true);
        }
    }

    // az ideiglenes es az aktiv jatekter felcserelese
    uint8_t* temp = game->incative_playfield;
    game->incative_playfield = game->playfield;
    game->playfield = temp;
}

// bit meghatarozasa egy cella x,y coordinatai alapjan
size_t calculate_index(GameState* game, int64_t x, int64_t y, size_t* byte_index, size_t* bit_index) {
    x = x >= 0 ? x : game->width + x; 
    y = y >= 0 ? y : game->height + y;

    x = x % game->width;
    y = y % game->height;
    
    size_t index = (game->width * y) + x;
    *byte_index = index / 8;
    *bit_index = index % 8;
    
    return index;
}

CELL_STATE playfield_get(GameState* game, int64_t x, int64_t y) {
    size_t byte_index, bit_index;
    calculate_index(game, x, y, &byte_index, &bit_index);

    uint8_t byte = game->playfield[byte_index];

    return ((byte >> bit_index) & 1) ? ALIVE : DEAD;
}

void playfield_set(GameState* game, int64_t x, int64_t y, CELL_STATE state, bool inactive) {
    size_t byte_index, bit_index;
    calculate_index(game, x, y, &byte_index, &bit_index);
    uint8_t* playfield = inactive ? game->incative_playfield : game->playfield;

    if (state == ALIVE) {
        playfield[byte_index] |= (1 << bit_index);
    } else {
        playfield[byte_index] &= ~(1 << bit_index);
    }
}

void playfield_print(GameState* game, bool clear_screen) {
    // kepernyo torlese, ha szukseges
    if (clear_screen) {
        printf("%s%s", CLEAR_SCREEN, HOME_CURSOR);
    }

    // vegigmegyunk az osszes cellan es kirajzoljuk
    for(size_t y = 0; y < game->height; y++) {
        for(size_t x = 0; x < game->width; x++) {
            printf("%s  %s", playfield_get(game, x, y) ? FULL_WHITE : FULL_BLACK, CLEAR);
        }
        putchar('\n');
    }
}