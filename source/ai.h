#ifndef GOGO_HEADER_AI_H
#define GOGO_HEADER_AI_H

#include "game_state.h"
#include "move.h"

#include <stdbool.h>

static move_type ai_decide_move(game_state state, bool is_first) {
    // ToDo: implement
    // テスト用で書き換えてありますが特に意味はありません
    move_type move;
    move.is_drop = false;
    move.piece = PIECE_OU;
    move.from = 24;
    move.to = 19;
    move.do_promote = false;

    return move;
}

#endif // GOGO_HEADER_AI_H