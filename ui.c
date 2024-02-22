#include "ui.h"
#include <stdio.h>
#include <stdbool.h>

void ui_present_options(UIOption *options, size_t count) {
    for (size_t i = 0; i < count; i++) {
        printf("%lu: %s\n", i, options[i].label);
    }

    size_t index;
    while(1 != scanf("%lu", &index) && index >= 0 && index < count) {}

    options[index].callback(options[index].argument);
}