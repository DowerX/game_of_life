#include "game.h"
#include "ui.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// megszakitas (pl. ctrl+c) kezelese,
// folyamatos iteracio megszakitasa
bool work = true;
void interrupt(int signum) {
    work = false;
    putchar('\n');
}

// menuopcio fuggvenyek
void load_state(void* arg) {
    char path[1024];
    do {
        scanf("%s", path);
    } while(!game_load_state((GameState*)arg, path));
    playfield_print((GameState*)arg, true);
}

void save_state(void* arg) {
    char path[1024];
    do {
        scanf("%s", path);
    } while(!game_save_state((GameState*)arg, path));
}

void create_empty(void* arg) {
    size_t width, height;
    while(2 != scanf("%lu %lu", &width, &height));
    game_free(*(GameState*)arg);
    (*(GameState*)arg) = game_allocate(width, height);
    playfield_print((GameState*)arg, false);
}

void set_cell(void* arg) {
    int64_t x, y;
    int state;
    while(3 != scanf("%ld %ld %d", &x, &y, &state) && (state == 0 || state == 1));
    playfield_set((GameState*)arg, x, y, state == 0 ? DEAD : ALIVE, false);
    playfield_print((GameState*)arg, true);
}

void step_one(void* arg) {
    game_step((GameState*)arg);
    playfield_print((GameState*)arg, true);
}

void c99_sleep(float seconds) {
    clock_t start = clock();
    while ((clock() - start) / (float)CLOCKS_PER_SEC < seconds);
}

void step_forever(void* arg) {
    work = true;
    while(work) {
        game_step((GameState*)arg);
        playfield_print((GameState*)arg, true);
        c99_sleep(0.2);
    }
}

void quit(void* arg) {
    game_free(*(GameState*)arg);
    exit(0);
}

// menuopciok kezelese
void present_options(GameState* game) {
    UIOption options[] = {
        {
            "step one iteration",
            &step_one,
            (void*)game
        },
        {
            "step forever",
            &step_forever,
            (void*)game
        },
        {
            "save state",
            &save_state,
            (void*)game
        },
        {
            "load state",
            &load_state,
            (void*)game
        },
        {
            "create empty state",
            &create_empty,
            (void*)game
        },
        {
            "set cell",
            &set_cell,
            (void*)game
        },
        {
            "quit",
            &quit,
            (void*)game
        },
    };
    ui_present_options(options, 7);
}

int main(int argc, char** argv) {
    // megszakitaskezelo beallitasa
    signal(SIGINT, interrupt);

    // ures jatekallapot
    GameState game = {
        0,0,
        NULL, NULL
    };

    // jatek futasa
    while(true) {
        present_options(&game);
    }

    // nincs return, mivel a program soha nem jut el ide
    // a kilepesre a quit(void*) menuopcion keresztul kerul sor
}