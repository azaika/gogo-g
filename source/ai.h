#ifndef GOGO_HEADER_AI_H
#define GOGO_HEADER_AI_H

#include "game_state.h"
#include "move.h"

#include <stdbool.h>

static move_type ai_decide_move(game_state state, bool is_first) {
    // ToDo: implement

    move_type move;
    move.from = 0;
    move.to = 0;
    move.do_promote = false;

    return move;
}

#endif // GOGO_HEADER_AI_H