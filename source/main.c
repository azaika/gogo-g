#include <stdio.h>

#include "game_state.h"
#include "gogo_controller.h"

#include <stdbool.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "too many or too short arguments\n");
        return 0;
    }
    
    gogo_controller gc;
    if (!strcmp(argv[1], "0")) {
        init_gogo(&gc, true);
    }
    else if (!strcmp(argv[1], "1")) {
        init_gogo(&gc, false);
    }
    else {
        fprintf(stderr, "argument must be 0 or 1");
        return 0;
    }

    #ifdef DEBUG
    printf("init success!\n");
    printf("(turn %d)\n", gc.turn);
    print_board(*gc.state);
    printf("\n");
    #endif

    while (advance_turn(&gc)) {
        #ifdef DEBUG
        printf("(turn %d)\n", gc.turn);
        print_board(*gc.state);
        printf("\n");
        #endif
    }

    destroy_gogo(&gc);
    
    return 0;
}