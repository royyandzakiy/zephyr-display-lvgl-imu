#include "display.h"

int main(void)
{
    display_init();

    while (1) {
        display_loop();
    }
}