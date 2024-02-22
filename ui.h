#ifndef UI_H
#define UI_H

#include <stddef.h>

// egy menuopciot jellemzo struct
typedef struct {
    char* label;             // megjeleno felirat
    void (*callback)(void*); // futtatando fuggveny
    void* argument;          // argomentum a futtatando fuggvenynek
} UIOption;

// opciok kiirasa es a felhasznalo valasztasanak feldolgzasa 
void ui_present_options(UIOption* options, size_t count);

#endif